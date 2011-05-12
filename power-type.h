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
#ifndef __loaded__power_type_h__
#define __loaded__power_type_h__
using namespace std;
#line 1 "power-type.h++"
#include <math.h>
#include "om-data.h"

const float no_power = -1.0;

class power_type
{
 public:
  float left;
  float right;
  bool fully_defined() 
    {
      return 
	left!=no_power && 
	right!=no_power;
    };
  bool badly_defined() 
    {
      return 
	!fully_defined() ||
	!isnormal(left) ||
	!isnormal(right);
    };
  power_type()
    {
      right = left = no_power;
    }
  power_type(float l, float r)
    {
      left = l;
      right = r;
    }
  Data get_data(int version) const;
  void set_data(Data d);
  void read(char*);
};

#endif // __loaded__power_type_h__
