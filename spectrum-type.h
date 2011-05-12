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

#ifndef SPECTRUM_TYPE_H
#define SPECTRUM_TYPE_H

#include <math.h>
#include "common.h"
#include "signals.h"
#include "files.h"

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
 private:
  spectrum_freq bark[spectrum_size];
 public:
  spectrum_type()
    {
      for(int i = 0 ; i < spectrum_size; i ++)
	bark[i]=0;
    };
  spectrum_type(spectrum_type * other)
    {
      memcpy(bark,other->bark,sizeof(spectrum_freq)*spectrum_size);
    }
  spectrum_type(const char* t);
  spectrum_freq band(int i) 
    {
      return bark[i];
    };
  void set_band(int i, spectrum_freq a)
    {
      bark[i]=a;
      if (i==spectrum_size-1)
	translate_mean(bark,spectrum_size);
    }
  void set_bark(int i, spectrum_freq a)
    {
      bark[i]=a;
    }
  spectrum_freq get_bark(int i)
    {
      return bark[i];
    }
  void pre27init(const char* txt);
  void post27init(const char * txt);
  const void write_idx(FILE * f)
    {
      fprintf(f,"spectrum : ");
      for(int i = 0 ; i < spectrum_size ; i++)
	fprintf(f,"%g ",bark[i]);
      fprintf(f,"\n");
    }
  const void write_bib_v27(FILE * index)
    {
      for(int i = 0 ; i < spectrum_size ; i ++)
	file_float4(bark[i],index);
    }
  void read_bib_v27()
    {
      for(int i = 0 ; i < spectrum_size ; i ++)
	bark[i]=buffer_float4();
    }
  
};

#endif
