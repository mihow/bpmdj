/****
 BpmDj v3.8: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****/
#ifndef __loaded__dsp_jack_cpp__
#define __loaded__dsp_jack_cpp__
using namespace std;
#line 1 "dsp-jack.c++"
#ifdef COMPILE_JACK
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <qdialog.h>
#include <qstring.h>
#include "player-core.h"
#include "version.h"
#include "dsp-jack.h"
#include "memory.h"

typedef jack_default_audio_sample_t sample_t;
pthread_mutex_t thread_lock = PTHREAD_MUTEX_INITIALIZER;

/* Stolen from alsaplayer */
#define SAMPLE_MAX_16BIT  32768.0f

void sample_move_dS_s16 (sample_t *dst, char *src,
			 unsigned long nsamples, unsigned long src_skip) 
{
  /* ALERT: signed sign-extension portability !!! */
  while (nsamples--) {
    *dst = (*((short *) src)) / SAMPLE_MAX_16BIT;
    dst++;
    src += src_skip;
  }
}     

/*-------------------------------------------
 *         Dsp operations
 *-------------------------------------------*/ 
void dsp_jack::start()
{
  printf("dsp_jack::start called\n");
}

void dsp_jack::pause()
{
  // stop the device output
  pthread_mutex_lock (&thread_lock);
  memset(buffer, 0, buffer_size*sizeof(unsigned4));
  filled = position = 0;
  position = latency();
  pthread_mutex_unlock (&thread_lock);
  wait_for_unpause();
  // WVB - it is better to check the latency when the unpause call
  // returns because the latency before the pause is very unlikely 
  // the same as the latency after the pause.
  // BF - but for some strange reason, it leads to overestimation of the latency
  // (or too many frames skipped).
}

void dsp_jack::write(stereo_sample2 value)
{
  /* Jack is not using blocking I/O, so everything is buffered until 
     the callback is called */
  pthread_mutex_lock (&thread_lock);
  buffer[filled++]=value.value();
  // WVB - i don't think this should happen, since you nicely wait 
  // with the usleep below. It can normally happen only when the
  // consumer threat does not eat the data sufficient, but how 
  // this could happen is not yet clear to me at the moment. 
  // If it does we should know when and why it happens and solve it
  assert(filled<=buffer_size);
  pthread_mutex_unlock (&thread_lock);
  if (filled > position) 
    while (filled - position > 4096) 
      usleep(10);
}

signed8 dsp_jack::latency()
{
  signed8 l = (signed4)jack_port_get_total_latency(client,output_port_1)
    + ((signed4)filled - (signed4)position); 
  if (verbose)
    printf("latency = %lld\n",l);
  return l;
} 
 
dsp_jack::dsp_jack(const PlayerConfig & config) : dsp_driver(config)
{
  arg_dev = strdup(config.get_jack_dev());
  verbose = config.get_jack_verbose();
  printf("dsp_jack::<constructor> called\n");
  printf("  device = %s\n",arg_dev);
  printf("  verbose = %d\n",verbose);
}

/**
 * The callback called from within the jack thread. We simply pass the control flow through to 
 * the jack dsp driver which will handle the request in a better manner.
 */
int process (jack_nframes_t nframes, void *dsp) 
{
  return ((dsp_jack *) dsp)->audio_process(nframes);
}

int dsp_jack::audio_process (jack_nframes_t nframes) 
{
  sample_t *b1 = (sample_t *) jack_port_get_buffer (output_port_1, nframes);
  sample_t *b2 = (sample_t *) jack_port_get_buffer (output_port_2, nframes);

  // WVB - I moved this to the front to avoid a change in the position and filled
  // variables during updating of them
  pthread_mutex_lock (&thread_lock);
  
  // WVB - I modified this a bit to handle the cases where position < filled and position + nframes > filled
  // the idea is to copy everything necessary.
  if (position + nframes <= filled) 
    {
      sample_move_dS_s16(b1, (char *) (buffer + position), nframes, sizeof(short) << 1);
      sample_move_dS_s16(b2, (char *) (buffer + position) + sizeof(short), nframes, sizeof(short) << 1);
      position += nframes;
    }
  else if (position < filled)
    {
      assert(filled-position<nframes);
      int todo=filled-position;
      sample_move_dS_s16(b1, (char *) (buffer + position), todo, sizeof(short) << 1);
      sample_move_dS_s16(b2, (char *) (buffer + position) + sizeof(short), todo, sizeof(short) << 1);
      position += todo;
      memset(b1+todo, 0, (nframes-todo)*(sizeof(short) << 1));
      memset(b2+todo, 0, (nframes-todo)*(sizeof(short) << 1));
    }
  else 
    {
      memset(b1, 0, nframes*(sizeof(short) << 1));
      memset(b2, 0, nframes*(sizeof(short) << 1));
    }
  
  if ((filled > position) && (position > buffer_size / 2)) 
    {
      // WVB - there is a small reason to start allocating new buffers
      // although I believe we can be much more optimal here by using 
      // some modulo arithmetic.
      unsigned4 *newbuf = bpmdj_allocate(buffer_size,unsigned4);  
      memcpy(newbuf, buffer + position, (filled - position)*sizeof(sample_t));
      bpmdj_deallocate(buffer);
      buffer = newbuf;
      filled -= position;
      position = 0;
    }
  pthread_mutex_unlock (&thread_lock);
  return 0;
}

int dsp_jack::open()
{
  printf("dsp_jack::open called\n");
  /**
   * This routine must set the playing speed to WAVRATE
   * 2 channels and 16 bit.
   */
  if ((client = jack_client_new (arg_dev)) == 0) 
    {
      fprintf (stderr, "jack server not running?\n");
      return err_dsp;
    }
  
  if (verbose) 
    {
      printf ("engine sample rate: %d\n",
	      jack_get_sample_rate (client));
    }
  
  if (jack_get_sample_rate (client) != 44100) {
    printf("Please be sure jackd is running at 44100\n");
    return err_dsp;
  }
  buffer_size = 100000;
  filled = position = 0;
  buffer = bpmdj_allocate(buffer_size,unsigned4);

  /* opening jack port */
  output_port_1 = jack_port_register (client, "output_1", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  output_port_2 = jack_port_register (client, "output_2", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

  jack_set_process_callback (client, process, this);
  
  /* activating client */
  if (jack_activate (client)) 
    {
      fprintf (stderr, "cannot activate client");
      return err_dsp;
    }      
  
  start();
  return err_none;
}

void dsp_jack::close(bool flush_first)
{
  printf("dsp_jack::close called\n");
  jack_client_close (client);
}

#endif
#endif // __loaded__dsp_jack_cpp__
