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

#ifndef STEREO_SAMPLE2
#define STEREO_SAMPLE2
#include "Data/om-data.h"

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
  unsigned4 & value()
    {
      return *((unsigned4*)((void*)&left));
    }
  void value(unsigned4 v)
    {
      *(unsigned4*)(&left) = v;
    }
  inline stereo_sample2 muldiv(signed4 mul, signed4 div)
    {
      return stereo_sample2(((signed4)left)*mul/div,
			    ((signed4)right)*mul/div);
    }
  inline stereo_sample2 muldiv2(signed4 mull, signed4 divl, signed4 mulr, signed4 divr)
    {
      return stereo_sample2(((signed4)left)*mull/divl,
			    ((signed4)right)*mulr/divr);
    }
  inline stereo_sample2 add(signed4 l, signed4 r)
    {
      return stereo_sample2(left+l,right+r);
    }
};
#endif
