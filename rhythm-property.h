/****
 BpmDj v4.0: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.
****/
#ifndef __loaded__rhythm_property_h__
#define __loaded__rhythm_property_h__
using namespace std;
#line 1 "rhythm-property.h++"
#include "memory.h"
#include "histogram-property.h"

#define rhythm_prop_sx 96
class rhythm_property: public histogram_property<rhythm_prop_sx>
{
 public:
  rhythm_property() : histogram_property<rhythm_prop_sx>() {};
  virtual void init()
    {
      histogram_property<rhythm_prop_sx>::init();
      for(int i=0; i<24;i++)
	bark[i].init();
    }
  void set_scale(int b, float8 s) 
    {
      bark[b].set_scale(s);
    }
  unsigned1 get_energy(int b, int x)
    {
      return bark[b].get_energy(x);
    }
  void set_energy(int x, int y, unsigned1 s) 
    {
      bark[y].set_energy(x,s);
    }
};
#endif // __loaded__rhythm_property_h__
