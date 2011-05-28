/****
 BpmDj v4.2: Free Dj Tools
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
#ifndef __loaded__player_core_h__
#define __loaded__player_core_h__
using namespace std;
#line 1 "player-core.h++"
#include "dsp-drivers.h"
#include "index.h"

/*-------------------------------------------
 *         Wavrate conversion routines
 *-------------------------------------------*/ 
/**
 * metarate is the sample rate used to express measure lengths 
 * in the database, independent from diskrate or playrate. This
 * is normally set to 44100
 */
const int metarate=44100;
/**
 * Diskrate is the sample rate of the decoded file
 * Will be defined later on in clock-drivers.c++
 */
const int diskrate=44100;

#define samples2ms(samples,rate) (int)(1000*(samples)/rate)
#define samples2s(samples,rate) (int)(samples/rate)
#define mperiod2bpm(period,rate) (4.0*(float8)rate*60.0/(float8)(period))
#define ms2samples(ms,rate) (int)((ms)*rate/1000)
#define ms2bytes(ms,rate) (int)(4*ms2samples(ms,rate))
#define bytes2ms(bytes,rate) (int)(samples2ms(bytes,rate)/4)

inline signed4 metarate_to_diskrate(signed4 mr)
{
  return (signed8)mr*(signed8)diskrate/(signed8)metarate;
}

inline signed4 diskrate_to_metarate(signed4 dr)
{
  return (signed8)dr*(signed8)metarate/(signed8)diskrate;
}

inline signed4 metarate_to_playrate(signed4 mr)
{
  return (signed8)mr*(signed8)dsp->playrate/(signed8)metarate;
}

inline signed4 playrate_to_metarate(signed4 pr)
{
  return (signed8)pr*(signed8)metarate/(signed8)dsp->playrate;
}


#define wave_bufsize (32L*1024L)
typedef unsigned8 cue_info;
/**
 * cue points are expressed as positions (samples) in the song at metarate
 * ::cue is the current cue
 */
extern  cue_info cue_metarate;

/**
 * These are the for Z, X, C and V cues, in that order
 */
extern  cue_info cues_metarate[4];
extern  char* wave_name;
extern  Index* playing;
unsigned4 wave_max();
void      cue_set();
void      cue_store(int);
void      cue_retrieve(int);
void      jumpto(signed8);
void      cue_shift_metarate(signed8);

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
void map_set_diskrate(signed2 map_size, map_data m, unsigned8 size, 
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
int loop_set_diskrate(unsigned8 jumpback);

void set_normalperiod_metarate(quad_period_type newnormalperiod, 
			       bool update_on_disk=true);
#endif // __loaded__player_core_h__
