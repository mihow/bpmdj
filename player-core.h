/****
 BpmDj v4.1: Free Dj Tools
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
#define  mperiod2bpm(period) (4.0*(float8)WAVRATE*60.0/(float8)(period))

#define  wave_bufsize (32L*1024L)
typedef unsigned8 cue_info;
/**
 * cue points are expressed as positions (samples) in the non-stretched song
 * ::cue is the current cue
 */
extern  cue_info cue;

/**
 * These are the for Z, X, C and V cues, in that order
 */
extern  cue_info cues[4];
extern  char* wave_name;
extern  Index* playing;
unsigned4 wave_max();
void      cue_set();
void      cue_store(int);
void      cue_retrieve(int);
void      jumpto(signed8, int);
void      cue_shift(signed8);

/**
 * Initializes the index fields and start reading the audio
 * The error can be err_none (wanted !), err_needidx, err_noraw, err_nospawn, 
 * or err_dsp.
 */
int       core_meta_init();
int       core_object_init(bool sync);
#define   err_none    0
#define   err_needidx 1
#define   err_noraw   2
#define   err_nospawn 3
#define   err_dsp     5

/**
 * Asks the player to pause its playing. This function must return immediately
 * and simply signals a pause request
 */
void      pause_playing();
void      unpause_playing();

/**
 * Closes the wave and writes any changes to the index
 */
void      core_done();

/**
 * Starts the core if the wave is already opened. If user interface is true it
 * will use QMessageBox to display errors.
 */
int       core_start(bool ui);
/**
 * Waits for the core to finish, does nothing else
 */
void      core_stop();

stereo_sample2 lfo_no(stereo_sample2 x);
stereo_sample2 lfo_saw(stereo_sample2 x);
stereo_sample2 lfo_pan(stereo_sample2 x);
stereo_sample2 lfo_break(stereo_sample2 x);
stereo_sample2 lfo_difference(stereo_sample2 x);
stereo_sample2 lfo_revsaw(stereo_sample2 x);
stereo_sample2 lfo_metronome(stereo_sample2 x);
typedef stereo_sample2 (*_lfo_)(stereo_sample2 x);
void  lfo_set(const char* name, _lfo_ l, unsigned8 freq, unsigned8 phase);
_lfo_ lfo_get();

typedef struct t_segment 
{
  signed2 take_from;
  signed2 speed_mult;
  signed2 speed_div;
  signed2 volume; // per 100
} map_segment;
typedef map_segment* map_data;

#define map_exit_restart -1
#define map_exit_continue -2
#define map_exit_stop -1000
// exit is expressed at normal playing speed
void map_set(signed2 map_size, map_data m, unsigned8 size, 
	     signed8 exit, bool loop);
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

/**
 * This function is called by the player core when 
 * the writing process finished its execution.
 */
extern void msg_writing_finished();
FILE * openCoreRawFile();
int loop_set(unsigned8 jumpback);

void set_normalperiod(quad_period_type newnormalperiod, 
 		      bool update_on_disk=true);
#endif // __loaded__player_core_h__
