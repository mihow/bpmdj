/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle

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

#ifndef SMALLHISTOGRAM_TYPE_H
#define SMALLHISTOGRAM_TYPE_H

#include <assert.h>
#include "basic-types.h"
#include "common.h"

class histogram_type;

template <int smallhistogram_size> class smallhistogram_type
{
 public:
  double scale;
  //  int    count;
  unsigned1 bin[smallhistogram_size];
  smallhistogram_type();
  void init(histogram_type * other);
  void init();
  void read_idx(const char*);
  const void write_idx(FILE * f);
  const void write_bib_v272(FILE * index);
  void read_bib_v271();
  void read_bib_v272();
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
  void set_scale(double s)
    {
      scale = s;
    }
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

#endif
