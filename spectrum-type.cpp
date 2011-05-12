/****
 BpmDj v4.1pl1: Free Dj Tools
 Copyright (C) 2001-2010 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.

 See the authors.txt for a full list of people involved.
****/
#ifndef __loaded__spectrum_type_cpp__
#define __loaded__spectrum_type_cpp__
using namespace std;
#line 1 "spectrum-type.c++"
#include <assert.h>
#include <qstring.h>
#include <stdio.h>
#include <stdlib.h>
#include "index.h"
#include "signals.h"
#include "files.h"

Data spectrum_type::get_data() const
{
  Array<1,float4> spectrumdata(spectrum_size);
  for(signed4 i = 0 ; i < spectrum_size ; i++)
    spectrumdata[i]=bark[i];
  return spectrumdata;
}

void spectrum_type::set_data(Data &data)
{
  Array<1,float4> spectrumdata = data;
  for(signed4 i = 0 ; i < spectrum_size ; i++)
    {
      float4 f=spectrumdata[i];
      if (isnan(f))
	throw new DataError("Spectrum float at is nan");
      if (isinf(f))
	throw new DataError("Spectrum float at is inf");
      bark[i]=f;
    }
}

float8 barkbounds[barksize+1] =
  {
    0,100,200,300,
    400,510,630,770,
    920,1080,1270,1480,
    1720,2000,2380,2700,
    3150,3700,4400,5300,
    6400,7700,9500,12000,
    15500
  };
#endif // __loaded__spectrum_type_cpp__
