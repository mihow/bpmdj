/****
 BpmDj v4.2 beta: Free Dj Tools
 Copyright (C) 2001-2010 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.

 See the authors.txt for a full list of people involved.
****/
#ifndef __loaded__dsp_jack_cpp__
#define __loaded__dsp_jack_cpp__
using namespace std;
#line 1 "dsp-jack.c++"
#ifdef COMPILE_JACK
#include <stdlib.h>
#include <cstdio>
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
#include "info.h"

pthread_mutex_t thread_lock = PTHREAD_MUTEX_INITIALIZER;

void dsp_jack::start(audio_source* s)
{
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_jack::start called");
#endif
  
  audio=s;
  stop_request=false;
  stopped=false;
#ifndef NO_AUTOSTART
  paused=false;
#else
  paused=true;
#endif
}

signed8 dsp_jack::latency()
{
  signed8 l = (signed4)jack_port_get_total_latency(client,output_port_1)
    + jack_ringbuffer_read_space(buf1) / sizeof(sample_t);
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
 * The callback called from within the jack thread. We simply pass the control 
 * flow through to the jack dsp driver which will handle the request in a better
 * manner.
 */
int chunk_getter(jack_nframes_t nframes, void *dsp) 
{
  dsp_jack* dj=(dsp_jack*)dsp;
  assert(dj);
#ifdef DEBUG_WAIT_STATES
  /**
   * WVB -- I suspect we might have a concurrency problem here. Suppose the 
   * jack driver is stopped and the dsp_jack object deleted. If a callback 
   * comes in then we have a problem with the callback because the object 
   * referred to no longer exists at all. I added a quick check here to see 
   * whether this occurs and if it does it will crash.
   */
  {
    Synchronized(jack_set_lock);
    assert(jack_dsps.find(dj)!=jack_dsps.end());
  }
#endif
  return dj->audio_process(nframes);
}

int buffer_size_callback(jack_nframes_t nframes, void *arg) 
{
  dsp_jack* dj=(dsp_jack*)dsp;
  return dj->set_buffer_size(nframes);
}

int dsp_jack::set_buffer_size(jack_nframes_t nframes)
{
  if (bufsize) 
    {
      jack_ringbuffer_free(buf1);
      jack_ringbuffer_free(buf2);
    }
  bufsize = nframes;
  minfill = 8*bufsize*sizeof(sample_t);
  buf1 = jack_ringbuffer_create(2*minfill);
  buf2 = jack_ringbuffer_create(2*minfill);
  return 0;
}

dsp_jack::dsp_jack(const PlayerConfig & config) : dsp_driver(config)
{
#ifdef DEBUG_WAIT_STATES
  {
    Synchronized(jack_set_lock);
    jack_dsps.insert(this);
  }
#endif
  arg_dev = strdup(config.get_jack_dev().toAscii().data());
  lout = strdup(config.get_jack_lout().toAscii().data());
  rout = strdup(config.get_jack_rout().toAscii().data());
  verbose = config.get_jack_verbose();
  bufsize = 0;
  if (verbose)
    Debug("dsp_jack::<constructor> called\n"
	  "  device = %s\n", arg_dev);
}

int dsp_jack::fill_empty_buffer(jack_nframes_t nframes) 
{
  sample_t *b1 = (sample_t *) jack_port_get_buffer (output_port_1, nframes);
  sample_t *b2 = (sample_t *) jack_port_get_buffer (output_port_2, nframes);
  memset(b1, 0, nframes*(sizeof(sample_t)));
  memset(b2, 0, nframes*(sizeof(sample_t)));
  return 0;
}

void * pbuffers(void* dsp)
{
  dsp_jack* dj=(dsp_jack*)dsp;
  dj->process_buffers();
  return NULL;
}

void dsp_jack::process_buffers()
{
  while (1) 
    {    
      unsigned8 filled = jack_ringbuffer_read_space(buf1);
      if ((filled < minfill) && (!get_stopped()) && (!paused)) 
	{
	  while (filled < minfill)
	    {
	      stereo_sample2 value=audio->read();
	      sample_t val;
	      val = value.left / signed2_sample_max_f;
	      jack_ringbuffer_write(buf1, (const char *)&val, sizeof(sample_t));
	      val = value.right / signed2_sample_max_f;
	      jack_ringbuffer_write(buf2, (const char *)&val, sizeof(sample_t));
	      filled += 2*sizeof(sample_t);
	    }
	}
      if (get_stopped() || paused) 
	{
	  jack_ringbuffer_reset(buf1);
	  jack_ringbuffer_reset(buf2);
	}      
      usleep(100);
  }
}

int dsp_jack::generate_buffer(jack_nframes_t nframes) 
{
  size_t nbytes = nframes*sizeof(sample_t);
  if ((jack_ringbuffer_read_space(buf1) < nbytes) 
      && (!get_stopped()) && (!paused))
    Debug("dsp_jack: Data not ready");
  if (get_stopped() || paused || (jack_ringbuffer_read_space(buf1)  < nbytes)) 
    return fill_empty_buffer(nframes);
  sample_t *b1 = (sample_t *) jack_port_get_buffer (output_port_1, nframes);
  sample_t *b2 = (sample_t *) jack_port_get_buffer (output_port_2, nframes);
  jack_ringbuffer_read(buf1, (char *)b1, nbytes);
  jack_ringbuffer_read(buf2, (char *)b2, nbytes);
  return 0;
}

int dsp_jack::audio_process (jack_nframes_t nframes) 
{
  int retval;
  // client = NULL when the player has been closed
  if (!client) retval=0;
  // if the player stopped we can directly dump zeros
  else if (stopped)
    retval=fill_empty_buffer(nframes);
  // otherwise we generate the data on the fly
  else 
    retval=generate_buffer(nframes);
  return retval;
}

void dsp_jack::stop()
{
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_jack_stop(): entered");
#endif
  stopped=true;
  stop_request=false;
  // close the dsp device immediately
  close(true);
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_jack_stop(): finished");
#endif
}

int dsp_jack::open(bool ui)
{ 
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_jack::open called");
#endif

  if ((client = jack_client_open (arg_dev, JackNullOption, NULL)) == 0) 
    {
      Error(ui,"The jack server (jackd) should be running. Is it ?");
      return err_dsp;
    }

  if (verbose) 
    Info("engine sample rate: %d",jack_get_sample_rate (client));
  
  if (jack_get_sample_rate (client) != WAVRATE) 
    {
      Error(ui,"engine sample rate: %d\n"
	    "Please be sure jackd is running at %d",
	    jack_get_sample_rate (client),
	    WAVRATE);
      return err_dsp;
    }
  

  /* opening jack port */
  output_port_1 = jack_port_register (client, "output_1", 
		      JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  output_port_2 = jack_port_register (client, "output_2", 
		      JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

  jack_set_process_callback (client, chunk_getter, this);

  set_buffer_size(jack_get_buffer_size(client));

  /* starting buffering thread */
  pthread_t *jt = bpmdj_allocate(1,pthread_t);
  pthread_create(jt,NULL,pbuffers,(void *)this);

  /* activating client */
  if (jack_activate (client)) 
    {
      Error(ui,"Cannot activate jack client");
      return err_dsp;
    }      

  char dstport[100];
  sprintf(dstport, "%s:output_1", arg_dev);
  jack_connect(client, dstport, lout);
  sprintf(dstport, "%s:output_2", arg_dev);
  jack_connect(client, dstport, rout);


  return err_none;
}

void dsp_jack::close(bool flush_first)
{
  if (!client) {
#ifdef DEBUG_WAIT_STATES
    Debug("dsp_jack::close unnecessary");
#endif
    ;;
  }
  else 
    {
#ifdef DEBUG_WAIT_STATES
      Debug("dsp_jack::close called");
#endif
      jack_deactivate(client);
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
