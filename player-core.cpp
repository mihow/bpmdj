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
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <qdialog.h>
#include "player-core.h"
#include "memory.h"
#include "scripts.h"
#include "version.h"
#include "dsp-drivers.h"
#include "fourier.h"
#include "energy-analyzer.h"
#include "player-config.h"
#include "capacity.h"

using namespace std;

//#define DEBUG_WAIT_STATES

/*-------------------------------------------
 *         Constants & Variables
 *-------------------------------------------*/
#define  wave_bufsize (32L*1024L)

volatile int stop = 0;
volatile int finished = 0;
static volatile bool paused = true;

quad_period_type targetperiod;
quad_period_type currentperiod;
quad_period_type normalperiod;
signed8 y,x=0;
int     opt_quiet = 0;
int     opt_match = 0;
char*   arg_match = 0;
char*   argument;
Index*  playing = NULL;
dsp_driver *dsp = NULL;

extern PlayerConfig * config;

QString rawpath="";

QString get_rawpath()
{
  assert(config);
  if (rawpath.isEmpty())
    rawpath = config->get_core_rawpath();
  return rawpath;
}

FILE * openCoreRawFile()
{
  if (!playing) return NULL;
  return openRawFile(playing,get_rawpath());
}

/*-------------------------------------------
 *         File input oprations
 *-------------------------------------------*/
char * wave_name = NULL;
stereo_sample2 wave_buffer[wave_bufsize];
FILE * wave_file;
unsigned4 wave_bufferpos=wave_bufsize;

static int writer = -1;
static int writing = 0;
void writer_died(int sig, siginfo_t *info, void* hu)
{
  char newstr[80];
  // now we have the complete length of the file...
  // if it differs from the length in the .ini file we update it
  // get length;
  writing = 0;
  unsigned long long sec = samples2s(wave_max());
  unsigned long long min; 
  if (wave_file) 
    {
      min = sec/60;
      sec = sec%60;
      sprintf(newstr,"%02d:%02d",(int)min,(int)sec);
      playing->set_time(newstr);
    }
  msg_writing_finished();
}

int wave_open(Index * playing, bool synchronous)
{
  if (!playing) return err_none;
  const char * fname = playing->get_filename();
  int decoder = set_decoder_environment(config,playing);
  if (!decoder) return err_noraw;
  
  // start decoding the file; this means: fork bpmdj-raw process
  // wait 1 second and start reading the file
  if (synchronous)
    {
      writing = 1;
      if (!start_bpmdj_raw(get_rawpath(),fname))
	return err_nospawn;
      writing = 0;
    }
  else 
    {
      // prepare signals
      struct sigaction *act = bpmdj_allocate(1,struct sigaction);
      act->sa_sigaction = writer_died;
      act->sa_flags = SA_SIGINFO;
      sigaction(SIGUSR1,act,NULL);
      // fork and execute, send back signal when done 
      writing = 1;
      if (!(writer = fork()))
	{
	  start_bpmdj_raw(get_rawpath(),fname);
	  kill(getppid(),SIGUSR1);
	  /**
	   * If we use exit instead of _exit our own static data structures
	   * will be destroyed !
	   */
	  _exit(0);
	}
    }
  wave_name=getRawFilename(get_rawpath(),fname);

  wave_file=fopen(wave_name,"rb");
      
  if (synchronous)
    {
      writer_died(0,NULL,NULL);
      if (!wave_file)
	return err_noraw;
    }
  return err_none;
}

void wave_close()
{
  if (wave_file)
    fclose(wave_file);
  if (writing)
    {
      // send terminate (the insisting variant) signal
      kill(writer,SIGKILL);
      writing = 0;
    }
  remove(wave_name);
}

unsigned4 wave_max()
{
  if (!wave_file) 
    {
      wave_file=fopen(wave_name,"rb");
      if (!wave_file) return 0;
    }
  return fsize(wave_file)/4;
}

void wave_process(stereo_sample2 *wave_buffer, int size)
  // this contrast enhancing works quite well, except that the 'mean curve' 
  // is certainly not good. Short beats will sound stronger in the end result
  // than long sounds, which is not what is required
  // 
 {
  static double *tot_energy = NULL;
  static int count = 0;
  static double *amp;
  static double *co;
  static double *si;
  static double *dl;

  if (!tot_energy)
    {
      tot_energy = bpmdj_allocate(size,double);
      for(int i = 0 ; i < size ; i ++)
	tot_energy[i]=0;
      amp = bpmdj_allocate(size,double);
      co = bpmdj_allocate(size,double);
      si = bpmdj_allocate(size,double);
      dl = bpmdj_allocate(size,double);
    }
  for(int i = 0 ; i < size ; i ++)
    {
      amp[i] = wave_buffer[i].left;
      amp[i]/=32768.0;
    }
  fft_double(size,0,amp,NULL,co,si);
  count++;
  if (count % 100 == 0)
    {
      for(int i = 0 ; i < size ; i ++)
	printf("%g  ",tot_energy[i]);
      printf("\n");
    }
  for(int i = 0 ; i < size ; i ++)
    {
      double e = co[i]*co[i]+si[i]*si[i];
      double ea = sqrt(e);
      double an = atan2(si[i],co[i]);
      e = tot_energy[i]+=ea/count;
      
      ea/=e;
      if (ea<1)
	{
	  if (ea<0) ea = -sqrt(ea)/3;
	  else ea=sqrt(ea)/3;
	  //if (ea<0) ea = -ea*ea;
	  // else ea*=ea;
	  ea*=ea;
	}
      ea*=e;
      
      co[i]=ea*cos(an);
      si[i]=ea*sin(an);
    }
  fft_double(size,1,co,si,amp,dl);
  for(int i = 0 ; i < size ; i ++)
    wave_buffer[i].left = (short int)(amp[i]*32768.0);
}

void wave_process()
{
  const int size = 512;
  for(int i = 0 ; i < wave_bufsize ; i+=size)
    wave_process(wave_buffer+i,size);
}

int wave_read(unsigned4 pos, stereo_sample2 *val)
{
  if (pos<wave_bufferpos || pos-wave_bufferpos>=wave_bufsize)
    {
      // we only stop one second later than necessary
      if (pos>=wave_max()+44100L) return -1;
      if (pos>=wave_max())
	{
	  wave_bufferpos=pos;
	  for(int i = 0 ; i < wave_bufsize ; i ++)
	    wave_buffer[i].value(0);
	  val->value(0);
	  return 0;
	}
      wave_bufferpos=pos;
      fseek(wave_file,pos*4,SEEK_SET);
      int r = fread(wave_buffer,4,wave_bufsize,wave_file);
      if (r>-1 && r < wave_bufsize)
	for(int i = r ; i < wave_bufsize ; i ++)
	  wave_buffer[i].value(0);
      //wave_process();
    }
  *val=wave_buffer[pos-wave_bufferpos];
  return 0;
}


/*-------------------------------------------
 *         Volume Synth operations
 *-------------------------------------------*/
unsigned8        lfo_phase=0;
quad_period_type lfo_period=0;
_lfo_   lfo_do;

void jumpto(signed8, int);

_lfo_ lfo_get()
{
  return lfo_do;
}

void lfo_set(char* name, _lfo_ l, unsigned8 freq, unsigned8 phase)
{
  /* if paused, unpause and set phase */
  lfo_period=currentperiod.muldiv(4,freq);
  lfo_do=l;
  if (paused)
    {
      jumpto(0,0);
      lfo_phase=y;
      unpause_playing();
    }
  else
    {
      lfo_phase=phase;
    }
}

static signed4 volume = 100;
stereo_sample2 lfo_no(stereo_sample2 lt)
{
  return lt.muldiv(volume,100);
}

stereo_sample2 lfo_volume(stereo_sample2 x, signed4 multiplierleft, signed4 divisorleft, signed4 multiplierright,signed4 divisorright)
{
  return x.muldiv2(multiplierleft,divisorleft,multiplierright,divisorright);
}

stereo_sample2 lfo_metronome(stereo_sample2 x)
{
   double val;
   signed8 diff;
   diff=(signed8)y-(signed8)lfo_phase;
   diff=diff%lfo_period;
   /* say ping */
   val=sin(6.28*diff*(double)(WAVRATE)/440.0)*4096.0*(1.0-(double)diff/(double)lfo_period);
   /* mix them */
   stereo_sample2 a =  x.muldiv(7,8);
   return a.add((signed4)val,(signed4)val);
}

#define DIFF 100
#define DECAY 11050
stereo_sample2 lfo_pan(stereo_sample2 x)
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

stereo_sample2 lfo_saw(stereo_sample2 x)
{
   signed8 diff;
   diff=(signed8)y-(signed8)lfo_phase;
   diff=diff%lfo_period;
   return lfo_volume(x,diff,lfo_period,diff,lfo_period);
}

stereo_sample2 lfo_break(stereo_sample2 x)
{
  signed8 diff;
  diff=(signed8)y-(signed8)lfo_phase;
  diff=diff%lfo_period;
  if (diff>lfo_period*95/100) 
    return stereo_sample2();
  else 
    return x;
}

stereo_sample2 lfo_revsaw(stereo_sample2 x)
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
 *         Mapping Synth operations
 *-------------------------------------------*/
unsigned8   map_start_pos = 0;
unsigned8   map_stop_pos = 0;
bool        map_loop = false;
signed8     map_exit_pos = 0;
bool        map_do;
map_data    bpmdj_map = NULL;
signed2     map_size = 0;

void map_loop_set(bool l)
{
  map_loop=l;
}

unsigned8 map_active(unsigned8 a)
{
  // out of range ?
  if (a<map_start_pos) return a;
  if (a>=map_stop_pos) 
    {
      if (map_loop)
	{
	  a=map_start_pos;
	  x = map_start_pos;
	  y = y_normalise(x);
	}
      else
	{
	  volume = 100;
	  if (map_exit_pos == map_exit_stop)
	    {
	      x = map_start_pos;
	      pause_playing();
	    }
	  if (map_exit_pos == map_exit_restart)
	    x = map_start_pos;
	  else if (map_exit_pos == map_exit_continue)
	    x = map_stop_pos;
	  else 
	    x = map_exit_pos;
	  y = y_normalise(x);
	  map_set(0,NULL,0,map_exit_restart,false);
	  return x;
	}
    }

  // determine fine grained relative position of 'wants to play'
  unsigned8 dx = a - map_start_pos;
  // determine segment of 'wants to play'
  signed2 segment = dx*map_size/(map_stop_pos-map_start_pos);
#ifdef DEBUG_SEGSEQ
  static int last_segment = -1;
  if (segment!=last_segment)
    {
      last_segment=segment;
      printf("segment = %d\n",segment);
    }
  assert(segment<map_size);
#endif
  // determine offset wrt actual segment start
  unsigned8 segment_start = segment * (map_stop_pos-map_start_pos) / map_size;
  unsigned8 segment_offset = dx - segment_start;
  // obtain segment of new postion
  signed2 new_segment = bpmdj_map[segment].take_from;
  
  // calculate speed
  segment_offset = ((signed4)segment_offset) 
    * ((signed4)bpmdj_map[segment].speed_mult) 
    / ((signed4)bpmdj_map[segment].speed_div);
  if (new_segment < 0)
    return map_stop_pos;
  // transfer volume
  volume = bpmdj_map[segment].volume;
  // obtain new segment start position
  unsigned8 new_segment_start = new_segment * (map_stop_pos-map_start_pos) / map_size;
  // fix should play position with actual map_start_pos and fine grained segment offset
  return new_segment_start+segment_offset+map_start_pos;
}
  
void map_stop()
{
  if (map_do)
    {
      x = map_active(x);
      y = y_normalise(x);
      map_do=false;
      if (bpmdj_map) bpmdj_deallocate(bpmdj_map);
      bpmdj_map=NULL;
    }
}

void map_set(signed2 size, map_data inmap, unsigned8 msize, signed8 mexit, bool l)
{
  // if paused we start at the last cue position
  unsigned8 mstart;
  unsigned8 mstop;
  if (size > 0  && paused)
    mstart = cue;
  else
    mstart = x_normalise(::y - dsp->latency());
  mstop = mstart + msize;
  map_exit_pos = mexit;
  map_data old_map = bpmdj_map;
  // should we inactivate it ?
  if (size<=0) map_do=false;
  // should we modify first the size or the data ?
  // this is important because the map is still in use !
  if (size>map_size)
    {
      map_start_pos = mstart;
      map_stop_pos = mstop;
      bpmdj_map=inmap;
      map_size=size;
    }
  else
    {
      map_start_pos = mstart;
      map_stop_pos = mstop;
      map_size=size;
      bpmdj_map=inmap;
    }
  map_loop = l;
  // should we now activate it ?
  map_do = size > 0;
  // free old map ?
  if (old_map) bpmdj_deallocate(old_map);
  // unpause ?
  if (paused)
    {
      jumpto(0,0);
      unpause_playing();
    }
}

void map_init()
{
  map_do=false;
}

/*-------------------------------------------
 *         Cue points
 *-------------------------------------------*
 * these are expressed in sampleticks in the 
 * real (non-stretched) song
 */

signed8 x_normalise(signed8 y)
{
  return y*normalperiod/currentperiod;
}

signed8 y_normalise(signed8 x)
{
  return x*currentperiod/normalperiod;
}

static cue_info cue_before = 0;
cue_info cue = 0;
cue_info cues[4] = {0,0,0,0};

void cue_set()
{
  cue=x_normalise(y-dsp->latency());
}

void cue_shift(signed8 whence)
{
  if (whence < 0 && (unsigned8)(-whence) > cue) cue=0;
  else cue+=whence;
  if (!paused) 
    pause_playing();
}

void cue_store(int nr)
{
  cues[nr]=cue;
}

void cue_retrieve(int nr)
{
  cue=cues[nr];
}

void cue_write()
{
  if (playing)
    {
      playing->set_cue(cue);
      playing->set_cue_z(cues[0]);
      playing->set_cue_x(cues[1]);
      playing->set_cue_c(cues[2]);
      playing->set_cue_v(cues[3]);
    }
}

void cue_read()
{
  if (playing)
    {
      cue_before=cue=playing->get_cue();
      cues[0]=playing->get_cue_z();
      cues[1]=playing->get_cue_x();
      cues[2]=playing->get_cue_c();
      cues[3]=playing->get_cue_v();
    }
  else
    {
      cue_before = 0;
      cues[0]=cues[1]=cues[2]=cues[3]=0;
    }
}

/*-------------------------------------------
 *         RMS Normalization
 *-------------------------------------------*/
static float4 left_sub;
static float4 left_fact;
static float4 right_sub;
static float4 right_fact;
static bool   opt_rms = false;

void rms_init()
{
  assert(config);
  opt_rms = config->get_player_rms();
  float arg_rms = config->get_player_rms_target();
  
  sample4_type min = playing->get_min();
  sample4_type max = playing->get_max();
  sample4_type mean = playing->get_mean();
  power_type pow = playing->get_power();
  if (min.badly_defined() ||
      max.badly_defined() ||
      mean.badly_defined() || 
      pow.badly_defined())
    {
      Info("Switching of rms normalisation, this song has no known energy levels");
      opt_rms=0;
      return;
    }

  // obtain amplitude maximisation factor factor 
  left_sub = mean.left;
  left_fact = normalization_factor(min.left,max.left,mean.left);
  right_sub = mean.right;
  right_fact = normalization_factor(min.right,max.right,mean.right);
  
  // take into account the rms
  left_fact *= arg_rms;
  left_fact /= pow.left;
  if (arg_rms > pow.left) left_fact = 1.0;
  
  right_fact *= arg_rms;
  right_fact /= pow.right;
  if (arg_rms > pow.right) right_fact = 1.0;

}

void rms_normalize(stereo_sample2 *l)
{
  // left
  float v = l->left;
  v -= left_sub;
  v *= left_fact;
  l->left=(signed2)v;
  // right
  v = l->right;
  v -= right_sub;
  v *= right_fact;
  l->right=(signed2)v;
}

/*-------------------------------------------
 *         Loop operations
 *-------------------------------------------*/
const signed8 loop_off = 0x7FFFFFFFFFFFFFFFLL;
      signed8 loop_at = loop_off;
      signed8 loop_restart = 0;

int loop_set(unsigned8 jumpback)
{
  if (paused)
    {
      loop_at = cue;
      if (loop_at<0) 
	loop_at = loop_off;
      unpause_playing();
    }
  if (loop_at == loop_off)
    {
      loop_at=x;
    }
  if (jumpback==0)
    loop_at=loop_off;
  else
    if ((signed8)jumpback<=loop_at)
      loop_restart=loop_at-jumpback;
    else
      loop_at=loop_off;
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
void changetempo(signed8 period)
{
  /* change tempo to period
   * - the current x should remain the same
   * x = y * normalperiod / currentperiod
   * y = x * currentperiod / normalperiod
   */
  currentperiod = period;
  y = x * currentperiod / normalperiod; 
}

void jumpto(signed8 mes, int txt)
{
  if (paused)
    {
      unpause_playing();
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
      // - this fixed difference is added to gotopos
      signed8 gotopos=y_normalise(cue)-currentperiod*mes;
      // signed8 difference=gotopos-y+dsp->latency();
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

void read_write_bare_loop()
{
  stereo_sample2 value[1];
  unsigned8 m;
  lfo_init();
  map_init();
  dsp->start();
  while(!stop)
    {
      // wait for pause
      if (paused)
	dsp->pause();
      // calculate value
      x = y * normalperiod/currentperiod;
      if (x>loop_at)
	loop_jump();
      if (map_do)
	m = map_active( x );
      else
	m = x;
      if (wave_read(m,value)<0)
	{
	  // printf("End of song, pausing\n");
	  pause_playing();
	  value[0] = stereo_sample2();
	};
      value[0]=lfo_do(value[0]);
      y=y+1;
      
      // write value
      dsp->write(value[0]);
    }
}

void read_write_normalize_loop()
{
  stereo_sample2 value[1];
  unsigned8 m;
  if (!opt_rms)
    {
      read_write_bare_loop();
      return;
    }
  lfo_init();
  map_init();
  dsp->start();
  while(!stop)
    {
      // wait for pause
      if (paused)
	dsp->pause();
      // calculate value
      x = y * normalperiod/currentperiod;
      if (x>loop_at)
	loop_jump();
      if (map_do)
	m = map_active( x );
      else
	m = x;
      if (wave_read(m,value)<0)
	{
	  // printf("End of song, pausing\n");
	  pause_playing();
	  value[0] = stereo_sample2();
	};
      rms_normalize(value);
      value[0]=lfo_do(value[0]);
      y=y+1;
      
      // write value
      dsp->write(value[0]);
    }
}

void read_write_loop()
{
  if (opt_rms)
    read_write_normalize_loop();
  else
    read_write_bare_loop();
}  

void wait_for_unpause()
{
#ifdef DEBUG_WAIT_STATES
  Debug("wait_for_unpause(): entered\n");
  fflush(stdout);
#endif
  while(paused);
#ifdef DEBUG_WAIT_STATES
  Debug("wait_for_unpause(): finished\n");
  fflush(stdout);
#endif
}

bool get_paused()
{
  return ::paused;
}

void pause_playing()
{
  if (!paused)
    {
      paused = true;
      msg_playing_state_changed();
    }
}

void unpause_playing()
{
  if (paused)
    {
      paused=false;
      msg_playing_state_changed();
    }
}

/*-------------------------------------------
 *         Startup code
 *-------------------------------------------*/ 
void copyright()
{
  printf("BpmDj Player v%s, Copyright (c) 2001-2006 Werner Van Belle\n",VERSION);
  printf("This software is distributed under the GPL2 license. See copyright.txt\n");
  printf("--------------------------------------------------------------------\n");
  fflush(stdout);
  fflush(stderr);
}

int core_meta_init()
{
  copyright();
  common_init();
  // Parsing the arguments
  if (opt_match)
    {
      Index target(arg_match);
      targetperiod=period_to_quad(target.get_period());
    }

  int L = strlen(argument);
  
  if (L<4 || strcmp(argument+L-4,".idx")!=0)
    //if ( strstr(argument,".idx")==NULL || strcmp(strstr(argument,".idx"),".idx")!=0)
    return err_needidx;
  playing = new Index(argument);
  
  normalperiod=period_to_quad(playing->get_period());
  if (!opt_match) targetperiod=normalperiod;
  if (normalperiod<=0 && targetperiod>0) normalperiod=targetperiod;
  else if (normalperiod>0 && targetperiod<=0) targetperiod=normalperiod;
  currentperiod=targetperiod;
  
  if (WAVRATE==22050)
    {
      currentperiod=currentperiod.halved();
      normalperiod=normalperiod.halved();
      targetperiod=targetperiod.halved();
    }

  cue_read();
  return err_none;
}


int core_object_init(bool sync)
{
  return wave_open(playing, sync);
}

int core_open()
{
  return dsp->open();
}

void core_play()
{
  if (playing)
    {
      stop = 0;
      finished = 0;
      unpause_playing();
      rms_init();
      read_write_loop();
    }
  else
    {
      stop = true;
      finished = true;
      pause_playing();
    }
}

void core_close()
{
  dsp->close();
  finished = 1;
}

void core_done()
{
  wave_close();
  if (playing)
    {
      cue_write();
      if (playing->changed())
	{
	  if (!opt_quiet) Info("Updating index file");
	  playing->write_idx();
	}
    }
#ifdef DEBUG_WAIT_STATES
  Debug("finished marked true");
  fflush(stdout);
#endif
  finished = 2;
}

/*static void * go(void* neglect)
{
  core_play();
  core_close();
  core_done();
  return neglect;
}
*/

static void * go2(void* neglect)
{
  core_play();
  core_close();
  return neglect;
}

/*int core_run()
{
  int err = core_object_init(false);
  if (err==err_noraw || err==err_nospawn)
    return err;
  core_start();
  err = core_open();
  if (err==err_dsp) 
    {
      core_done();
      return err;
    }
  pthread_t *y = bpmdj_allocate(1,pthread_t);
  pthread_create(y,NULL,go,NULL);
  return err_none;
}
*/

int core_start()
{
  int err = core_open();
  if (err==err_dsp) 
    {
      core_close();
      return err;
    }
  pthread_t *y = bpmdj_allocate(1,pthread_t);
  pthread_create(y,NULL,go2,NULL);
  return err_none;
}

void core_stop()
{
#ifdef DEBUG_WAIT_STATES
  Debug("core_stop(): entered\n");
#endif
  ::stop=1;
  unpause_playing();
  while(!finished) ;
#ifdef DEBUG_WAIT_STATES
  Debug("core_stop(): finished\n");
#endif
}
