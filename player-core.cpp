/****
 BpmDj v4.2: Free Dj Tools
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
#ifndef __loaded__player_core_cpp__
#define __loaded__player_core_cpp__
using namespace std;
#line 1 "player-core.c++"
#include <cstdio>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <libgen.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <qdialog.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "player-core.h"
#include "memory.h"
#include "scripts.h"
#include "version.h"
#include "dsp-drivers.h"
#include "energy-analyzer.h"
#include "player-config.h"
#include "capacity.h"
#include "clock-drivers.h"
#include "info.h"

/*-------------------------------------------
 *         Constants & Variables
 *-------------------------------------------*/
#define  wave_bufsize (32L*1024L)

volatile int finished = 1;

int     opt_quiet = 0;
int     opt_match = 0;
char*   arg_match = 0;
char*   argument;
Index*  playing = NULL;

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
  return openRawFile(playing,
		     get_rawpath().toAscii().data());
}

/*-------------------------------------------
 *         File input operations
 *-------------------------------------------*/
char * wave_name = NULL;
stereo_sample2 wave_buffer[wave_bufsize];
FILE * wave_file;
unsigned4 wave_bufferpos=wave_bufsize;

static int writer = -1;
static int writing = false;

/**
 * Bug# 975: the jackd daemon seems to spawn its own processes
 * To make sure that we retrieve the proper signal from the 
 * process we spawned, we retrieve the signal info and compare 
 * it against what we expect.
 */
void writer_died()
{
  char newstr[80];
  // now we have the complete length of the file...
  // if it differs from the length in the .ini file we update it
  // get length;
  writing = false;
  unsigned8 sec = samples2s(wave_max(),diskrate);
  unsigned8 min; 
  if (wave_file) 
    {
      min = sec/60;
      sec = sec%60;
      sprintf(newstr,"%02d:%02d",(int)min,(int)sec);
      playing->set_time(newstr);
    }
  msg_writing_finished();
}

void something_died(int sig, siginfo_t *info, void* hu)
{
  ExitStatus es;
  while(true)
    {
      es.pid=waitpid(-1,&es.status,WNOHANG);
      if (es.pid==-1)
	{
	  if (errno==ECHILD) return;
	  Warning("Playercore: error %d during wait: %s\n",
		  errno,strerror(errno));
	}
      else if (es.pid==0)
	return;
      
      // check the ones we expect to die
      if (es.pid==writer)
	writer_died();
      else
	Debug("Unknown process %d died. Ignoring",sig);
    }
}

int wave_open(Index * playing, bool synchronous)
{
  if (!playing) return err_none;
  const char * fname = strdup(playing->get_filename().toAscii().data());
  int decoder = set_decoder_environment(config,playing);
  if (!decoder) return err_noraw;
  
  // start decoding the file; this means: fork bpmdjraw process
  // wait 1 second and start reading the file
  if (synchronous)
    {
      writing = true;
      int result = bpmdjraw(true,fname,get_rawpath());
      writing = false;
      if (!result)
	return err_nospawn;
    }
  else 
    {
      // prepare signals
      struct sigaction *act = bpmdj_allocate(1,struct sigaction);
      act->sa_sigaction = something_died;
      act->sa_flags = SA_SIGINFO | SA_NOCLDSTOP;
      sigaction(SIGCHLD,act,NULL);
      // fork and execute, send back signal when done 
      writing = true;
      writer = bpmdjraw(false,fname,get_rawpath());
      assert(writer);
    }
  wave_name=getRawFilename(get_rawpath().toAscii().data(),fname);
  wave_file=fopen(wave_name,"rb");
  if (synchronous)
    {
      writer_died();
      if (!wave_file)
	return err_noraw;
    }
  return err_none;
}

void wave_close()
{
  if (wave_file)
    {
      fclose(wave_file);
      wave_file = NULL;
    }
  if (writing)
    {
      // send terminate (the insisting variant) signal
      assert(writer!=-1);
      kill(writer,SIGKILL);
      writing = false;
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

int wave_read(unsigned4 pos, stereo_sample2 *val)
{
  if (pos<wave_bufferpos || pos-wave_bufferpos>=wave_bufsize)
    {
      // we only stop one second later than necessary
      if (pos>=wave_max()+(long)diskrate) return -1;
      if (pos>=wave_max())
	{
	  wave_bufferpos=pos;
	  for(int i = 0 ; i < wave_bufsize ; i ++)
	    wave_buffer[i].value(0);
	  val->value(0);
	  return 0;
	}
      wave_bufferpos=pos;
      if (wave_bufferpos<0) wave_bufferpos=0;
      fseek(wave_file,pos*4,SEEK_SET);
      int r = fread(wave_buffer,4,wave_bufsize,wave_file);
      if (r>-1 && r < wave_bufsize)
	for(int i = r ; i < wave_bufsize ; i ++)
	  wave_buffer[i].value(0);
    }
  *val=wave_buffer[pos-wave_bufferpos];
  return 0;
}

/*-------------------------------------------
 *         Volume Synth operations
 *-------------------------------------------*/
unsigned8        lfo_phase_playrate=0;
quad_period_type lfo_period_playrate=0;
_lfo_   lfo_do;

void jumpto(signed8);

_lfo_ lfo_get()
{
  return lfo_do;
}

void lfo_set(const char* name, _lfo_ l, unsigned8 freq, unsigned8 phase)
{
  /* if paused, unpause and set phase */
  lfo_period_playrate=currentperiod_metarate.muldiv(4*dsp->playrate,freq*metarate);
  lfo_do=l;
  if (dsp->get_paused())
    {
      jumpto(0);
      lfo_phase_playrate=y_playrate;
      unpause_playing();
    }
  else
    {
      lfo_phase_playrate=phase;
    }
}

static signed4 volume = 100;
stereo_sample2 lfo_no(stereo_sample2 lt)
{
  return lt.muldiv(volume,100);
}

stereo_sample2 lfo_volume(stereo_sample2 x, 
			  signed4 multiplierleft, signed4 divisorleft, 
			  signed4 multiplierright,signed4 divisorright)
{
  return x.muldiv2(multiplierleft,divisorleft,multiplierright,divisorright);
}

stereo_sample2 lfo_metronome(stereo_sample2 x)
{
   float8 val;
   signed8 diff;
   diff=(signed8)y_playrate-(signed8)lfo_phase_playrate;
   diff=diff%lfo_period_playrate;
   /* say ping */
   val=sin(6.28*diff*(float8)(dsp->playrate)/440.0)*4096.0
     *(1.0-(float8)diff/(float8)lfo_period_playrate);
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
  diff=(signed8)y_playrate-(signed8)lfo_phase_playrate;
  diff=diff%lfo_period_playrate;
  quart=lfo_period_playrate/4;
  // at position 0, center 
  if (diff<quart)
    return lfo_volume(x,quart-diff,quart,1,1);
  // at position 0.25, right 
  diff-=quart;
  if (diff<quart)
    return lfo_volume(x,diff,quart,1,1);
  // at position 0.5 center 
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
   diff=(signed8)y_playrate-(signed8)lfo_phase_playrate;
   diff=diff%lfo_period_playrate;
   return lfo_volume(x,diff,lfo_period_playrate,diff,lfo_period_playrate);
}

stereo_sample2 lfo_break(stereo_sample2 x)
{
  signed8 diff;
  diff=(signed8)y_playrate-(signed8)lfo_phase_playrate;
  diff=diff%lfo_period_playrate;
  if (diff>lfo_period_playrate*95/100) 
    return stereo_sample2();
  else 
    return x;
}

stereo_sample2 lfo_revsaw(stereo_sample2 x)
{
  signed8 diff;
  diff=(signed8)y_playrate-(signed8)lfo_phase_playrate;
  while (diff<0 && lfo_period_playrate) 
    diff+=lfo_period_playrate;
  diff=diff%lfo_period_playrate;
  diff=lfo_period_playrate-diff;
  return lfo_volume(x,diff,lfo_period_playrate,diff,lfo_period_playrate);
}

void lfo_init()
{
  lfo_do=lfo_no;
}

/*-------------------------------------------
 *         Mapping Synth operations
 *-------------------------------------------*/
unsigned8   map_start_pos_diskrate = 0;
unsigned8   map_stop_pos_diskrate = 0;
bool        map_loop = false;
signed8     map_exit_pos_diskrate = 0;
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
  if (a<map_start_pos_diskrate) return a;
  if (a>=map_stop_pos_diskrate) 
    {
      if (map_loop)
	{
	  a=map_start_pos_diskrate;
	  x_diskrate = map_start_pos_diskrate;
	  y_playrate = y_normalise(x_diskrate);
	}
      else
	{
	  volume = 100;
	  if (map_exit_pos_diskrate == map_exit_stop)
	    {
	      x_diskrate = map_start_pos_diskrate;
	      pause_playing();
	    }
	  if (map_exit_pos_diskrate == map_exit_restart)
	    x_diskrate = map_start_pos_diskrate;
	  else if (map_exit_pos_diskrate == map_exit_continue)
	    x_diskrate = map_stop_pos_diskrate;
	  else 
	    x_diskrate = map_exit_pos_diskrate;
	  y_playrate = y_normalise(x_diskrate);
	  map_set_diskrate(0,NULL,0,map_exit_restart,false);
	  return x_diskrate;
	}
    }
  
  // determine fine grained relative position of 'wants to play'
  unsigned8 dx = a - map_start_pos_diskrate;
  // determine segment of 'wants to play'
  signed2 segment = dx*map_size/(map_stop_pos_diskrate-map_start_pos_diskrate);
#ifdef DEBUG_SEGSEQ
  static int last_segment = -1;
  if (segment!=last_segment)
    {
      last_segment=segment;
      Debug("segment = %d",segment);
    }
  assert(segment<map_size);
#endif
  // determine offset w.r.t. actual segment start
  unsigned8 segment_start = segment * (map_stop_pos_diskrate-map_start_pos_diskrate) / map_size;
  unsigned8 segment_offset = dx - segment_start;
  // obtain segment of new position
  signed2 new_segment = bpmdj_map[segment].take_from;
  
  // calculate speed
  segment_offset = ((signed4)segment_offset) 
    * ((signed4)bpmdj_map[segment].speed_mult) 
    / ((signed4)bpmdj_map[segment].speed_div);
  if (new_segment < 0)
    return map_stop_pos_diskrate;
  // transfer volume
  volume = bpmdj_map[segment].volume;
  // obtain new segment start position
  unsigned8 new_segment_start = new_segment 
    * (map_stop_pos_diskrate-map_start_pos_diskrate) / map_size;
  // fix should play position with actual map_start_pos and fine 
  // grained segment offset
  return new_segment_start+segment_offset+map_start_pos_diskrate;
}
  
void map_stop()
{
  if (map_do)
    {
      x_diskrate = map_active(x_diskrate);
      y_playrate = y_normalise(x_diskrate);
      map_do=false;
      if (bpmdj_map) bpmdj_deallocate(bpmdj_map);
      bpmdj_map=NULL;
    }
}

void map_set_diskrate(signed2 size, map_data inmap, unsigned8 msize, signed8 mexit, 
		      bool l)
{
  // if paused we start at the last cue position
  unsigned8 mstart;
  unsigned8 mstop;
  if (size > 0  && dsp->get_paused())
    mstart = metarate_to_diskrate(cue_metarate);
  else
    mstart = x_normalise(::y_playrate - dsp->latency());
  mstop = mstart + msize;
  map_exit_pos_diskrate = mexit;
  map_data old_map = bpmdj_map;
  // should we inactivate it ?
  if (size<=0) map_do=false;
  // should we modify first the size or the data ?
  // this is important because the map is still in use !
  if (size>map_size)
    {
      map_start_pos_diskrate = mstart;
      map_stop_pos_diskrate = mstop;
      bpmdj_map=inmap;
      map_size=size;
    }
  else
    {
      map_start_pos_diskrate = mstart;
      map_stop_pos_diskrate = mstop;
      map_size=size;
      bpmdj_map=inmap;
    }
  map_loop = l;
  // should we now activate it ?
  map_do = size > 0;
  // free old map ?
  if (old_map) bpmdj_deallocate(old_map);
  // unpause ?
  if (dsp->get_paused())
    {
      jumpto(0);
      unpause_playing();
    }
}

void map_init()
{
  map_do=false;
}

/*-------------------------------------------
 *         Cues
 *-------------------------------------------*/
static cue_info cue_before = 0;
cue_info cue_metarate = 0;
cue_info cues_metarate[4] = {0,0,0,0};

void cue_set()
{
  cue_metarate=diskrate_to_metarate(x_normalise(y_playrate-dsp->latency()));
}

void cue_shift_metarate(signed8 whence)
{
  if (whence < 0 && (unsigned8)(-whence) > cue_metarate) cue_metarate=0;
  else cue_metarate+=whence;
  if (!dsp->get_paused())
    pause_playing();
}

/**
 * Will write the current position into one of the cue points
 */
void cue_store(int nr)
{
  cues_metarate[nr]=cue_metarate;
}

/**
 * Will retrieve the cue from memory and place it in ::cue
 * It will effectively be used later in the jumpto function
 * which will also try to stay in sync with the current
 * position in the phrase.
 */
void cue_retrieve(int nr)
{
  cue_metarate=cues_metarate[nr];
}

void cue_write()
{
  if (playing)
    {
      playing->set_cue(cue_metarate);
      playing->set_cue_z(cues_metarate[0]);
      playing->set_cue_x(cues_metarate[1]);
      playing->set_cue_c(cues_metarate[2]);
      playing->set_cue_v(cues_metarate[3]);
    }
}

void cue_read()
{
  if (playing)
    {
      cue_before=cue_metarate=playing->get_cue();
      cues_metarate[0]=playing->get_cue_z();
      cues_metarate[1]=playing->get_cue_x();
      cues_metarate[2]=playing->get_cue_c();
      cues_metarate[3]=playing->get_cue_v();
    }
  else
    {
      cue_before = 0;
      cues_metarate[0]=cues_metarate[1]=cues_metarate[2]=cues_metarate[3]=0;
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
  float4 arg_rms = config->get_player_rms_target();
  
  sample4_type min = playing->get_min();
  sample4_type max = playing->get_max();
  sample4_type mean = playing->get_mean();
  power_type pow = playing->get_power();
  if (min.badly_defined() ||
      max.badly_defined() ||
      mean.badly_defined() || 
      pow.badly_defined())
    {
      Info("Switching off RMS normalization, "
	   "this song has no known energy levels");
      opt_rms=0;
      return;
    }
  
  // obtain amplitude maximization factor factor 
  left_sub = mean.left;
  left_fact = normalization_factor(min.left,max.left,mean.left);
  right_sub = mean.right;
  right_fact = normalization_factor(min.right,max.right,mean.right);
  
  // take into account the RMS
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
  float4 v = l->left;
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
      signed8 loop_at_diskrate = loop_off;
      signed8 loop_restart_diskrate = 0;

int loop_set_diskrate(unsigned8 jumpback)
{
  if (dsp->get_paused())
    {
      loop_at_diskrate = metarate_to_diskrate(cue_metarate);
      if (loop_at_diskrate<0) 
	loop_at_diskrate = loop_off;
      unpause_playing();
    }
  if (loop_at_diskrate == loop_off)
    {
      loop_at_diskrate=x_diskrate;
    }
  if (jumpback==0)
    loop_at_diskrate=loop_off;
  else
    if ((signed8)jumpback<=loop_at_diskrate)
      loop_restart_diskrate=loop_at_diskrate-jumpback;
    else
      loop_at_diskrate=loop_off;
  return loop_at_diskrate!=loop_off;
}

void loop_jump()
{
  x_diskrate=loop_restart_diskrate;
  y_playrate=y_normalise(x_diskrate);
}

/*-------------------------------------------
 *         Program logic
 *-------------------------------------------*/
void jumpto(signed8 mes)
{
  if (dsp->get_paused())
    {
      unpause_playing();
      y_playrate=y_normalise(metarate_to_diskrate(cue_metarate));
    }
  else
    {
      // - first we have the position which _should_ be playing 'NOW'
      // - second, we have the data which is now queued
      // - third we have the latency
      // - if we subtract the latency from the data being queued, we have the 
      //   real playing position
      // - if we subtract what should be playing with what is playing, we have 
      //   a difference
      // - we can fix this difference to fit a measure
      // - this fixed difference is added to gotopos
      signed8 currentperiod_playrate=currentperiod_metarate*dsp->playrate/metarate;
      signed8 gotopos_playrate=y_normalise(metarate_to_diskrate(cue_metarate))-currentperiod_playrate*mes;
      signed8 difference_playrate=gotopos_playrate-y_playrate;
      signed8 fixeddiff=(difference_playrate/currentperiod_playrate)*currentperiod_playrate;
      y_playrate+=fixeddiff;
    }
}

template<bool RMS>
class source_template: public audio_source
{
  stereo_sample2 value;
  unsigned8 m;
  virtual stereo_sample2 read()
  {
    x_diskrate = y_playrate * normalperiod_metarate * diskrate
      / (currentperiod_metarate * dsp->playrate);
    if (x_diskrate>loop_at_diskrate)
      loop_jump();
    if (map_do)
      m = map_active( x_diskrate );
    else
      m = x_diskrate;
    if (wave_read(m,&value)<0)
      {
	pause_playing();
	value.left=value.right= 0;
      };
    if (RMS)
      rms_normalize(&value);
    value=lfo_do(value);
    y_playrate++;
    return value;
  }
};

typedef source_template<false> bare_source;
typedef source_template<true> normalized_source;

void pause_playing()
{
  if (!dsp->get_paused())
    {
      dsp->pause();
      msg_playing_state_changed();
    }
}

void unpause_playing()
{
  if (dsp->get_paused())
    {
      dsp->unpause();
      msg_playing_state_changed();
    }
}

/*-------------------------------------------
 *         Startup code
 *-------------------------------------------*/ 
void copyright()
{
  printf(
 "BpmDj Player v%s, Copyright (c) 2000-2010 Werner Van Belle\n"
 "This software is distributed under the GPL2 license. See copyright.txt\n"
 "--------------------------------------------------------------------\n\n\n",
    VERSION);
  fflush(stdout);
  fflush(stderr);
}

int core_meta_init()
{
  copyright();
  common_init();
  if (opt_match)
    {
      Index target(arg_match);
      targetperiod_metarate=period_to_quad(target.get_period());
    }
  int L = strlen(argument);
  if (L<4 || strcmp(argument+L-4,".idx")!=0)
    return err_needidx;
  playing = new Index(argument);
  normalperiod_metarate=period_to_quad(playing->get_period());
  if (!opt_match) targetperiod_metarate=normalperiod_metarate;
  if (normalperiod_metarate<=0 && targetperiod_metarate>0) normalperiod_metarate=targetperiod_metarate;
  else if (normalperiod_metarate>0 && targetperiod_metarate<=0) targetperiod_metarate=normalperiod_metarate;
  currentperiod_metarate=targetperiod_metarate;
  cue_read();
  return err_none;
}

int core_object_init(bool sync)
{
  return wave_open(playing, sync);
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

int core_start(bool ui)
{
  int err = dsp->open(ui);
  if (err==err_dsp) 
    {
      dsp->close(false);
      finished = 1;
      return err;
    }
  if (!playing)
    {
      finished = 1;
      pause_playing();
      dsp->close(false);
      return err_none;
    }
  
  assert(playing);
  finished = false;
  
  rms_init();
  lfo_init();
  map_init();
  
  if (opt_rms)
    dsp->start(new normalized_source());
  else
    dsp->start(new bare_source());

  msg_playing_state_changed();  
  return err_none;
}

void core_stop()
{
  if(!finished)
    dsp->stop();
  finished = 1;
}

extern clock_driver* metronome;

void set_normalperiod_metarate(quad_period_type newnormalperiod, bool update_on_disk)
{
  assert(metronome);
  metronome->set_normalperiod_metarate(newnormalperiod);
  playing->set_period(newnormalperiod/4,update_on_disk);
}
#endif // __loaded__player_core_cpp__
