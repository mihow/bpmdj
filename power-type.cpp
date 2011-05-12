/****
 BpmDj v3.6: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

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
#ifndef __loaded__power_type_cpp__
#define __loaded__power_type_cpp__
using namespace std;
#line 1 "power-type.c++"
#include <stdio.h>
#include <stdlib.h>
#include "power-type.h"

Data power_type::get_data(int version) const
{
  Array<1,float4> result(2);
  result[0]=left;
  result[1]=right;
  return result;
}

void power_type::set_data(Data data) 
{
  Array<1,float4> cast = data;
  left = cast[0];
  right = cast[1];
}

void power_type::read(char* l)
{
  left = atof(l);
  while(*l && *l!=' ') l++;
  while(*l==' ') l++;
  right = atof(l);
}
#endif // __loaded__power_type_cpp__
