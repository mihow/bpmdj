/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001 Werner Van Belle
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

#include <qstring.h>
#include <stdio.h>
#include "spectrum.h"

float scales[24] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
int sums[24] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static int spectra=0;

void newSpectrum(QString spectrum)
{
  if (spectrum.isNull()) return;
  spectra++;
  for(int i =0 ;i <24;i++)
    {
      char letter = spectrum.at(i).latin1();
      int value = letter- 'a';
      sums[i]+=value;
    }
}

void lastSpectrum()
{
  for(int i=0;i<24;i++)
    {
      scales[i]=(float)spectra/(float)sums[i];
      scales[i]*=5;
    }
}
