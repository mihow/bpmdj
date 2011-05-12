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
#ifndef __loaded__spectrum_type_h__
#define __loaded__spectrum_type_h__
using namespace std;
#line 1 "spectrum-type.h++"
#include <math.h>
#include "common.h"
#include "signals.h"
#include "files.h"
#include "om-data.h"

#define spectrum_size 24
#define barksize spectrum_size
extern double barkbounds[barksize+1];

/**
 * Versions <2.7 used a lineair scale
 * Versions >=2.7 use a logartihmic scale in dB normalized to 1
 */
typedef float spectrum_freq;
extern double barkbounds[barksize+1];
#define no_spectrum NULL

class spectrum_type
{
 public:
  spectrum_freq bark[spectrum_size];
  spectrum_type()
  {
    for(signed4 i = 0 ; i < spectrum_size; i ++)
      bark[i]=0;
  };
  spectrum_type(Data &d)
  {
    set_data(d);
  }
  spectrum_type(spectrum_type * other)
  {
    memcpy(bark,other->bark,sizeof(spectrum_freq)*spectrum_size);
  }
  spectrum_freq band(int i) 
  {
    return bark[i];
  };
  void set_band(signed4 i, spectrum_freq a)
  {
    bark[i]=a;
    if (i==spectrum_size-1)
      translate_mean(bark,spectrum_size);
  }
  void set_bark(signed4 i, spectrum_freq a)
  {
    bark[i]=a;
  }
  spectrum_freq get_bark(signed4 i)
  {
    return bark[i];
  }
  Data get_data() const;
  void set_data(Data &data);
};
#endif // __loaded__spectrum_type_h__
