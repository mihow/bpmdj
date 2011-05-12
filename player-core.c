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
#include "cbpm-index.h"
#include "version.h"
#include "scripts.h"

// WVB -- testing variable for the impulse characeteristic
//#define IMPULSE_PANNING

/*-------------------------------------------
 *         Constants & Variables
 *-------------------------------------------*/
#define  wave_bufsize (32L*1024L)
volatile int stop = 0;
volatile int finished = 0;
volatile int paused = 0;
signed8 targetperiod;
signed8 currentperiod;
signed8 normalperiod;
signed8 y,x=0;
int     opt_quiet = 0;
char*   arg_dsp = "/dev/dsp";
int     opt_match = 0;
char*   arg_match = 0;
int     opt_latency = 0;
int     opt_fragments = 0;
char*   arg_fragments = "16";
int     opt_dspverbose = 0;
int     opt_nolatencyaccounting = 0;
char*   arg_latency = "300";
char*   arg_mixer = "/dev/mixer";
char*   argument;

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

void dsp_start()
{
  dsp_writecount=0;
}

signed8 dsp_playcount()
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

void dsp_pause()
{
  ioctl(dsp,SNDCTL_DSP_RESET);
  while(paused);
  dsp_writecount=0;
}

void dsp_write(unsigned4 *value)
{
  dsp_writecount++;
  write(dsp,value,4);
}

signed8 dsp_latency()
{
  if (opt_nolatencyaccounting)
    return dsp_writecount - (latency.bytes/4);
  else
    return dsp_writecount - dsp_playcount();
}

void dsp_catch(int ignore)
{
  printf("Failed to open (alarm timeout) %s",arg_dsp);
  exit(0);
}

int dsp_open()
{
  int p;
  // start alarm before opening...
  // open int, and if it doesn't answer in time kill it
  signal(SIGALRM,dsp_catch);
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
  
  // set fragement size
  // opt_latency bevat de gevraagde latency. Dus dit omzetten naar
  // hiervan moeten we natuurlijk het logaritme nemen..
  // zie OSS manual

  {
    int latency_setter;
    int latency_checker;
    p = atoi(arg_fragments) << 16;
    latency_setter  = ms2bytes(atoi(arg_latency));
    latency_setter /= atoi(arg_fragments);
    latency_checker = atoi(arg_fragments);
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
  
  // here we goo..
  dsp_start();
  clock_start();
  return err_none;
}

void dsp_flush()
{
  ioctl(dsp,SNDCTL_DSP_SYNC);
}

void dsp_close()
{
  close(dsp);
}

/*-------------------------------------------
 *         Mixer operations
 *-------------------------------------------*/
static int mixer=-1;
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

/*-------------------------------------------
 *         File input oprations
 *-------------------------------------------*/
char * wave_name=NULL;
unsigned4 wave_buffer[wave_bufsize];
FILE * wave_file;
unsigned4 wave_bufferpos=wave_bufsize;

static int writer = -1;
static int writing = 0;
void writer_died(int sig,siginfo_t *info, void* hu)
{
  char newstr[80];
  // now we have the complete length of the file...
  // if it differs from the length in the .ini file we update it
  // get length;
  unsigned long long sec;
  unsigned long long min; 
  writing = 0;
  if (!wave_file) return;
  sec = samples2s(wave_max());
  min = sec/60;
  sec = sec%60;
  sprintf(newstr,"%02d:%02d",(int)min,(int)sec);
  if (!index_time || (strcmp(newstr,index_time)!=0))
    {
      index_time=strdup(newstr);
      index_changed=1;
    }
}

int wave_open(char* fname, int synchronous)
{
  char fn[500];
  // start decoding the file; this means: fork bpmdj-raw process
  // wait 1 second and start reading the file
  if (synchronous)
    {
      writing = 1;
      if (!vexecute(CREATERAW_CMD,fname))
	return err_nospawn;
      writing=0;
    }
  else 
    {
      // prepare signals
      struct sigaction *act = allocate(1,struct sigaction);
      act->sa_sigaction = writer_died;
      act->sa_flags = SA_SIGINFO;
      sigaction(SIGUSR1,act,NULL);
      // fork and execute, send back signal when done 
      writing = 1;
      if (!(writer = fork()))
	{
	  vexecute(CREATERAW_CMD,fname);
	  kill(getppid(),SIGUSR1);
	  exit(0);
	}
    }
  sprintf(fn,"%s.raw",fname);
  wave_name=strdup(basename(fn));
  int tries = 5;
  do
    {
      wave_file=fopen(wave_name,"rb");
      sleep(1);
    }
  while (wave_file==NULL && tries-->0);
  if (!wave_file)
    return err_noraw;
  return err_none;
}

void wave_close()
{
  fclose(wave_file);
  if (writing)
    {
      // send terminate (well the insisting variant) signal
      kill(writer,SIGKILL);
      writing = 0;
    }
  remove(wave_name);
}

unsigned4 wave_max()
{
  return fsize(wave_file)/4;
}

int wave_read(unsigned4 pos, unsigned4 *val)
{
   if (pos-wave_bufferpos>=wave_bufsize)
     {
	if (pos>wave_max()) return -1;
	wave_bufferpos=pos;
	fseek(wave_file,pos*4,SEEK_SET);
	fread(wave_buffer,4,wave_bufsize,wave_file);
     }
   *val=wave_buffer[pos-wave_bufferpos];
   return 0;
}


/*-------------------------------------------
 *         Synth operations
 *-------------------------------------------*/
unsigned8   lfo_phase=0;
signed8   lfo_period=0;
_lfo_   lfo_do;

void jumpto(signed8, int);

void lfo_set(char* name, _lfo_ l, unsigned8 freq, unsigned8 phase)
{
  /* if paused, unpause and set phase */
  lfo_period=4*currentperiod/freq;
  lfo_do=l;
  if (paused)
    {
      jumpto(0,0);
      lfo_phase=y;
      paused=0;
      printf("Restarted with %s lfo at 4M/%d\n",name,(int)freq);
    }
  else
    {
      lfo_phase=phase;
      printf("Switched to %s lfo at 4M/%d\n",name,(int)freq);
    }
}

unsigned4 lfo_no(unsigned4 x)
{
  return x;
}

unsigned4 lfo_volume(unsigned4 x, signed4 multiplierleft, signed4 divisorleft, signed4 multiplierright,signed4 divisorright)
{
   longtrick lt;
   signed4 left;
   signed4 right;
   lt.value=x;
   left=lt.leftright.left;
   right=lt.leftright.right;
   left*=multiplierleft;
   right*=multiplierright;
   left/=divisorleft;
   right/=divisorright;
   lt.leftright.left=left;
   lt.leftright.right=right;
   return lt.value;
}

unsigned4 lfo_metronome(unsigned4 x)
{
   double val;
   signed8 diff;
   longtrick lt;
   diff=(signed8)y-(signed8)lfo_phase;
   diff=diff%lfo_period;
   /* say ping */
   val=sin(6.28*diff*(double)(WAVRATE)/440.0)*4096.0*(1.0-(double)diff/(double)lfo_period);
   /* mix them */
   lt.value=x;
   lt.leftright.left+=(signed short)val;
   lt.leftright.right+=(signed short)val;
   return lt.value;
}

#define DIFF 100
#define DECAY 11050
unsigned4 lfo_difference(unsigned4 x)
{
   static signed short freq=2;
   static signed short oldestat=0;
   static signed8 suml=0;
   static signed8 sumr=0;
   static signed short prevl[DIFF];
   static signed short prevr[DIFF];
   static signed8 lostrengths[DECAY];
   static signed8 histrengths[DECAY];
   static signed8 lostrength=1L;
   static signed8 histrength=1L;
   static signed8 leftlo, lefthi, rightlo, righthi;
   static signed8 oldeststrengthat=0;
   longtrick lt;
   signed4 left;
   signed4 right;
   signed4 oldleft;
   signed4 oldright;
   lt.value=x;
   /* get the oldest value */
   oldleft=prevl[oldestat];
   oldright=prevr[oldestat];
   /* subtract them from the sum */
   suml-=oldleft;
   sumr-=oldright;
   /* store this value */
   prevl[oldestat]=lt.leftright.left;
   prevr[oldestat]=lt.leftright.right;
   oldestat++;
   if (oldestat>=freq) oldestat=0;
   /* add it to the sum */
   suml+=lt.leftright.left;
   sumr+=lt.leftright.right;
   /* calculate the new ratio */
   lostrength-=lostrengths[oldeststrengthat];
   histrength-=histrengths[oldeststrengthat];
   /* calculate the difference */
   lefthi=lt.leftright.left;
   leftlo=suml/freq;
   righthi=lt.leftright.right;
   rightlo=sumr/freq;
   left=lefthi-leftlo;
   right=righthi-rightlo;
   /* this will be played */
   lt.leftright.left=left;
   lt.leftright.right=right;
   /* calculate the new strength */
   leftlo=abs(leftlo);
   lefthi=abs(lefthi);
   lostrength+=leftlo;
   histrength+=lefthi;
   lostrengths[oldeststrengthat]=leftlo;
   histrengths[oldeststrengthat]=lefthi;
   oldeststrengthat++;
   if (oldeststrengthat>=DECAY) 
     {
	oldeststrengthat=0;
	printf("%g %g\n",(log(lostrength)/log(1.002))-8000.0,(log(histrength)/log(1.002))-8000.0);
     }
   return lt.value;
}


#ifdef IMPULSE_PANNING
#undef DIFF
static int DIFF = 19190*4;
static float *prevl;
static float *prevr;
void pan_init()
{
  int i;
  DIFF = normalperiod;
  prevl = allocate(DIFF,float);
  prevr = allocate(DIFF,float);
  
  for(i = 0 ; i < DIFF ; i ++)
    {
      prevl[i]=0.0;
      prevr[i]=0.0;
    }
}

static float maxsig=0.0;
unsigned4 lfo_pan(unsigned4 x)
{
   signed8 diff;
   longtrick lt;
   signed4 left;
   signed4 right;
   lt.value=x;
   int pos = y %DIFF;
   prevl[pos]+=(float)lt.leftright.left/100.0;
   prevr[pos]+=(float)lt.leftright.right/100.0;
   if (fabs(prevl[pos])>maxsig)
     maxsig=fabs(prevl[pos]);
   if (fabs(prevr[pos])>maxsig)
     maxsig=fabs(prevr[pos]);
   lt.leftright.left = prevl[pos]*32767.0/maxsig;
   lt.leftright.right = prevr[pos]*32767.0/maxsig;
   return lt.value;
}
#endif

#ifndef IMPULSE_PANNING
unsigned4 lfo_pan(unsigned4 x)
{
  signed8 diff;
  signed4 quart;
  diff=(signed8)y-(signed8)lfo_phase;
  diff=diff%lfo_period;
  quart=lfo_period/4;
  // at position 0, centre 
  if (diff<quart)
    return lfo_volume(x,quart-diff,quart,1,1);
  // at position 0.25, right 
  diff-=quart;
  if (diff<quart)
    return lfo_volume(x,diff,quart,1,1);
  // at position 0.5 centre 
  diff-=quart;
  if (diff<quart)
    return lfo_volume(x,1,1,quart-diff,quart);
  // at position 0.75 left
  diff-=quart;
  return lfo_volume(x,1,1,diff,quart);
}
#endif

unsigned4 lfo_saw(unsigned4 x)
{
   signed8 diff;
   diff=(signed8)y-(signed8)lfo_phase;
   diff=diff%lfo_period;
   return lfo_volume(x,diff,lfo_period,diff,lfo_period);
}

unsigned4 lfo_break(unsigned4 x)
{
   signed8 diff;
   diff=(signed8)y-(signed8)lfo_phase;
   diff=diff%lfo_period;
   if (diff>lfo_period*95/100) 
     return 0;
   else 
     return x;
}

unsigned4 lfo_revsaw(unsigned4 x)
{
  signed8 diff;
  diff=(signed8)y-(signed8)lfo_phase;
  while (diff<0 && lfo_period) 
    diff+=lfo_period;
  diff=diff%lfo_period;
  diff=lfo_period-diff;
  return lfo_volume(x,diff,lfo_period,diff,lfo_period);
}

void lfo_init()
{
  lfo_do=lfo_no;
}

/*-------------------------------------------
 *         Cue points
 *-------------------------------------------*
 * these are expressed in sampleticks in the 
 * real (non-stretched) song
 */

unsigned8 x_normalise(unsigned8 y)
{
  return y*normalperiod/currentperiod;
}

unsigned8 y_normalise(unsigned8 x)
{
  return x*currentperiod/normalperiod;
}

static cue_info cue_before = 0;
cue_info cue = 0;
cue_info cues[4] = {0,0,0,0};

void cue_set()
{
   cue=x_normalise(y-dsp_latency());
   printf("Setting cue\n");
}

void cue_shift(char* txt, signed8 whence)
{
   printf("%s\n",txt);
   cue+=whence;
   if (!paused) 
     paused=1;
}

void cue_store(char* text,int nr)
{
   printf("Storing %s cue\n",text);
   cues[nr]=cue;
}

void cue_retrieve(char* text,int nr)
{
   printf("Retrieving and jumping to %s cue\n",text);
   cue=cues[nr];
}

void cue_write()
{
   if (cue!=cue_before)
     {
	index_cue=cue;
	index_changed=1;
	printf("Index has been changed\n");
     }
   if (index_cue_z!=(unsigned4)cues[0])
     {
	printf("Z cue has been changed\n");
	index_changed=1;
	index_cue_z=cues[0];
     }
   if (index_cue_x!=(unsigned4)cues[1])
     {
	printf("X cue has been changed\n");
	index_changed=1;
	index_cue_x=cues[1];
     }
   if (index_cue_c!=(unsigned4)cues[2])
     {
	printf("C cue has been changed\n");
	index_changed=1;
	index_cue_c=cues[2];
     }
   if (index_cue_v!=(unsigned4)cues[3])
     {
	printf("V cue has been changed\n");
	index_changed=1;
	index_cue_v=cues[3];
     }
}

void cue_read()
{
   cue_before=cue=index_cue;
   cues[0]=index_cue_z;
   cues[1]=index_cue_x;
   cues[2]=index_cue_c;
   cues[3]=index_cue_v;
   if (!opt_quiet)
     {
	printf("Available cue's: ");
	if (cues[0]) printf("Z ");
	if (cues[1]) printf("X ");
	if (cues[2]) printf("C ");
	if (cues[3]) printf("V ");
	printf("\n");
     }
   
}

/*-------------------------------------------
 *         Loop operations
 *-------------------------------------------*/
#define   loop_off  ((unsigned8)-1)
unsigned8 loop_at = loop_off;
unsigned8 loop_restart = 0;

int loop_set(unsigned8 jumpback)
{
  //  printf("---\nloop_at = %d, loop_restart = %d, cue = %d, jumpback=%d, paused = %d\n",(int)loop_at,(int)loop_restart,(int)cue,(int)jumpback,(int)paused);
  if (paused)
    {
      loop_at = cue;
      if (loop_at<0) 
	loop_at = loop_off;
      paused = 0;
    }
  if (loop_at==loop_off)
    {
      loop_at=x;
    }
  if (jumpback==0)
    loop_at=loop_off;
  else
    if (jumpback<=loop_at)
      loop_restart=loop_at-jumpback;
    else
      loop_at=loop_off;
  //  printf("loop_at = %d, loop_restart = %d, cue = %d, jumpback=%d, paused = %d\n",(int)loop_at,(int)loop_restart,(int)cue,(int)jumpback,(int)paused);
  return loop_at!=loop_off;
}

void loop_jump()
{
  x=loop_restart;
  y=y_normalise(x);
}

/*-------------------------------------------
 *         Program logic
 *-------------------------------------------*/
void help();

void doubleperiod()
{
   index_period*=2; index_changed=1;
}

void halveperiod()
{
   index_period/=2; index_changed=1;
}

void changetempo(signed8 period)
{
   /* change tempo to period
    * - the current x should remain the same
    * x = y * normalperiod / currentperiod
    * y = x * currentperiod / normalperiod
    */
   int change=(long)(currentperiod*1000/period)-(long)1000;
   if (period>currentperiod)
     msg_slowdown(change);
   else
     msg_speedup(change);
   currentperiod = period;
   y = x * currentperiod / normalperiod;
   
}

void jumpto(signed8 mes, int txt)
{
  if (paused)
    {
      paused=0;
      y=y_normalise(cue);
      if (txt) printf("Restarted at cue ");
    }
  else
    {
      // - first we have the position which _should_ be playing 'NOW'
      // - second, we have the data which is now queued
      // - third we have the latency
      // - if we subtract the latency from the data being queued, we have the real playing position
      // - if we subtract what should be playing with what is playing, we have a difference
      // - we can fix this difference to fit a measure
      // - this fixed difference is added to gotopos.
      signed8 gotopos=y_normalise(cue)-currentperiod*mes;
      // signed8 difference=gotopos-y+dsp_latency();
      signed8 difference=gotopos-y;
      signed8 fixeddiff=(difference/currentperiod)*currentperiod;
      y+=fixeddiff;
      if (txt) printf("Started at cue ");
    }
  if (txt)
    {
      if (mes)
	printf("-%d measures\n",(int)mes);
      else
	printf("\n");
    }
}

void read_write_loop()
{
  signed8   latencycheck;
  unsigned4 value[1];
  lfo_init();
  dsp_start();
  clock_start();
  while(!stop)
    {
      // wait for pause
      if (paused) 
	{
	  dsp_pause();
	  clock_start();
	}
      // calculate value
      x=y*normalperiod/currentperiod;
      if (x>loop_at)
	loop_jump();
      if (wave_read(x,value)<0)
	{
	  printf("End of song, pausing\n");
	  paused = 1;
	  value[0] = 0L;
	};
      value[0]=lfo_do(value[0]);
      y=y+1;
      
      // write value
      dsp_write(value);
    }
  
  latencycheck=clock_ticks();
  dsp_flush();
  if (opt_dspverbose)
    printf("dsp: fluffy-measured playing latency = %d ms\n",samples2ms(clock_ticks()-latencycheck));
}

/*-------------------------------------------
 *         Startup code
 *-------------------------------------------*/ 
void line()
{
  printf("--------------------------------------------------------------------\n");
}

void copyright()
{
   printf("BpmDj Player v%s, Copyright (c) 2001 Werner Van Belle\n",VERSION);
   printf("This software is distributed under the GPL2 license. See copyright.txt\n");
   printf("Press 'h' for Help. See beatmixing.ps for details\n");
   line();
}

int core_init(int sync)
{
  int err;
  copyright();
  common_init();
  // Parsing the arguments
  if (opt_match)
    {
      index_read(arg_match);
      targetperiod=index_period*4;
      index_free();
    }
  if (strstr(argument,".idx")==NULL || strcmp(strstr(argument,".idx"),".idx")!=0)
    return err_needidx;
  index_read(argument);
  normalperiod=index_period*4;
  if (!opt_match) targetperiod=normalperiod;
  currentperiod=targetperiod;
  if (WAVRATE==22050)
    {
      currentperiod/=2;
      normalperiod/=2;
      targetperiod/=2;
    }
  if (!opt_quiet)
    {
      double normaltempo = mperiod2bpm(normalperiod);
      double targettempo = mperiod2bpm(targetperiod);
      if (normaltempo>0)
	printf("Normal tempo = %g BPM\n",normaltempo);
      else
	printf("No normal tempo known\n");
      if (targettempo>0)
	printf("Target tempo = %g BPM",targettempo);
      else
	printf("No Target tempo known\n");
      printf(" speed(%g)\n",(double)normalperiod/(double)currentperiod);
    }
  cue_read();
#ifdef IMPULSE_PANNING
  pan_init();
#endif
  err = wave_open(index_file,sync);
  return err;
}

int core_open()
{
  int err;
  err = mixer_open();
  if (err) return err;
  err = dsp_open();
  return err;
}

void core_play()
{
   read_write_loop();
}

void core_close()
{
   dsp_close();
   mixer_close();
}

void core_done()
{
   wave_close();
   cue_write();
   if (index_changed)
     {
       if (!opt_quiet)
	 printf("Updating index file\n");
       index_write();
     }
   index_free();
   finished = 1;
}
