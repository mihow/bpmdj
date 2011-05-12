/****
 BpmDj v3.6: Free Dj Tools
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
#ifndef __loaded__smallhistogram_type_h__
#define __loaded__smallhistogram_type_h__
using namespace std;
#line 1 "smallhistogram-type.h++"
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "common.h"
#include "om-data.h"
#include "histogram-type.h"
#include "common.h"
#include "memory.h"
#include "signals.h"
#include "files.h"

class histogram_type;
extern Symbol key_content;
extern Symbol key_scale;

template <int smallhistogram_size> class smallhistogram_type
{
 public:
  double scale;
  //  int    count;
  unsigned1 bin[smallhistogram_size];
  smallhistogram_type() 
    {
    };
  void init(histogram_type * other);
  void init();
  void set_energy(int x, unsigned1 v)
    {
      assert(x<smallhistogram_size);
      bin[x]=v;
    }
  unsigned1 get_energy(int x)
    {
      //      if (x>=count) return 127;
      assert(x<smallhistogram_size);
      return bin[x];
    }
  Data get_data(int version) const;
  void set_data(Data& data);
  void set_scale(double s)
    {
      scale = s;
      validate_scale();
    }
  void validate_scale();
  unsigned1 get_probability(int delay)
    {
      // assert(delay<count);
      return bin[delay];
    }
  float4 get_probability_scaled(int delay)
    {
      // assert(delay<count);
      return (float4)bin[delay]*scale;
    }
};

template <int smallhistogram_size> void smallhistogram_type<smallhistogram_size>::validate_scale()
{
  if (isnan(scale) || isinf(scale))
    scale = 0;
}

template <int smallhistogram_size> void smallhistogram_type<smallhistogram_size>::init(histogram_type * other)
{
  set_scale(other->scale);
  if (other->count<smallhistogram_size)
    {
      for(int i = 0 ; i < other->count; i++)
	bin[i]=abs(other->bins[i]);
      for(int i = other->count ; i < smallhistogram_size ; i ++)
	bin[i]=0;
    }
  else
    {
      for(int i = 0 ; i < smallhistogram_size; i++)
	bin[i]=abs(other->bins[i]);
    }
}

template <int smallhistogram_size> void smallhistogram_type<smallhistogram_size>::init()
{
  for(int i = 0 ; i < smallhistogram_size; i++)
    bin[i]=0;
}

template <int smallhistogram_size> Data smallhistogram_type<smallhistogram_size>::get_data(int version) const
{
  Token result;
  result[key_scale]=Float8(scale);
  Array<1,unsigned1> a(smallhistogram_size);
  result[key_content]=a;
  for(int i = 0 ; i < smallhistogram_size ; i++)
    a[i]=bin[i];
  return result;
}

template <int smallhistogram_size> 
void smallhistogram_type<smallhistogram_size>::set_data(Data& data)
{
  Token result = data;
  scale = (Float8)result[key_scale];
  Array<1,unsigned1> a = result[key_content];
  // can we make use of an ordere array ?
  typename Array<1,unsigned1>::ordered i(a);
  if (i.linear() && i.size()==smallhistogram_size)
    memcpy(bin,i.current(),smallhistogram_size);
  // if not copy one by one
  else
    for(int i = 0 ; i < smallhistogram_size ; i++)
      bin[i]=a[i];
}
#endif // __loaded__smallhistogram_type_h__
