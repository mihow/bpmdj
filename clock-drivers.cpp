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
#ifndef __loaded__clock_drivers_cpp__
#define __loaded__clock_drivers_cpp__
using namespace std;
#line 1 "clock-drivers.c++"
#include "clock-drivers.h"
quad_period_type targetperiod;
quad_period_type currentperiod;
quad_period_type normalperiod;
signed8 x=0;
signed8 y=0;
clock_driver *metronome = NULL;

void changetempo(signed8 period)
{
  /**
   * Change tempo to period
   * - the current x should remain the same
   * x = y * normalperiod / currentperiod
   * y = x * currentperiod / normalperiod
   */
  if (metronome)
    metronome->tempo_will_change(period);
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
