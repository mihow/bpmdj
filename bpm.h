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
#ifndef __loaded__bpm_h__
#define __loaded__bpm_h__
using namespace std;
#line 1 "bpm.h++"
/**-------------------------------------------
 * 		Imports
 **-------------------------------------------*/
#include <fftw3.h>
#include "type-operations.h"

/**-------------------------------------------
 * 		Imports
 **-------------------------------------------*/
class BpmCounter
{
private:
  unsigned4 measured;
  unsigned4 sample_length;
  stereo_sample2* block;
  float8* audio;
  unsigned4 b1, b2;
  float8 * en;
  float8 * ts;
  float8 * co;
  fftw_plan forward;
  fftw_plan backward;
  int      input_rate;
  int      measure_rate;
  FILE   * log;
public:
  BpmCounter(FILE * l, stereo_sample2 *block, unsigned4 sample_length, 
	     int wavrate, float8 lower_boundary, float8 higher_boundary) 
  {
    log = l;
    init(sample_length,block,wavrate, lower_boundary,higher_boundary);
  };
  ~BpmCounter();
  void init(unsigned4 sample_length, stereo_sample2 *block, int wavrate, 
	    float8 lower_boundary, float8 higher_boundary);
  float8 measure();
};
#endif // __loaded__bpm_h__
