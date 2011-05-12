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
#include "smallhistogram-type.h"
#include "histogram-type.h"
#include "common.h"
#include "memory.h"
#include "signals.h"
#include "files.h"

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

template <int smallhistogram_size> smallhistogram_type<smallhistogram_size>::smallhistogram_type()
{
}

template <int smallhistogram_size> const void smallhistogram_type<smallhistogram_size>::write_idx(FILE*f)
{
  fprintf(f,"%g %d ",scale,smallhistogram_size);
  for(int i = 0 ; i < smallhistogram_size ; i++)
    fprintf(f,"%2x ",bin[i]);
  fprintf(f,"\n");

}

//static long winst = 0;
template <int smallhistogram_size> void smallhistogram_type<smallhistogram_size>::read_idx(const char* str)
{
  char * cur;
  char * nxt;
  scale = strtof(str,&cur);
  validate_scale();
  int count = strtol(cur,&nxt,10);
  cur=nxt;
  if (count>smallhistogram_size) 
    count=smallhistogram_size;
  assert(count<=smallhistogram_size);
  if (count<0) count = 0;
  int i =0;
  for( ; i < count ; i++)
    {
      bin[i]=strtol(cur,&nxt,16);
      cur = nxt;
    }
  while(i<smallhistogram_size) 
    bin[i++]=0;
}

template <int smallhistogram_size> const void smallhistogram_type<smallhistogram_size>::write_bib_v272(FILE * index)
{
  file_float4(scale,index);
  file_unsigned1(smallhistogram_size,index);
  file_sequence(bin,smallhistogram_size,index);
}

template <int smallhistogram_size> void smallhistogram_type<smallhistogram_size>::read_bib_v271()
{
  scale = buffer_float4();
  validate_scale();
  buffer_sequence(bin,smallhistogram_size);
}

template <int smallhistogram_size> void smallhistogram_type<smallhistogram_size>::read_bib_v272()
{
  scale = buffer_float4();
  validate_scale();
  int count = buffer_unsigned1();
  assert(count>=0);
  buffer_sequence(bin,count);
  while(count<smallhistogram_size)
    bin[count++]=0;
}
