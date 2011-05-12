/****
 BpmDj v4.1pl1: Free Dj Tools
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
#ifndef __loaded__dsp_oss_cpp__
#define __loaded__dsp_oss_cpp__
using namespace std;
#line 1 "dsp-oss.c++"
#ifdef COMPILE_OSS
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <qstring.h>
#include <qdialog.h>
#include <libgen.h>
#include <linux/soundcard.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include "player-core.h"
#include "version.h"
#include "scripts.h"
#include "dsp-oss.h"
#include "scripts.h"
#include "info.h"

/*-------------------------------------------
 *         Clock operations
 *-------------------------------------------*/
static clock_t starttick;
void clock_start()
{
   starttick=times(NULL);
}

signed8 clock_ticks()
{
  // clock ticks are expressed in WAVRATE resolution
  signed8 ticks=(signed8)times(NULL)-(signed8)starttick;
  signed8 answer=ticks*(signed8)WAVRATE/(signed8)CLOCK_FREQ;
  assert(answer>=0);
  return answer;
}

/*-------------------------------------------
 *         Dsp operations
 *-------------------------------------------*/ 
dsp_oss::dsp_oss(const PlayerConfig & config) : dsp_driver(config)
{
  opt_fragments = config.get_oss_init_fragments();
  arg_dsp = strdup(config.get_oss_dsp().toAscii().data());
  arg_fragments = config.get_oss_fragments();
  opt_nolatencyaccounting = config.get_oss_nolatencyaccounting();
  dsp_writecount=0;
  arg_latency = config.get_oss_latency();
  verbose = config.get_oss_verbose();
}

void dsp_oss::start(audio_source*a)
{
  dsp_writecount=0;
  dsp_driver::start(a);
}

signed8 dsp_oss::playcount()
{
  count_info cnt;
  //  audio_errinfo err;
  if (ioctl(dsp,SNDCTL_DSP_GETOPTR,&cnt)==-1)
    return dsp_writecount+(dsp_latency.bytes/2);
  // if (ioctl(dsp,SNDCTL_DSP_GETERRORS,&err)!=-1)
  // if (err.play_underruns>0)
  // cnt.bytes-=err.play_underruns;
  if (cnt.bytes<0)
    cnt.bytes=0;
  return cnt.bytes/4;
}

void dsp_oss::internal_pause()
{
  //  dsp_flush();
  ioctl(dsp,SNDCTL_DSP_RESET);
}

void dsp_oss::internal_unpause()
{
  clock_start();
  dsp_writecount=0;
}

void dsp_oss::write(stereo_sample2 value)
{
  dsp_writecount++;
  ::write(dsp,&value,4);
}

signed8 dsp_oss::latency()
{
  if (opt_nolatencyaccounting)
    return dsp_writecount - (dsp_latency.bytes/4);
  else
    return dsp_writecount - playcount();
}

void oss_catch(int ignore)
{
  Fatal("Failed to open (alarm timeout) oss dsp device");
}

int dsp_oss::open(bool ui)
{
  int p;
  // start alarm before opening...
  // open int, and if it doesn't answer in time kill it
  signal(SIGALRM,oss_catch);
  alarm(1);
  if (verbose)
    Info("dsp: opening %s",arg_dsp);
  dsp = ::open(arg_dsp,O_WRONLY);
  if (dsp==-1)
    {
      alarm(0);
      return err_dsp;
    }
  alarm(0);
  
  // set dsp parameters
  p=AFMT_S16_LE;
  if (ioctl(dsp,SNDCTL_DSP_SETFMT,&p)==-1)
    Error(ui,"Setting Oss driver to standard 16 bit failed");
  p=2;
  if (ioctl(dsp,SNDCTL_DSP_CHANNELS,&p)==-1)
    Error(ui,"Setting Oss driver to 2 channels failed");
  p=WAVRATE;
  if (ioctl(dsp,SNDCTL_DSP_SPEED,&p)==-1)
    {
      Error(ui,"Setting Oss driver speed (%d) failed",p++);
      if (ioctl(dsp,SNDCTL_DSP_SPEED,&p)==-1)
	{
	  Error(ui,"Setting Oss dsp speed (%d) failed",p);
	  p+=2;
	  if (ioctl(dsp,SNDCTL_DSP_SPEED,&p)==-1)
	    Error(ui,"Setting Oss dsp speed (%d) failed\n",p);
	}
    }
  
  // set fragment size
  // opt_latency bevat de gevraagde latency. Dus dit omzetten naar
  // hiervan moeten we natuurlijk het logaritme nemen..
  // zie OSS manual
  {
    int latency_setter;
    int latency_checker;
    p = arg_fragments << 16;
    latency_setter  = ms2bytes(arg_latency);
    latency_setter /= arg_fragments;
    latency_checker = arg_fragments;
    if (verbose)
      Info("Setting Oss latency to %d ms",arg_latency);
    while(latency_setter>=1)
      {
	latency_setter/=2;
	latency_checker*=2;
	p++;
      }
    ioctl(dsp,SNDCTL_DSP_SETFRAGMENT,&p);
    ioctl(dsp,SNDCTL_DSP_GETOSPACE,&dsp_latency);
    latency_setter = bytes2ms(dsp_latency.bytes);
    if (verbose) 
      Info("actually latency will be %d ms",latency_setter);
    if (dsp_latency.bytes != latency_checker)
      Error(ui,"Impossible to set Oss to the requested latency");
    if (verbose)
      Info(" fragments = %d\n"
	   " fragsize = %d ms\n"
	   " bytes = %d ms",
	   dsp_latency.fragments, bytes2ms(dsp_latency.fragsize),
	   bytes2ms(dsp_latency.bytes));
    
    // now get the capacities
    ioctl(dsp,SNDCTL_DSP_GETCAPS,&p);
    if (verbose)
      Info(" realtime capability = %s\n"
	   " batch limitation = %s\n"
	   " mmap capability = %s",
	   (p & DSP_CAP_REALTIME) ? "yes" : "no",
	   (p & DSP_CAP_BATCH) ? "yes" : "no",
	   (p & DSP_CAP_MMAP) ? "yes" : "no");
  }
  
  // here we go
  clock_start();
  return err_none;
}

void dsp_oss::flush()
{
  ioctl(dsp,SNDCTL_DSP_SYNC);
}

void dsp_oss::close(bool fl)
{
  signed8   latencycheck;
  latencycheck=clock_ticks();
  if (fl) flush();
  if (verbose)
    Info("Fluffily-measured playing latency when closing = %d ms",
	 samples2ms(clock_ticks()-latencycheck));
  ::close(dsp);
}
#endif
#endif // __loaded__dsp_oss_cpp__
