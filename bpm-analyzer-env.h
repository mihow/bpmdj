/****
 BpmDj v4.2-pl2: Free Dj Tools
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
#ifndef __loaded__bpm_analyzer_env_h__
#define __loaded__bpm_analyzer_env_h__
using namespace std;
#include <cstdio>
#include "bpm-analyzer-impl.h"

typedef float8 fft_type;

class BpmAnalyzerEnv : public BpmAnalyzerImpl
{
  const unsigned4 audiorate;
  const signed4 spectrum_shifter;
private:
  unsigned1 * audio;
  signed4     audiosize;
  unsigned4   startshift, stopshift;
  fft_type * freq;
  fft_type * peak_bpm;
  fft_type * peak_energy;
  signed4  * peak_fit;
  signed4    windowsize;
  signed4    peaks;
public:
  BpmAnalyzerEnv(stereo_sample2 *block, unsigned4 sample_length, 
		 float8 lower_boundary, float8 higher_boundary);
  virtual bool step();
};
#endif // __loaded__bpm_analyzer_env_h__
