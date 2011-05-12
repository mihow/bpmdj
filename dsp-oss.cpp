/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

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
  arg_dsp = strdup(config.get_oss_dsp());
  arg_fragments = config.get_oss_fragments();
  opt_nolatencyaccounting = config.get_oss_nolatencyaccounting();
  dsp_writecount=0;
  arg_latency = config.get_oss_latency();
  verbose = config.get_oss_verbose();
}

void dsp_oss::start()
{
  dsp_writecount=0;
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

void dsp_oss::pause()
{
  //  dsp_flush();
  ioctl(dsp,SNDCTL_DSP_RESET);
  wait_for_unpause();
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
  printf("Failed to open (alarm timeout) oss dsp device");
  exit(0);
}

int dsp_oss::open()
{
  int p;
  // start alarm before opening...
  // open int, and if it doesn't answer in time kill it
  signal(SIGALRM,oss_catch);
  alarm(1);
  if (verbose)
    {
      printf("dsp: opening %s\n",arg_dsp);
    }
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
    printf("dsp: setting dsp to standard 16 bit failed\n");
  p=2;
  if (ioctl(dsp,SNDCTL_DSP_CHANNELS,&p)==-1)
    printf("dsp: setting dsp to 2 channels failed\n");
  p=WAVRATE;
  if (ioctl(dsp,SNDCTL_DSP_SPEED,&p)==-1)
    {
      printf("dsp: setting dsp speed (%d) failed\n",p++);
      if (ioctl(dsp,SNDCTL_DSP_SPEED,&p)==-1)
	{
	  printf("dsp: setting dsp speed (%d) failed\n",p);
	  p+=2;
	  if (ioctl(dsp,SNDCTL_DSP_SPEED,&p)==-1)
	    printf("dsp: setting dsp speed (%d) failed\n",p);
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
      printf("dsp: setting latency to %d ms\n",arg_latency);
    while(latency_setter>=1)
      {
	latency_setter/=2;
	latency_checker*=2;
	p++;
	// printf("  setter = %d , checker = %d, p == %x\n",latency_setter, latency_checker, p);
      }
    ioctl(dsp,SNDCTL_DSP_SETFRAGMENT,&p);
    ioctl(dsp,SNDCTL_DSP_GETOSPACE,&dsp_latency);
    latency_setter = bytes2ms(dsp_latency.bytes);
    if (verbose) 
      printf("     actually latency will be %d ms\n",latency_setter);
    if (dsp_latency.bytes != latency_checker)
      printf("dsp: impossible to set the required latency\n");
    if (verbose)
      {
	printf("     fragments = %d\n", dsp_latency.fragments);
	printf("     fragsize = %d ms\n", bytes2ms(dsp_latency.fragsize));
	printf("     bytes = %d ms\n", bytes2ms(dsp_latency.bytes));
      }
    
    // now get the capacities
    ioctl(dsp,SNDCTL_DSP_GETCAPS,&p);
    if (verbose)
      printf("dsp: realtime capability = %s\n"
	     "     batch limitation = %s\n"
	     "     mmap capability = %s\n",
	     (p & DSP_CAP_REALTIME) ? "yes" : "no",
	     (p & DSP_CAP_BATCH) ? "yes" : "no",
	     (p & DSP_CAP_MMAP) ? "yes" : "no");
  }
  
  // here we go
  start();
  clock_start();
  return err_none;
}

void dsp_oss::flush()
{
  ioctl(dsp,SNDCTL_DSP_SYNC);
}

void dsp_oss::close()
{
  signed8   latencycheck;
  latencycheck=clock_ticks();
  flush();
  if (verbose)
    printf("dsp: fluffy-measured playing latency = %d ms\n",samples2ms(clock_ticks()-latencycheck));
  
  ::close(dsp);
}

#endif
