/****
 BpmDj v4.2: Free Dj Tools
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
#ifndef __loaded__echo_property_h__
#define __loaded__echo_property_h__
using namespace std;
#line 1 "echo-property.h++"
#include "histogram-property.h"

#define echo_prop_sx 96
class echo_property: public histogram_property<echo_prop_sx>
{
 public:
  echo_property(histogram_type**arr) : histogram_property<echo_prop_sx>(arr)
    {
    }
  echo_property() : histogram_property<echo_prop_sx>()
    {
    }
};
#endif // __loaded__echo_property_h__
