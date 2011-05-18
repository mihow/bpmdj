/****
 BpmDj v4.2 beta: Free Dj Tools
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
#ifndef __loaded__stereo_sample2_h__
#define __loaded__stereo_sample2_h__
using namespace std;
#line 1 "stereo-sample2.h++"
#include "Data/data.h"
const float4 signed2_sample_max_f =32768.0f;

class stereo_sample2 
{
public:
  signed2 left;
  signed2 right;
  stereo_sample2()
  {
    left = 0;
    right = 0;
  }
  stereo_sample2(signed short int l, signed short int r)
  {
    left = l;
    right = r;
  }
  signed4 summed()
  {
    return (signed4)left+(signed4)right;
  }
  unsigned4 & value()
  {
    return *((unsigned4*)((void*)&left));
  }
  void value(unsigned4 v)
  {
    union 
    {
      struct
      {
	signed2 left;
	signed2 right;
      } split;
      unsigned4 combined;
      } trick;
    trick.combined = v;
    left = trick.split.left;
    right = trick.split.right;
  }
  inline stereo_sample2 muldiv(signed4 mul, signed4 div)
  {
    return stereo_sample2(((signed4)left)*mul/div,
			  ((signed4)right)*mul/div);
  }
  inline stereo_sample2 muldiv2(signed4 mull, signed4 divl, 
				signed4 mulr, signed4 divr)
  {
    return stereo_sample2(((signed4)left)*mull/divl,
			  ((signed4)right)*mulr/divr);
  }
  inline stereo_sample2 add(signed4 l, signed4 r)
  {
    return stereo_sample2(left+l,right+r);
  }
};
#endif // __loaded__stereo_sample2_h__
