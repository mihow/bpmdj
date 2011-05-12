/****
 BpmDj v3.6: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

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
#ifndef __loaded__player_core_h__
#define __loaded__player_core_h__
using namespace std;
#line 1 "player-core.h++"
#include "dsp-drivers.h"
#include "index.h"

/*-------------------------------------------
 *         Wavrate conversion routines
 *-------------------------------------------*/ 
#define WAVRATE (44100)
#define  samples2ms(samples) (int)(1000*(samples)/WAVRATE)
#define  bytes2ms(bytes) (int)samples2ms((bytes)/4)
#define  ms2samples(ms) (int)((ms)*WAVRATE/1000)
#define  ms2bytes(ms) (int)(4*ms2samples(ms))
#define  samples2s(samples) (int)(samples)/WAVRATE
#define  mperiod2bpm(period) (4.0*(double)WAVRATE*60.0/(double)(period))

/*-------------------------------------------
 *         Other prullaria
 *-------------------------------------------*/ 
#define  wave_bufsize (32L*1024L)
// one period is the length of 1 measure
extern  quad_period_type targetperiod;
extern  quad_period_type currentperiod;
extern  quad_period_type normalperiod;
// x is the data position in the raw file (thus at normal tempo)
// y is the position in the playing file (thus at target tempo)
extern  signed8 y,x;
typedef unsigned8 cue_info;
extern  cue_info cue;
extern  cue_info cues[4];
extern  char* wave_name;
extern  Index* playing;
unsigned4 wave_max();
signed8   x_normalise(signed8 y);
signed8   y_normalise(signed8 x);
void      cue_set();
void      cue_store(int);
void      cue_retrieve(int);
void      jumpto(signed8, int);
void      changetempo(signed8);
void      cue_shift(signed8);

extern dsp_driver *dsp;

#define   err_none    0
// initializes the index fields
// and start reading the audio
#define   err_needidx 1
#define   err_noraw   2
#define   err_nospawn 3
int       core_meta_init();
int       core_object_init(bool sync);
#define   err_dsp     5
int       core_open();
// plays until asked to stop
void      core_play();
bool      get_paused();
void      pause_playing();
void      unpause_playing();
void      wait_for_unpause();
// closes the playing device
void      core_close();
// closes the wave and writes any changes to the index 
void      core_done();
// starts the core if the wave is already opened
int       core_start();
// waits for the core to finish, does nothing else
void      core_stop();

stereo_sample2 lfo_no(stereo_sample2 x);
stereo_sample2 lfo_saw(stereo_sample2 x);
stereo_sample2 lfo_pan(stereo_sample2 x);
stereo_sample2 lfo_break(stereo_sample2 x);
stereo_sample2 lfo_difference(stereo_sample2 x);
stereo_sample2 lfo_revsaw(stereo_sample2 x);
stereo_sample2 lfo_metronome(stereo_sample2 x);
typedef stereo_sample2 (*_lfo_)(stereo_sample2 x);
void  lfo_set(char* name, _lfo_ l, unsigned8 freq, unsigned8 phase);
_lfo_ lfo_get();

typedef struct t_segment {
  signed2 take_from;
  signed2 speed_mult;
  signed2 speed_div;
  signed2 volume; // per 100
} map_segment;
typedef map_segment* map_data;

#define map_exit_restart -1
#define map_exit_continue -2
#define map_exit_stop -1000
// the exit is expressed at the normal speed of playing
void map_set(signed2 map_size, map_data m, unsigned8 size, signed8 exit, bool loop);
void map_loop_set(bool l);
void map_stop();

extern int   opt_quiet;
extern int   opt_match;
extern char* arg_match;
extern char* argument;
extern bool  opt_check;

extern int app_init(int argc, char *argv[]);
extern void process_options(int argc,char *argv[]);
extern void msg_playing_state_changed();
extern void msg_writing_finished();

// exported functions
FILE * openCoreRawFile();
#endif // __loaded__player_core_h__
