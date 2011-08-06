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
#ifndef __loaded__sample4_type_cpp__
#define __loaded__sample4_type_cpp__
using namespace std;
#include "sample4-type.h"

Data sample4_type::get_data(int versionnr) const
{
  Array<1,signed4> result(2);
  result[0] = left;
  result[1] = right;
  return result;
}

void sample4_type::read(char* l)
{
  left = atoi(l);
  while(*l && *l!=' ') l++;
  while(*l==' ') l++;
  right = atoi(l);
}


void sample4_type::set_data(Data &data)
{
  Array<1,signed4> cast;
  cast = (Array<1,signed4>)data;
  left  = cast[0];
  right = cast[1];
}
#endif // __loaded__sample4_type_cpp__
