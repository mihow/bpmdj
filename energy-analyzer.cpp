/****
 BpmDj v3.8: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

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
#ifndef __loaded__energy_analyzer_cpp__
#define __loaded__energy_analyzer_cpp__
using namespace std;
#line 1 "energy-analyzer.c++"
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
#include <qstring.h>
#include <qdialog.h>
#include <math.h>
#include "energy-analyzer.h"
#include "version.h"
#include "player-core.h"
#include "scripts.h"
#include "common.h"
#include "memory.h"

void EnergyAnalyzer::readAudio()
{
  FILE * raw = openCoreRawFile();
  audiosize=fsize(raw);
  audiosize/=4;
  audio=bpmdj_allocate(audiosize+1,stereo_sample2);
  /* Fixes bug: #964 */
  if (audiosize==0) return;
  readsamples(audio,audiosize,raw);
}

float4 normalization_factor(signed2 min, signed2 max, signed2 mean)
{
  float4 a = fabs(min - mean);
  float4 b = fabs(max - mean);
  float4 c = a > b ? a : b;
  if (c==0) return 32767;
  return 32767.0 / c;
}

void EnergyAnalyzer::analyze()
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
  for(unsigned8 i = 0 ; i < audiosize ; i ++)
    {
      signed2 l = audio[i].left;
      signed2 r = audio[i].right;
      if (l < minl) minl=l;
      if (r < minr) minr=r;
      if (l > maxl) maxl=l;
      if (r > maxr) maxr=r;
    }
  
  signed8 meanl = 0;
  signed8 meanr = 0;
  signed8 bs = 0;
  /* Fixes bug: #964 */
  for(unsigned8 b = 0 ; b + 65536 < audiosize ; b+=65536)
    {
      signed8 smeanl = 0;
      signed8 smeanr = 0;
      for(unsigned8 i = 0 ; i < 65536 ; i ++)
	{
	  signed2 l = audio[i].left;
	  signed2 r = audio[i].right;
	  smeanl += l;
	  smeanr += r;
	}
      smeanl/=65536;
      smeanr/=65536;
      meanl+=smeanl;
      meanr+=smeanr;
      bs++;
    }
  if (bs)
    {
      meanl/=bs;
      meanr/=bs;
      
      // obtain rms of normalized sound
      float8 facl = normalization_factor(minl, maxl, meanl)/32767.0;
      float8 facr = normalization_factor(minr, maxr, meanr)/32767.0;
      float8 subl = meanl;
      float8 subr = meanr;
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
      playing->index_mean.left=meanl;
      playing->index_mean.right=meanr;
      playing->index_power.left=powl;
      playing->index_power.right=powr;
      playing->set_changed();
    }
  else // mean could not be calculated
    {
      // assign values to playing song
      playing->index_min.left=0;
      playing->index_min.right=0;
      playing->index_max.left=0;
      playing->index_max.right=0;
      playing->index_mean.left=0;
      playing->index_mean.right=0;
      playing->index_power.left=0;
      playing->index_power.right=0;
      playing->set_changed();
    }
}

#endif // __loaded__energy_analyzer_cpp__
