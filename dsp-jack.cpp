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
#include <sched.h>
#include <set>
#include "dsp-jack.h"
#include "player-core.h"
#include "version.h"
#include "memory.h"
#include "scripts.h"
#include "lock.h"

typedef jack_default_audio_sample_t sample_t;
pthread_mutex_t thread_lock = PTHREAD_MUTEX_INITIALIZER;

/* Stolen from alsaplayer */
const float4 SAMPLE_MAX_16BIT =32768.0f;

void sample_move_dS_s16 (sample_t *dst, char *src, unsigned long nsamples, unsigned long src_skip) 
{
  while (nsamples--) 
    {
      *dst = (*((signed2 *) src)) / SAMPLE_MAX_16BIT;
      dst++;
      src += src_skip;
    }
}     

void dsp_jack::start(audio_source* s)
{
  Debug("dsp_jack::start called");
  
  if (synchronous)
    dsp_driver::start(s);
  else
    {
      audio=s;
      stop_request=false;
      stopped=false;
    }
}

void dsp_jack::internal_pause()
{
  pthread_mutex_lock (&thread_lock);
  if (synchronous)
    memset(buffer, 0, buffer_size*sizeof(unsigned4));
  filled = position = 0;
  pthread_mutex_unlock (&thread_lock);
}

void dsp_jack::internal_unpause()
{
  position=latency();
}

void dsp_jack::write(stereo_sample2 value)
{
  assert(synchronous);
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
    Info("latency = %lld",l);
  return l;
} 
 
#ifdef DEBUG_WAIT_STATES
static Lock jack_set_lock;
set<dsp_jack*> jack_dsps;
#endif

dsp_jack::~dsp_jack()
{
#ifdef DEBUG_WAIT_STATES
  Debug("Removing the dsp_jack object");
  Synchronized(jack_set_lock);
  jack_dsps.erase(this);
#endif
}

/**
 * The callback called from within the jack thread. We simply pass the control flow through to 
 * the jack dsp driver which will handle the request in a better manner.
 */
int chunk_getter(jack_nframes_t nframes, void *dsp) 
{
  dsp_jack* dj=(dsp_jack*)dsp;
  assert(dj);
#ifdef DEBUG_WAIT_STATES
  // WVB -- I suspect we might have a concurrencvy problem here. Suppose the jack driver is stopped and 
  // the dsp_jack object deleted. If a callback comes in then we have a problem with the callback because
  // the object referred to no longer exists at all.
  // I added a quick check here to see whether this occurs and if it does it will crash.
  {
    Synchronized(jack_set_lock);
    assert(jack_dsps.find(dj)!=jack_dsps.end());
  }
#endif
  return dj->audio_process(nframes);
}

dsp_jack::dsp_jack(const PlayerConfig & config) : dsp_driver(config)
{
#ifdef DEBUG_WAIT_STATES
  {
    Synchronized(jack_set_lock);
    jack_dsps.insert(this);
  }
#endif
  arg_dev = strdup(config.get_jack_dev());
  verbose = config.get_jack_verbose();
  synchronous = !config.get_jack_lowlatency();
  if (verbose)
    Debug("dsp_jack::<constructor> called\n"
	  "  device = %s\n"
	  "  synchronous = %d",arg_dev,synchronous);
}

int dsp_jack::fill_empty_buffer(jack_nframes_t nframes) 
{
  sample_t *b1 = (sample_t *) jack_port_get_buffer (output_port_1, nframes);
  sample_t *b2 = (sample_t *) jack_port_get_buffer (output_port_2, nframes);
  memset(b1, 0, nframes*(sizeof(sample_t)));
  memset(b2, 0, nframes*(sizeof(sample_t)));
  jack_time += nframes;
  return 0;
}

int dsp_jack::generate_buffer(jack_nframes_t nframes) 
{
  if (get_stopped()) return fill_empty_buffer(nframes);
  sample_t *b1 = (sample_t *) jack_port_get_buffer (output_port_1, nframes);
  sample_t *b2 = (sample_t *) jack_port_get_buffer (output_port_2, nframes);
  for (jack_nframes_t k = 0; k < nframes; k++) 
    {
      if (paused)
	b2[k] = b1[k] = 0;
      else
	{
	  stereo_sample2 value=audio->read();
	  b1[k] = value.left / SAMPLE_MAX_16BIT;
	  b2[k] = value.right / SAMPLE_MAX_16BIT;
	}
    }
  jack_time += nframes;
  return 0;
}

int dsp_jack::audio_process (jack_nframes_t nframes) 
{
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_jack::audio_process() entered");
#endif
  int retval;
  // client = NULL when the player has been closed
  if (!client) retval=0;
  // if the player stopped we can directly dump zeros
  else if (stopped)
    retval=fill_empty_buffer(nframes);
  // if it was synchronous we need to take it from our own internal buffer
  else if (synchronous)
    retval=get_buffer(nframes);
  // otherwise we generate the data on the fly
  else 
    retval=generate_buffer(nframes);
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_jack::audio_process() finished");
#endif
  return retval;
}

void dsp_jack::stop()
{
  if (synchronous) 
    {
      dsp_driver::stop();
      return;
    }
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_jack_stop(): entered");
#endif
  stopped=true;
  stop_request=false;
  // close the dsp device immediatelly
  close(true);
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_jack_stop(): finished");
#endif
}

int dsp_jack::get_buffer(jack_nframes_t nframes) 
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
      sample_move_dS_s16(b1, (char *) (buffer + position), nframes, sizeof(signed4));
      sample_move_dS_s16(b2, (char *) (buffer + position) + sizeof(signed2), nframes, sizeof(signed4));
      position += nframes;
    }
  else if (position < filled)
    {
      assert(filled-position<nframes);
      int todo=filled-position;
      sample_move_dS_s16(b1, (char *) (buffer + position), todo, sizeof(signed4));
      sample_move_dS_s16(b2, (char *) (buffer + position) + sizeof(signed2), todo, sizeof(signed4));
      position += todo;
      memset(b1+todo, 0, (nframes-todo)*(sizeof(sample_t)));
      memset(b2+todo, 0, (nframes-todo)*(sizeof(sample_t)));
    }
  else 
    {
      memset(b1, 0, nframes*(sizeof(sample_t)));
      memset(b2, 0, nframes*(sizeof(sample_t)));
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

int dsp_jack::open(bool ui)
{ 
  Debug("dsp_jack::open called");
  
  /* we want realtime scheduling */
  int priority = sched_get_priority_max(SCHED_FIFO);
  sched_param sp;
  sp.sched_priority = priority;
  sched_setscheduler(getpid(), SCHED_FIFO, &sp);
  mlockall(MCL_FUTURE | MCL_CURRENT);

  if ((client = jack_client_new (arg_dev)) == 0) 
    {
      Error(ui,"The jack server (jackd) should be running. Is it ?");
      return err_dsp;
    }
  
  if (verbose) 
    Info("engine sample rate: %d",jack_get_sample_rate (client));
  
  if (jack_get_sample_rate (client) != 44100) 
    {
      Error(ui,"engine sample rate: %d\n"
	    "Please be sure jackd is running at 44100",
	    jack_get_sample_rate (client));
      return err_dsp;
    }
  buffer_size = 100000;
  filled = position = 0;
  buffer = bpmdj_allocate(buffer_size,unsigned4);
  
  /* opening jack port */
  output_port_1 = jack_port_register (client, "output_1", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  output_port_2 = jack_port_register (client, "output_2", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  jack_set_process_callback (client, chunk_getter, this);
  
  /* activating client */
  if (jack_activate (client)) 
    {
      Error(ui,"Cannot activate jack client");
      return err_dsp;
    }      
  return err_none;
}

void dsp_jack::close(bool flush_first)
{
  if (!client)
    Debug("dsp_jack::close unnecessary");
  else 
    {
      Debug("dsp_jack::close called");
      jack_client_close(client);
      client=NULL;
    }
}

bool is_jack_driver()
{
  return typeid(*dsp) == typeid(dsp_jack);
}
#endif
#endif // __loaded__dsp_jack_cpp__
