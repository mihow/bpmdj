/****
 BpmDj v4.2: Free Dj Tools
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
#ifndef __loaded__hues_cpp__
#define __loaded__hues_cpp__
using namespace std;
#line 1 "hues.c++"
#include <math.h>
#include "hues.h"

QRgb hue2rgb[huecolors];

void set_hue2rgb_channels(int idx, int r, int g, int b, double I)
{
  if (idx<0) return;
  if (idx>=huecolors) return;
  hue2rgb[idx]=qRgb(r,g,b);
}

void init_hues()
{
  int s1=0;
  int s2=huecolors/3;
  int s3=2*huecolors/3;
  int s4=huecolors;
  int ss=1+(huecolors/3);
  for(int i = 0; i <= ss/2; i++)
    {
      double a=sin(i*M_PI/ss);
      int S=255.0*a;
      double j=2.0*(double)i/(double)ss;
      set_hue2rgb_channels(s1+i,255,S,0,j);
      set_hue2rgb_channels(s2+i,0,255,S,j);
      set_hue2rgb_channels(s3+i,S,0,255,j);
      set_hue2rgb_channels(s2-i,S,255,0,j);
      set_hue2rgb_channels(s3-i,0,S,255,j);
      set_hue2rgb_channels(s4-i,255,0,S,j);
    }
}

QRgb get_hue(unsigned2 h, unsigned1 i)
{
  QRgb rgb=hue2rgb[h];
  return qRgb(qRed(rgb)*i/255,
	      qGreen(rgb)*i/255,
	      qBlue(rgb)*i/255);
}
#endif // __loaded__hues_cpp__
