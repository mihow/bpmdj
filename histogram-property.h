/****
 BpmDj v3.6: Free Dj Tools
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
#ifndef __loaded__histogram_property_h__
#define __loaded__histogram_property_h__
using namespace std;
#line 1 "histogram-property.h++"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "memory.h"
#include "smallhistogram-type.h"
#include "spectrum-type.h"
#include "common.h"
#include "memory.h"
#include "signals.h"

template<int bandsize> class histogram_property
{
 public:
  smallhistogram_type<bandsize> *bark;

  histogram_property()
    {
      bark = NULL;
    }
  virtual ~histogram_property() {};
  histogram_property(histogram_type**arr)
    {
      init();
      for(int i = 0 ; i < spectrum_size ; i++)
	bark[i].init(arr[i]);
    }
  void clear()
    {
      bark=NULL;
    }
  bool empty() const
    {
      return bark==NULL;
    }
  virtual void init();

  Data get_data(int version) const;
  void set_data(Data& data); 
  unsigned1 get_freq_energy_probability(int band, int delay)
    {
      return bark[band].get_probability(delay);
    }
  float4 get_freq_energy_probability_scaled(int band, int delay)
    {
      return bark[band].get_probability_scaled(delay);
    }
  smallhistogram_type<bandsize> &get_band(int b)
    {
      return bark[b];
    }
  int get_count(int band)
    {
      return bandsize;
    }
};

template <int bandsize> void histogram_property<bandsize>::init()
{
  bark = new smallhistogram_type<bandsize>[spectrum_size]();
}

template <int bandsize> 
Data histogram_property<bandsize>::get_data(int version) const
{
  if (empty()) 
    return Array<1,Data>();
  Array<1,Data> result(spectrum_size);
  for(int i = 0 ; i < spectrum_size ; i++)
    result[i]=bark[i].get_data(version);
  return result;
}

template <int bandsize> 
void histogram_property<bandsize>::set_data(Data & data) 
{
  Array<1,Data> ar = data;
  if (ar.empty()) 
    bark = NULL;
  else
    {
      init();
      assert(ar.size(0)==spectrum_size);
      for(int i = 0 ; i < spectrum_size ; i++)
	bark[i].set_data(ar[i]);
    }
}
#endif // __loaded__histogram_property_h__
