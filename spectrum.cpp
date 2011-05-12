/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle
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

spectrum_freq scales[spectrum_size] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
spectrum_freq sums[spectrum_size] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static int spectra=0;

void new_spectrum(spectrum_type spectrum)
{
  if (spectrum==no_spectrum) return;
  spectra++;
  for(int i = 0 ;i < spectrum_size; i ++ )
    sums[i]+=spectrum[i];
}

void last_spectrum()
{
  for(int i = 0 ; i < spectrum_size ; i ++ )
    {
      // printf("mean of frequency band %d is %g\n",i,sums[i]/(float)spectra);
      scales[i]=(float)spectra/(float)sums[i];
    }
}
