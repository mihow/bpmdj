/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

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
#ifndef __BPMDJ___RESAMPLER_H__
#define __BPMDJ___RESAMPLER_H__
using namespace std;
#line 1 "resampler.h++"
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <fftw3.h>
#include "memory.h"
#include "kbpm-play.h"
#include "stereo-sample2.h"
#include "period-type.h"
#include "player-core.h"

#ifdef INCOMPLETE_FEATURES

/**
 * The resampler below is implemented using polyphase decimation combined with a windowed lowpass filter with size 2*L
 */
class ImpulseResponse
{
  int *index;
  double *value;
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
  double gain();
  /**
   * divides the amplitude
   */
  ImpulseResponse& operator /=(double val);
  void set_amp(double val);
  double & amp(int i )
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
#endif
