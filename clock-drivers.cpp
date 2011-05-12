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
#ifndef __loaded__clock_drivers_cpp__
#define __loaded__clock_drivers_cpp__
using namespace std;
#line 1 "clock-drivers.c++"
#include "clock-drivers.h"
#include "player-core.h"

quad_period_type targetperiod;
quad_period_type currentperiod;
quad_period_type normalperiod;
signed8 x=0;
signed8 y=0;
clock_driver *metronome = NULL;

void clock_driver::changetempo(signed8 period)
{
  /**
   * Change tempo to period
   * - the current x should remain the same
   * x = y * normalperiod / currentperiod
   * y = x * currentperiod / normalperiod
   */
  currentperiod = period;
  y = x * currentperiod / normalperiod; 
}

signed8 x_normalise(signed8 y)
{
  return y*normalperiod/currentperiod;
}

signed8 y_normalise(signed8 x)
{
  return x*currentperiod/normalperiod;
}

#endif // __loaded__clock_drivers_cpp__
