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

extern "C"
{
#include "common.h"
}

#define  WAVRATE  (22050)
#define  wave_bufsize (32L*1024L)
extern  signed8 targetperiod;
extern  signed8 currentperiod;
extern  signed8 normalperiod;
extern  signed8 latency;
extern  signed8 y,x;
extern      int stop;
extern      int paused;
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
