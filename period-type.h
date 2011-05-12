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
#ifndef __BPMDJ___PERIOD_TYPE_H__
#define __BPMDJ___PERIOD_TYPE_H__
using namespace std;
#line 1 "period-type.h++"
#include "files.h"
#include "types.h"

class period_type
{
 public:
  int period;
  static const int no_period = -1;
  period_type()
    {
      period = no_period;
    }
  period_type(int p)
    {
      period = p;
    }
  const bool none()
    {
      return period == no_period;
    }
  const bool valid()
    {
      return period > 0;
    }
  const period_type doubled()
    {
      return period_type(period*2);
    }
  const period_type halved()
    {
      return period_type(period/2);
    }
  const period_type lower(int delta)
    {
      if (period-delta < 100)
	return period_type(100);
      return period_type(period-delta);
    }
  const period_type higher(int delta)
    {
      return period_type(period+delta);
    }
};

class quad_period_type
{
 public:
  signed8 qperiod;
  static const int no_period = -1;
  quad_period_type()
    {
      qperiod = no_period;
    }
  quad_period_type(int p)
    {
      qperiod = p;
    }
  const bool none()
    {
      return qperiod == no_period;
    }
  const bool valid()
    {
      return qperiod > 0;
    }
  const quad_period_type doubled()
    {
      return quad_period_type(qperiod*2);
    }
  const quad_period_type halved()
    {
      return quad_period_type(qperiod/2);
    }
  const quad_period_type muldiv(signed8 mul, signed8 div)
    {
      return quad_period_type(qperiod*mul/div);
    }
  operator signed8()
    {
      return qperiod;
    }
};
#endif
