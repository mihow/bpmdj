/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

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
#ifndef __BPMDJ___FRAGMENT_H__
#define __BPMDJ___FRAGMENT_H__
using namespace std;
#line 1 "fragment.h++"
#include "stereo-sample2.h"
#include "memory.h"
#include "reference-count.h"

class FragmentObj: public ReferenceCount
{
public:
  stereo_sample2* samples;
  unsigned4 size;
  FragmentObj(QString filename);
  ~FragmentObj()
  {
    bpmdj_deallocate(samples);
  }
};

typedef Smart<FragmentObj> Fragment;
inline Fragment newFragment(QString filename)
{
  return Smart<FragmentObj>(new FragmentObj(filename));
}
#endif
