/****
 BpmDj v4.2-pl2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

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
#ifndef __loaded__lowpass_cpp__
#define __loaded__lowpass_cpp__
using namespace std;
/**
 * The functions Izero and LpFilter are taken from 
 * Julius O. Smiths resample toolkit (resample-1.7.tar.gz)
 * The Digital Audio Resampling Home Page is located at
 * http://www-ccrma.stanford.edu/~jos/resample/.
 * The files these two functions are taken from is filterkit.c
 */

#include <math.h>
#include "lowpass.h"

#define IzeroEPSILON 1E-21               /* Max error acceptable in Izero */
static float8 Izero(float8 x)
{
   float8 sum, u, halfx, temp;
   int n;

   sum = u = n = 1;
   halfx = x/2.0;
   do {
      temp = halfx/(float8)n;
      n += 1;
      temp *= temp;
      u *= temp;
      sum += u;
      } while (u >= IzeroEPSILON*sum);
   return(sum);
}

void LpFilter(float8 c[], int N, float8 frq, float8 Beta, int Num)
{
   float8 IBeta, temp, inm1;
   int i;

   /* Calculate ideal lowpass filter impulse response coefficients: */
   c[0] = 2.0*frq;
   for (i=1; i<N; i++) 
     {
       temp = M_PI*(float8)i/(float8)Num;
       c[i] = sin(2.0*temp*frq)/temp; /* Analog sinc function, cutoff = frq */
     }
   
   /*
    * Calculate and Apply Kaiser window to ideal lowpass filter.
    * Note: last window value is IBeta which is NOT zero.
    * You're supposed to really truncate the window here, not ramp
    * it to zero. This helps reduce the first sidelobe. 
    */
   IBeta = 1.0/Izero(Beta);
   inm1 = 1.0/((float8)(N-1));
   for (i=1; i<N; i++) 
     {
       temp = (float8)i * inm1;
       c[i] *= Izero(Beta*sqrt(1.0-temp*temp)) * IBeta;
     }
}

#endif // __loaded__lowpass_cpp__
