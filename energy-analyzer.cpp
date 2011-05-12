/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle

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
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
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
#include <assert.h>
#include <math.h>
#include "energy-analyzer.h"
#include "version.h"
#include "player-core.h"
#include "scripts.h"
#include "common.h"
#include "memory.h"

void EnergyAnalyzer::readAudio()
{
  FILE * raw = openRawFile(playing,arg_rawpath);
  audiosize=fsize(raw);
  audiosize/=4;
  audio=allocate(audiosize+1,stereo_sample2);
  if (audiosize==0) return;
  readsamples(audio,audiosize,raw);
}

float4 normalization_factor(signed2 min, signed2 max, signed2 mean)
{
  float a = fabs(min - mean);
  float b = fabs(max - mean);
  float c = a > b ? a : b;
  return 32767.0 / c;
}

void EnergyAnalyzer::run()
{
  readAudio();
  // fetch the mean values
  playing->clear_energy();
  if (audiosize == 0) return;
  // scan for minima, maxima and mean
  signed2 minl = audio[0].left;
  signed2 minr = audio[0].right;
  signed2 maxl = audio[0].left;
  signed2 maxr = audio[0].right;
  signed8 meal = 0;
  signed8 mear = 0;
  signed8 divl = audiosize;
  signed8 divr = audiosize;
  for(unsigned8 i = 0 ; i < audiosize ; i ++)
    {
      signed2 l = audio[i].left;
      signed2 r = audio[i].right;
      if (l < minl) minl=l;
      if (r < minr) minr=r;
      if (l > maxl) maxl=l;
      if (r > maxr) maxr=r;
      meal += l;
      mear += r;
      if (meal > 0x0fffffffffffffffLL)
	{
	  meal/=2;
	  divl/=2;
	}
      if (mear > 0x0fffffffffffffffLL)
	{
	  mear/=2;
	  divr/=2;
	}
    }
  if (divl > 0) meal/=divl;
  if (divr > 0) mear/=divr;

  // obtain rms of normalized sound
  float8 facl = normalization_factor(minl, maxl, meal)/32767.0;
  float8 facr = normalization_factor(minr, maxr, mear)/32767.0;
  float8 subl = meal;
  float8 subr = mear;
  float8 powl = 0;
  float8 powr = 0;
  for(unsigned8 i = 0 ; i < audiosize ; i ++)
    {
      float8 l = audio[i].left;
      float8 r = audio[i].right;
      l -= subl;
      r -= subr;
      l *= facl;
      r *= facr;
      l *= l;
      r *= r;
      powl += l;
      powr += r;
    }
  powl /= audiosize;
  powr /= audiosize;
  powl = sqrt(powl);
  powr = sqrt(powr);
  
  // assign values to playing song
  playing->index_min.left=minl;
  playing->index_min.right=minr;
  playing->index_max.left=maxl;
  playing->index_max.right=maxr;
  playing->index_mean.left=meal;
  playing->index_mean.right=mear;
  playing->index_power.left=powl;
  playing->index_power.right=powr;
  playing->set_changed();
}

