/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

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
#include <qstring.h>
#include <stdio.h>
#include <stdlib.h>
#include "spectrum-type.h"
#include "signals.h"
#include "files.h"

Data spectrum_type::get_data() const
{
  Array<1,float4> spectrumdata(spectrum_size);
  for(int i = 0 ; i < spectrum_size ; i++)
    spectrumdata[i]=bark[i];
  return spectrumdata;
}

void spectrum_type::set_data(Data &data)
{
  Array<1,float4> spectrumdata = data;
  for(int i = 0 ; i < spectrum_size ; i++)
    bark[i]=spectrumdata[i];
}

double barkbounds[barksize+1] =
  {
    0,100,200,300,
    400,510,630,770,
    920,1080,1270,1480,
    1720,2000,2380,2700,
    3150,3700,4400,5300,
    6400,7700,9500,12000,
    15500
  };

  /*
double barkbounds[barksize+1] =
  {
    0,50,100,150,200,250,300,350,
    400,455,510,570,630,700,770,845,
    920,1000,1080,1175,1270,1375,1480,1600,
    1720,1860,2000,2190,2380,2540,2700,2925,
    3150,3425,3700,4050,4400,4850,5300,5850,
    6400,7050,7700,8600,9500,10750,12000,13750,
    15500
  };
*/

