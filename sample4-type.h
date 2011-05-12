/****
 BpmDj v3.6: Free Dj Tools
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
#ifndef __loaded__sample4_type_h__
#define __loaded__sample4_type_h__
using namespace std;
#line 1 "sample4-type.h++"
#include <stdlib.h>
#include <stdio.h>
#include "om-data.h"

class sample4_type
{
 public:
  static const signed4 no_sample=0xffff+1; 
  // one more than the possible maximum of a signed short
  signed4 left;
  signed4 right;
  sample4_type()
    {
      left = no_sample;
      right = no_sample;
    }
  sample4_type(signed4 l, signed4 r)
    {
      left = l;
      right = r;
    }
  bool fully_defined() 
    {
      return left!=no_sample && right!=no_sample;
    };
  bool badly_defined()
    {
      return !fully_defined();
    }
  Data get_data(int versionnr) const; 
  void set_data(Data & data);
  void read(char*);
};
#endif // __loaded__sample4_type_h__
