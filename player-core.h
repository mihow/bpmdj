/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001 Werner Van Belle
 See 'BeatMixing.ps' for more information

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

#include "common.h"
#define  WAVRATE  (44100)
#define  wave_bufsize (32L*1024L)
// one period is the length of 1 measure
extern  signed8 targetperiod;
extern  signed8 currentperiod;
extern  signed8 normalperiod;
extern  signed8 latency;
// x is the data position in the raw file (thus at normal tempo)
// y is the position in the playing file (thus at target tempo)
extern  signed8 y,x;
volatile extern  int stop;
volatile extern  int finished;
volatile extern  int paused;
typedef unsigned8 cue_info;
extern   cue_info cue;
extern   cue_info cues[4];
extern   char* wave_name;

unsigned4 wave_max();
unsigned8 x_normalise(unsigned8 y);
unsigned8 y_normalise(unsigned8 x);
void      cue_set();
void      cue_store(char*, int);
void      cue_retrieve(char*, int);
void      jumpto(signed8, int);
signed8   dsp_latency();
void      changetempo(signed8);
void      cue_shift(char*, signed8);
void      mixer_set_main(int percent);
void      mixer_set_pcm(int percent);
int       mixer_get_main();
int       mixer_get_pcm();
void      doubleperiod();
void      halveperiod();

// initializes the index fields
// and start reading the audio
void      core_init(int synchronous);
// opens the playing device
void      core_open();
// plays until asked to stop
void      core_play();
// closes the playing device
void      core_close();
// closes the wave and writes any changes to the index 
void      core_done();

unsigned4 lfo_no(unsigned4 x);
unsigned4 lfo_saw(unsigned4 x);
unsigned4 lfo_pan(unsigned4 x);
unsigned4 lfo_break(unsigned4 x);
unsigned4 lfo_difference(unsigned4 x);
unsigned4 lfo_revsaw(unsigned4 x);
unsigned4 lfo_metronome(unsigned4 x);
typedef unsigned4 (*_lfo_)(unsigned4 x);
void lfo_set(char* name, _lfo_ l, unsigned8 freq, unsigned8 phase);
// the jumpback is expressed at the normal speed of playing
int  loop_set(unsigned8 jumpback);

extern int   opt_debuglatency;
extern int   opt_quiet;
extern char* arg_dsp;
extern int   opt_match;
extern char* arg_match;
extern int   opt_latency;
extern char* arg_latency;
extern char* arg_mixer;
extern char* argument;

void line();

extern int app_init(int argc, char *argv[]);
extern void process_options(int argc,char *argv[]);
extern void show_error(char*msg);
//extern void terminal_start();
//extern void terminal_stop();
extern void show_error(char*msg);
extern void msg_slowdown(int c);
extern void msg_speedup(int c);
