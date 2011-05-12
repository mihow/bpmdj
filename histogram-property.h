/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle
 See 'BeatMixing.ps' for more information

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

#ifndef HISTOGRAM_PROPERTY_H
#define HISTOGRAM_PROPERTY_H

#include "memory.h"
#include "histogram-type.h"
#include "spectrum-type.h"

class histogram_property
{
 public:
  smallhistogram_type *bark;
  histogram_property()
    {
      bark = NULL;
    }
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
  bool empty()
    {
      return bark==NULL;
    }
  virtual void init()
    {
      bark = new smallhistogram_type[spectrum_size]();
    }
  void read_idx(const char* str);
  const void write_idx(FILE * f, char* fieldname);
  const void write_bib_v272(FILE * index);
  void read_bib_v271();
  void read_bib_v272();
  unsigned1 get_freq_energy_probability(int band, int delay)
    {
      return bark[band].get_probability(delay);
    }
  float4 get_freq_energy_probability_scaled(int band, int delay)
    {
      return bark[band].get_probability_scaled(delay);
    }
  smallhistogram_type get_band(int b)
    {
      return bark[b];
    }
  int get_count(int band)
    {
      return bark[band].count;
    }
};

#endif
