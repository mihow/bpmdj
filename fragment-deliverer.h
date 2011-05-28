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
  void markFinished();
  virtual stereo_sample2 read()
  {
    if (finished) return stereo_sample2();
    stereo_sample2 value=samples[curpos++];
    curpos%=playing.get_size();
    if (curpos==0)
      markFinished();
    return value;
  }
};
#endif // __loaded__fragment_deliverer_h__
