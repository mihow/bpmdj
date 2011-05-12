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

#include <assert.h>
#include <qstring.h>
#include <stdio.h>
#include "spectrum-type.h"
#include "signals.h"
#include "files.h"

spectrum_type::spectrum_type(const char* str)
{
  while(str[0]==' ' || str[0]=='\t') str++;
  if (strlen(str)==24)
    pre27init(str);
  else
    post27init(str);
}

void spectrum_type::pre27init(const char * txt)
{
  for(int i = 0 ; i < spectrum_size; i++)
    {
      spectrum_freq d = txt[i]-'a';
      if (i==0) d*=2;
      d/=barkbounds[i+1]-barkbounds[i];
      bark[i]=30+10*log(d)/log(10);
      if (isnan(bark[i])) bark[i]=0;
      else if (bark[i]<0) bark[i]=0;
    }
  //for(int i = 0 ; i < spectrum_size; i++)
  //    printf("%g ",bark[i]);
  //printf("\n");
  translate_mean(bark,spectrum_size);
};

void spectrum_type::post27init(const char * txt)
{
  const char *cur = txt;
  for(int i = 0 ; i < spectrum_size; i++)
    {
      char* nxt;
      bark[i]=strtof(cur,&nxt);
      cur=nxt;
    }
  // normalize_mean(bark,spectrum_size);
};

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

