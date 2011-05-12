/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001 Werner Van Belle

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
#include <libgen.h>
#include <linux/soundcard.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include "player-core.h"
#include "version.h"
#include "scripts.h"

int     opt_ossfragments = 0;
char*   arg_dsp = "/dev/dsp";
char*   arg_ossfragments = "16";
int     opt_oss_nolatencyaccounting = 0;

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
static int            dsp;
static signed8        dsp_writecount=0;
static audio_buf_info latency;

void oss_start()
{
  dsp_writecount=0;
}

signed8 oss_playcount()
{
  count_info cnt;
  //  audio_errinfo err;
  if (ioctl(dsp,SNDCTL_DSP_GETOPTR,&cnt)==-1)
    return dsp_writecount+(latency.bytes/2);
  // if (ioctl(dsp,SNDCTL_DSP_GETERRORS,&err)!=-1)
  // if (err.play_underruns>0)
  // cnt.bytes-=err.play_underruns;
  if (cnt.bytes<0)
    cnt.bytes=0;
  return cnt.bytes/4;
}

void oss_pause()
{
  //  dsp_flush();
  ioctl(dsp,SNDCTL_DSP_RESET);
  while(paused);
  clock_start();
  dsp_writecount=0;
}

void oss_write(unsigned4 *value)
{
  dsp_writecount++;
  write(dsp,value,4);
}

signed8 oss_latency()
{
  if (opt_oss_nolatencyaccounting)
    return dsp_writecount - (latency.bytes/4);
  else
    return dsp_writecount - oss_playcount();
}

void oss_catch(int ignore)
{
  printf("Failed to open (alarm timeout) %s",arg_dsp);
  exit(0);
}

int oss_open()
{
  int p;
  // start alarm before opening...
  // open int, and if it doesn't answer in time kill it
  signal(SIGALRM,oss_catch);
  alarm(1);
  dsp=open(arg_dsp,O_WRONLY);
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
    p = atoi(arg_ossfragments) << 16;
    latency_setter  = ms2bytes(atoi(arg_latency));
    latency_setter /= atoi(arg_ossfragments);
    latency_checker = atoi(arg_ossfragments);
    printf("dsp: setting latency to %s ms\n",arg_latency);
    while(latency_setter>=1)
      {
	latency_setter/=2;
	latency_checker*=2;
	p++;
	// printf("  setter = %d , checker = %d, p == %x\n",latency_setter, latency_checker, p);
      }
    ioctl(dsp,SNDCTL_DSP_SETFRAGMENT,&p);
    ioctl(dsp,SNDCTL_DSP_GETOSPACE,&latency);
    latency_setter = bytes2ms(latency.bytes);
    printf("     actually latency will be %d ms\n",latency_setter);
    if (latency.bytes != latency_checker)
      printf("dsp: impossible to set the required latency\n");
    if (opt_dspverbose)
      {
	printf("     fragments = %d\n", latency.fragments);
	printf("     fragsize = %d ms\n", bytes2ms(latency.fragsize));
	printf("     bytes = %d ms\n", bytes2ms(latency.bytes));
      }
    
    // now get the capacities
    ioctl(dsp,SNDCTL_DSP_GETCAPS,&p);
    if (opt_dspverbose)
      printf("dsp: realtime capability = %s\n"
	     "     batch limitation = %s\n"
	     "     mmap capability = %s\n",
	     (p & DSP_CAP_REALTIME) ? "yes" : "no",
	     (p & DSP_CAP_BATCH) ? "yes" : "no",
	     (p & DSP_CAP_MMAP) ? "yes" : "no");
  }
  
  // here we go
  oss_start();
  clock_start();
  return err_none;
}

void oss_flush()
{
  ioctl(dsp,SNDCTL_DSP_SYNC);
}

void oss_close()
{
  signed8   latencycheck;
  latencycheck=clock_ticks();
  oss_flush();
  if (opt_dspverbose)
    printf("dsp: fluffy-measured playing latency = %d ms\n",samples2ms(clock_ticks()-latencycheck));

  close(dsp);
}

/*-------------------------------------------
 *         Mixer operations
 *-------------------------------------------*/
/*static int mixer=-1;
int mixer_open()
{
  if (mixer>-1) return err_none;
  mixer=open(arg_mixer,O_RDWR);
  if (mixer==-1)
    return err_mixer;
  return err_none;
}

void mixer_close()
{
   if (mixer==-1)
     return;
   close(mixer);
   mixer=-1;
}

int mixer_get_main()
{
   int volume;
   ioctl(mixer,SOUND_MIXER_READ_VOLUME,&volume);
   return volume&0xff;
}

int mixer_get_pcm()
 {
   int volume=0;
   ioctl(mixer,SOUND_MIXER_READ_PCM,&volume);
   return volume&0xff;
}

void mixer_set_main(int v)
{
   v=v|(v<<8);
   ioctl(mixer,SOUND_MIXER_WRITE_VOLUME,&v);
}

void mixer_set_pcm(int v)
{
   v=v|(v<<8);
   ioctl(mixer,SOUND_MIXER_WRITE_PCM,&v);
}
*/

#endif
