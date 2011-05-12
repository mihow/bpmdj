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

#include <assert.h>
#include <math.h>
#include "common.h"

signed8 higher_power_of_two(signed8 a)
{
  signed8 c = 1;
  while(c < a) c<<=1;
  return c;
}

double find_max(double * data, long l)
{
  double m;
  for(long i = 0 ; i < l ; i ++)
    if (data[i]>m)
      m=data[i];
  return m;
}

double find_abs_max(double * data, long l)
{
  if (l==0) return 0;
  double m = data[0];
  for(long i = 0 ; i < l ; i ++)
    if (fabs(data[i])>m)
      m=fabs(data[i]);
  return m;
}

void normalize_abs_max(double * data, long l)
{
  double m = find_abs_max(data,l);
  printf("normalizer: maximum is %g\n",m);
  if (m>0)
    for(long i = 0 ; i < l ; i ++)
      data[i]/=m;
}
