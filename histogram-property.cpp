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

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "histogram-property.h"
#include "common.h"
#include "memory.h"
#include "signals.h"

template <int bandsize> void histogram_property<bandsize>::read_idx(const char *str)
{
  if (empty()) 
    init();
  char *rest;
  int band = strtol(str,&rest,10);
  assert(band>=0 && band<spectrum_size);
  bark[band].read_idx(rest);
}

template <int bandsize> const void histogram_property<bandsize>::write_idx(FILE*f, char* name)
{
  if (empty()) return;
  for(int i = 0 ; i < spectrum_size ; i++)
    {
      fprintf(f,"%s : %d ",name,i);
      bark[i].write_idx(f);
    }
  fprintf(f,"\n");
}

template <int bandsize> const void histogram_property<bandsize>::write_bib_v272(FILE * index)
{
  if (empty()) 
    file_unsigned1(0,index);
  else 
    {
      file_unsigned1(spectrum_size,index);
      for(int i = 0 ; i < spectrum_size ; i ++)
	bark[i].write_bib_v272(index);
    }
}

template <int bandsize> void histogram_property<bandsize>::read_bib_v271()
{
  unsigned1 l = buffer_unsigned1();
  if (l==0) return;
  assert(l==spectrum_size);
  init();
  for(int i = 0 ; i < spectrum_size ; i++)
    bark[i].read_bib_v271();
}

template <int bandsize> void histogram_property<bandsize>::read_bib_v272()
{
  unsigned1 l = buffer_unsigned1();
  if (l==0) return;
  assert(l==spectrum_size);
  init();
  for(int i = 0 ; i < spectrum_size ; i++)
    bark[i].read_bib_v272();
}


// instatiating some dummys
static const histogram_property<32> ignore_1;
static const histogram_property<96> ignore_2;
