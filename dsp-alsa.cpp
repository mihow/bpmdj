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
#ifndef __loaded__dsp_alsa_cpp__
#define __loaded__dsp_alsa_cpp__
using namespace std;
#line 1 "dsp-alsa.c++"
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

/*-------------------------------------------
 *         Dsp operations
 *-------------------------------------------*/ 
void dsp_alsa::start()
{
  int err = 0;
  err = snd_pcm_prepare(dsp);
  if (err < 0)
    {
      printf("dsp: problem starting alsa device: %s\n",snd_strerror(err));
    }
}

void dsp_alsa::pause()
{
  int err;
  err = snd_pcm_drop(dsp);
  filled = 0;
  if (err < 0)
    printf("pause drop error: %s\n",snd_strerror(err));
  // wait...
  wait_for_unpause();
  // simply continue
  err = snd_pcm_prepare(dsp);
  if (err < 0)
    printf("dsp: unpause error: %s\n",snd_strerror(err));
}

// the normal write routine without prebuffering...
void dsp_alsa::wwrite(unsigned4 *value)
{
  int err = 0;
  do
    {
      err = snd_pcm_writei(dsp,value,1);
    }
  while (err==0) ;
  assert(err!=-EAGAIN);
  assert(err!=-ESTRPIPE);
  if (err==-EPIPE)
    {
      printf("underrun occured...\n");
      err = snd_pcm_prepare(dsp);
      if (err < 0)
	printf("cant recover from underrun: %s\n",snd_strerror(err));
      return;
    }
}

void dsp_alsa::write(stereo_sample2 value)
{
  buffer[filled]=value.value();
  if (++filled>=period_size)
    {
      int err;
      unsigned4* buf= buffer;
      do 
	{
	  err = snd_pcm_writei(dsp,buf,filled);
	  if (err<0) 
	    {
	      assert(err!=-EAGAIN);
	      assert(err!=-ESTRPIPE);
	      if (err==-EPIPE)
		{
		  printf("underrun occured...\n");
		  filled = 0;
		  err = snd_pcm_prepare(dsp);
		  if (err < 0)
		    printf("cant recover from underrun: %s\n",snd_strerror(err));
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

signed8 dsp_alsa::latency()
{
  snd_pcm_sframes_t delay;
  int err = snd_pcm_delay(dsp,&delay);
  if (err<0)
    {
      printf("error obtaining latency:%d %s\n",err,snd_strerror(err));
      return 0;
    }
  assert(err==0);
  if (verbose)
    printf("delay = %d\n",(int)delay);
  assert(delay >= 0 && (unsigned4)delay <= buffer_size);
  return delay + filled;
}

dsp_alsa::dsp_alsa(const PlayerConfig & config) : dsp_driver(config)
{
  arg_dev = strdup(config.get_alsa_dev());
  dsp = NULL;
  filled = 0;
  buffer_size = 0;
  period_size = 0;
  arg_latency = config.get_alsa_latency();
  verbose = config.get_alsa_verbose();
}

int dsp_alsa::open()
{

  int err;
  unsigned int buffer_time, period_time;
  //  snd_pcm_hw_params_t *hparams;
  snd_pcm_hw_params_t *hparams;
  snd_pcm_sw_params_t *sparams;
  snd_output_t * output = NULL;

  // attach to something ???? 
  // I don't have a clue why this is necessary, but all demo programs do this
  err = snd_output_stdio_attach(&output,stdout,0);
  if (err < 0 )
    {
      printf("attaching to stdio didn't succeed: %s\n",snd_strerror(err));
      return err_dsp;
    }
  // open device
  err = snd_pcm_open(&dsp, arg_dev, SND_PCM_STREAM_PLAYBACK, 0);
  if (err < 0)
    {
      printf("dsp: opening alsa device failed : %s\n",snd_strerror(err));
      return err_dsp;
    }
  
  // set dsp parameters
  snd_pcm_sw_params_alloca(&sparams);
  snd_pcm_hw_params_alloca(&hparams);
  
  err = snd_pcm_hw_params_any(dsp, hparams);
  if (err<0)
    {
      printf("Broken configuration file for pcm : %s\n",snd_strerror(err));
      return err_dsp;
    }
  
  // interleaved reading and writing (in contrast to channel per channel read/write)
  err = snd_pcm_hw_params_set_access(dsp, hparams, SND_PCM_ACCESS_RW_INTERLEAVED);
  if (err<0)
    {
      printf("Unable to set interleaved access to pcm device: %s\n",snd_strerror(err));
      return err_dsp;
    }
  err = snd_pcm_hw_params_set_format(dsp, hparams, SND_PCM_FORMAT_S16);
  if (err<0)
    {
      printf("Unable to set sample format to standard 16 bit: %s\n",snd_strerror(err));
      return err_dsp;
    }
  err = snd_pcm_hw_params_set_channels(dsp, hparams, 2);
  if (err < 0)
    {
      printf("dsp: setting dsp to 2 channels failed : %s\n",snd_strerror(err));
      return err_dsp;
    }
  
  unsigned int q = WAVRATE;
  err = snd_pcm_hw_params_set_rate_near(dsp, hparams, &q, 0);
  if (err < 0)
    {
      printf("dsp: setting dsp speed (%d) failed\n",q);
      return err_dsp;
    }
  if (q != WAVRATE)
    {
      printf("dsp: setting dsp speed (%d) failed, resulting rate = %d \n",WAVRATE, q);
    }

  // playing latency instellen...
  period_time = arg_latency * 1000;
  // period_time /=2; // we do this so we can afterward use multiple periods
  buffer_time = period_time *2;
  {
    unsigned int t = buffer_time;
    int dir = 1;
    err = snd_pcm_hw_params_set_buffer_time_near(dsp,hparams,&t,&dir);
    if (err<0)
      {
	printf("     impossible to set pcm buffer time to %i (%i): %s\n", buffer_time,t,snd_strerror(err));
	return err_dsp;
      }
    
    err = snd_pcm_hw_params_get_buffer_size(hparams,&buffer_size);
    if (err<0)
      {
	printf("     impossible to obtain buffer size: %s\n",snd_strerror(err));
	return err_dsp;
      }
    
    t = period_time;
    err = snd_pcm_hw_params_set_period_time_near(dsp, hparams,&t,&dir);
    if (err<0)
      {
	printf("     impossible to set pcm period time to %i (%i): %s\n", period_time,t,snd_strerror(err));
	return err_dsp;
      }
    err = snd_pcm_hw_params_get_period_size(hparams,&period_size,&dir);
    if (err<0)
      {
	printf("     ompossible to obtain period data size: %s\n",snd_strerror(err));
	return err_dsp;
      }
    if (period_size*2 - 1>buffer_size)
      {
	printf("     impossible to allocate large enough buffers. Please decrease the latency\n");
	printf("       playbuffer size = %li\n",period_size);
	printf("       total buffer    = %li\n",buffer_size);
	return err_dsp;
      }
    dir = 0;
  }
  err = snd_pcm_hw_params(dsp,hparams);
  if (err < 0)
    {
      printf("unable to set hw parameters for pcm: %s\n",snd_strerror(err));
      return err_dsp;
    }
  
  // nu moeten we nog de stop & start delays goed zetten (dit zijn de software parameters)
  err = snd_pcm_sw_params_current(dsp,sparams);
  if (err < 0)
    {
      printf("unable to determine sw parameters for pcm: %s\n",snd_strerror(err));
      return err_dsp;
    }
  err = snd_pcm_sw_params_set_start_threshold(dsp,sparams,buffer_size);
  if (err < 0)
    {
      printf("unable to set start to treshold mode: %s\n",snd_strerror(err));
      return err_dsp;
    }
  err = snd_pcm_sw_params_set_avail_min(dsp,sparams,period_size);
  if (err < 0 )
    {
      printf("unable to set minimum start size: %s\n",snd_strerror(err));
      return err_dsp;
    }
  /*
  err = snd_pcm_sw_params_set_xfer_align(dsp,sparams,1);
  if (err < 0 )
    {
      printf("unable to set transfer alignation: %s\n",snd_strerror(err));
      return err_dsp;
    }      
  */
  err = snd_pcm_sw_params(dsp,sparams);
  if (err < 0)
    {
      printf("unable to set software parameters: %s\n",snd_strerror(err));
      return err_dsp;
    }
  if (verbose)
    snd_pcm_dump(dsp,output);
  buffer = bpmdj_allocate(period_size,unsigned4);
  
  start();
  
  //  snd_pcm_hw_params_free(hparams);
  //  snd_pcm_sw_params_free(sparams);
  return err_none;
}

void dsp_alsa::close(bool flush_first)
{
  if (dsp)
    {
      // To flush the dsp we want to make sure that everyting that
      // can be played is played. So we add a colleciton of zeros to
      // make sure a flush occurs.
      if (flush_first)
	{
	  stereo_sample2 zero(0,0);
	  for(unsigned i = 0 ; i < period_size; i++)
	    write(zero);
	  snd_pcm_drain(dsp);
	}
      int err = snd_pcm_close(dsp);
      if (err < 0)
	printf("cant close pcm device: %s\n",snd_strerror(err));
      dsp=0;
    }
}

#endif
#endif // __loaded__dsp_alsa_cpp__
