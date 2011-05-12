/****
 BpmDj v4.1pl1: Free Dj Tools
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
#ifndef __loaded__resampler_h__
#define __loaded__resampler_h__
using namespace std;
#line 1 "resampler.h++"
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <fftw3.h>
#include "memory.h"
#include "bpmplay.h"
#include "stereo-sample2.h"
#include "period-type.h"
#include "player-core.h"

#ifdef INCOMPLETE_FEATURES

/**
 * The resampler below is implemented using poly-phase decimation combined with 
 * a windowed lowpass filter with size 2*L
 */
class ImpulseResponse
{
  int *index;
  float8 *value;
public:
  int length;
  ImpulseResponse() : length(0)
  {
    index = 0;
    value = 0;
  }
  ImpulseResponse(int size);
  stereo_sample2 convolve_with(stereo_sample2 * at);
  void print();
  /**
   * Returns the gain of the filter
   */
  float8 gain();
  /**
   * divides the amplitude
   */
  ImpulseResponse& operator /=(float8 val);
  void set_amp(float8 val);
  float8 & amp(int i )
  {
    assert(i>=0);
    assert(i<length);
    return value[i];
  }
  int & idx(int i )
  {
    assert(i>=0);
    assert(i<length);
    return index[i];
  }
  ImpulseResponse shrink();
};

class Resampler
{
private:
  static const int L = 20;
  int M; // = 85;
  quad_period_type table_generated_for;
  ImpulseResponse impulse_responses_at_mod[L];
  ImpulseResponse generate_table(int playpos);
public:
  Resampler()
    {
      M = -29;
    }
  void generate_table();
  ImpulseResponse& get_impulse_response(unsigned8 playpos)
  {
    if (table_generated_for!=currentperiod)
      generate_table();
    return impulse_responses_at_mod[playpos%L];
  }
  stereo_sample2 resample(stereo_sample2 * in);
};

extern Resampler resampler;
#endif
#endif // __loaded__resampler_h__
