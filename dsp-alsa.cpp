/****
 BpmDj v4.2-pl2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

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
#ifndef __loaded__dsp_alsa_cpp__
#define __loaded__dsp_alsa_cpp__
using namespace std;
#ifdef COMPILE_ALSA
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <qdialog.h>
#include <qstring.h>
#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API
#include "player-core.h"
#include "version.h"
#include "dsp-alsa.h"
#include "memory.h"
#include "scripts.h"
#include "info.h"

void dsp_alsa::start(audio_source * a)
{
  int err = 0;
  err = snd_pcm_prepare(dsp);
  if (err < 0)
    Error(true,"dsp: problem starting alsa device: %s",snd_strerror(err));
  dsp_driver::start(a);
}

void dsp_alsa::internal_pause()
{
  int err = snd_pcm_drop(dsp);
  filled = 0;
  if (err < 0)
    Warning("pause drop error: %s",snd_strerror(err));
}

void dsp_alsa::internal_unpause()
{
  int err = snd_pcm_prepare(dsp);
  if (err < 0)
    Warning("dsp: unpause error: %s",snd_strerror(err));
}

void dsp_alsa::write(stereo_sample2 value)
{
  buffer[filled]=value;
  if (++filled>=period_size)
    {
      int err;
      unsigned4* buf= (unsigned4*)buffer;
      do 
	{
	  err = snd_pcm_writei(dsp,buf,filled);
	  if (err<0) 
	    {
	      assert(err!=-EAGAIN);
	      assert(err!=-ESTRPIPE);
	      if (err==-EPIPE)
		{
		  Warning("underrun occurred...");
		  filled = 0;
		  err = snd_pcm_prepare(dsp);
		  if (err < 0)
		    Error(true,"cant recover from underrun: %s",
			  snd_strerror(err));
		  return;
		}
	    }
	  filled-=err;
	  buf+=err;
	}
      while (filled);
      filled = 0;
    }
}

/**
 * Because the latency call access the device from a concurrent thread 
 * (the other one is the sound pusher thread), it might happen that the
 * return value of the snd_pcm_delay call is crap. In that case we simply
 * try again.
 */
signed8 dsp_alsa::latency()
{
  
  snd_pcm_sframes_t delay;
  while(true)
    {
      int err = snd_pcm_delay(dsp,&delay);
      if (err<0)
	{
	  Warning("error obtaining latency:%d %s",err,snd_strerror(err));
	  return 0;
	}
      if (delay >= 0 && (unsigned4)delay <= buffer_size)
	return delay + filled;
    }
}

dsp_alsa::dsp_alsa(const PlayerConfig & config) : dsp_driver(config)
{
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_alsa::dsp_alsa<constructor>");
#endif
  arg_dev = strdup(config.get_alsa_dev().toAscii().data());
  dsp = NULL;
  filled = 0;
  buffer_size = 0;
  period_size = 0;
  arg_latency = config.get_alsa_latency();
}

int dsp_alsa::open(bool ui)
{
  int err;
  unsigned int buffer_time, period_time;
  snd_output_t * output = NULL;
  
  // I don't have a clue why this is necessary, but all demo programs do this
  err = snd_output_stdio_attach(&output,stdout,0);
  if (err < 0 )
    {
      Error(ui,"attaching to stdio didn't succeed: %s",snd_strerror(err));
      return err_dsp;
    }
  
  // open device
  err = snd_pcm_open(&dsp, arg_dev, SND_PCM_STREAM_PLAYBACK, 0);
  if (err < 0)
    {
      Error(ui,"dsp: opening alsa device failed : %s",snd_strerror(err));
      return err_dsp;
    }
  
  // set dsp parameters
  snd_pcm_sw_params_alloca(&sparams);
  snd_pcm_hw_params_alloca(&hparams);
  
  err = snd_pcm_hw_params_any(dsp, hparams);
  if (err<0)
    {
      Error(ui,"Broken configuration file for PCM : %s",snd_strerror(err));
      return err_dsp;
    }
  
  /**
   * Interleaved reading and writing (in contrast to channel per channel 
   * read/write)
   */
  err = snd_pcm_hw_params_set_access(dsp, hparams, 
				     SND_PCM_ACCESS_RW_INTERLEAVED);
  if (err<0)
    {
      Error(ui,"Unable to set interleaved access to PCM device: %s",
	    snd_strerror(err));
      return err_dsp;
    }
  err = snd_pcm_hw_params_set_format(dsp, hparams, SND_PCM_FORMAT_S16);
  if (err<0)
    {
      Error(ui,"Unable to set sample format to standard 16 bit: %s",
	    snd_strerror(err));
      return err_dsp;
    }
  err = snd_pcm_hw_params_set_channels(dsp, hparams, 2);
  if (err < 0)
    {
      Error(ui,"dsp: setting dsp to 2 channels failed : %s",snd_strerror(err));
      return err_dsp;
    }
  
  unsigned int q = playrate;
  err = snd_pcm_hw_params_set_rate_near(dsp, hparams, &q, 0);
  if (err < 0)
    {
      Error(ui,"dsp: setting dsp speed (%d) failed",q);
      return err_dsp;
    }
  if (q != (unsigned)playrate)
    {
      Warning("dsp: setting dsp speed (%d) failed, resulting rate = %d ",
	      playrate, q);
      playrate=q;
    }
  
  // playing latency ....
  period_time = arg_latency * 1000;
  buffer_time = period_time *2;
  {
    unsigned int t = buffer_time;
    int dir = 1;
    err = snd_pcm_hw_params_set_buffer_time_near(dsp,hparams,&t,&dir);
    if (err<0)
      {
	Error(ui,"dsp-alsa: Impossible to set PCM buffer time to %i (%i): %s", 
	      buffer_time,t,snd_strerror(err));
	return err_dsp;
      }
    
    err = snd_pcm_hw_params_get_buffer_size(hparams,&buffer_size);
    if (err<0)
      {
	Error(ui,"     impossible to obtain buffer size: %s",snd_strerror(err));
	return err_dsp;
      }
    
    t = period_time;
    err = snd_pcm_hw_params_set_period_time_near(dsp, hparams,&t,&dir);
    if (err<0)
      {
	Error(ui,"     impossible to set PCM period time to %i (%i): %s", 
	      period_time,t,snd_strerror(err));
	return err_dsp;
      }
    err = snd_pcm_hw_params_get_period_size(hparams,&period_size,&dir);
    if (err<0)
      {
	Error(ui,"     impossible to obtain period data size: %s",
	      snd_strerror(err));
	return err_dsp;
      }
    if (period_size*2 - 1>buffer_size)
      {
	Error(ui,"The alsa driver cannot allocate sufficiently large buffers "
	      "due to the large requested latency. (playbuffer size   = %li, "
	      "total buffer size = %li). Try decreasing the latency.",
	      period_size,buffer_size);
	return err_dsp;
      }
    dir = 0;
  }
  err = snd_pcm_hw_params(dsp,hparams);
  if (err < 0)
    {
      Error(ui,"unable to set hardware parameters for PCM: %s",
	    snd_strerror(err));
      return err_dsp;
    }
  
  err = snd_pcm_sw_params_current(dsp,sparams);
  if (err < 0)
    {
      Error(ui,"unable to determine software parameters for PCM: %s",
	    snd_strerror(err));
      return err_dsp;
    }
  err = snd_pcm_sw_params_set_start_threshold(dsp,sparams,buffer_size);
  if (err < 0)
    {
      Error(ui,"unable to set start to threshold mode: %s",
	    snd_strerror(err));
      return err_dsp;
    }
  err = snd_pcm_sw_params_set_avail_min(dsp,sparams,period_size);
  if (err < 0 )
    {
      Error(ui,"unable to set minimum start size: %s",snd_strerror(err));
      return err_dsp;
    }
  err = snd_pcm_sw_params(dsp,sparams);
  if (err < 0)
    {
      Error(ui,"unable to set software parameters: %s",snd_strerror(err));
      return err_dsp;
    }
  if (verbose)
    snd_pcm_dump(dsp,output);
  buffer = bpmdj_allocate(period_size,stereo_sample2);
  return err_none;
}

void dsp_alsa::close(bool flush_first)
{
  if (!dsp) return;
  // To flush the dsp we want to make sure that everything that
  // can be played is played. So we add a collection of zeros to
  // make sure a flush occurs.
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_alsa::close(%d) entered",flush_first);
#endif
  if (flush_first)
    {
      stereo_sample2 zero(0,0);
      for(unsigned i = 0 ; i < period_size; i++)
	write(zero);
      snd_pcm_drain(dsp);
    }
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_alsa::close() close call");
#endif
  int err = snd_pcm_close(dsp);
  if (err < 0)
    Warning("cant close PCM device: %s",snd_strerror(err));
#ifdef DEBUG_WAIT_STATES
  Debug("dsp_alsa::close() finished");
#endif
  dsp=0;
}
#endif
#endif // __loaded__dsp_alsa_cpp__
