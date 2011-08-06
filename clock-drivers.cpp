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
#ifndef __loaded__clock_drivers_cpp__
#define __loaded__clock_drivers_cpp__
using namespace std;
#include "clock-drivers.h"
#include "player-core.h"

quad_period_type targetperiod_metarate;
quad_period_type currentperiod_metarate;
quad_period_type normalperiod_metarate;
signed8 x_diskrate=0;
signed8 y_playrate=0;
// int diskrate=44100;
clock_driver *metronome = NULL;

void clock_driver::changetempo(signed8 period)
{
  /**
   * Change tempo to period
   * - the current x should remain the same
   * x = y * normalperiod / currentperiod
   * y = x * currentperiod / normalperiod
   */
  currentperiod_metarate = period;
  assert(dsp);
  y_playrate = x_diskrate * dsp->playrate * currentperiod_metarate / (normalperiod_metarate * diskrate); 
}

signed8 x_normalise(signed8 y_playrate)
{
  assert(dsp);
  return y_playrate*diskrate*normalperiod_metarate/(currentperiod_metarate*dsp->playrate);
}

signed8 y_normalise(signed8 x_diskrate)
{
  assert(dsp);
  return x_diskrate*dsp->playrate*currentperiod_metarate/(normalperiod_metarate*diskrate);
}

#endif // __loaded__clock_drivers_cpp__
