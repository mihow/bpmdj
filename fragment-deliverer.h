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
#ifndef __loaded__fragment_deliverer_h__
#define __loaded__fragment_deliverer_h__
using namespace std;
#line 1 "fragment-deliverer.h++"
#include "fragment-cache.h"
#include "dsp-drivers.h"

class FragmentDeliverer: public audio_source
{
public:
  FragmentInMemory playing;
  unsigned4 curpos; // the current playing position in the waveform
  stereo_sample2 * samples;
  bool finished;
  FragmentDeliverer()
  {
    finished=true;
    curpos=0;
    samples=NULL;
  }
  void reset(FragmentInMemory m)
  {
    finished=true;
    playing = m;
    curpos = 0;
    samples = playing.get_samples();
    if (samples) 
      finished=false;
  }
  virtual stereo_sample2 read()
  {
    if (finished) return stereo_sample2();
    stereo_sample2 value=samples[curpos++];
    curpos%=playing.get_size();
    if (curpos==0) 
      finished=true;
    return value;
  }
};
#endif // __loaded__fragment_deliverer_h__
