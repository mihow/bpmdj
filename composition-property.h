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
#ifndef __loaded__composition_property_h__
#define __loaded__composition_property_h__
using namespace std;
#include "memory.h"
#include "histogram-property.h"

#define composition_prop_sx 32
class composition_property: public histogram_property<composition_prop_sx>
{
 public:
  composition_property() : histogram_property<composition_prop_sx>() {};
  virtual void init()
    {
      histogram_property<composition_prop_sx>::init();
      for(int i = 0 ; i < 24 ; i++)
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
  float4 get_energy_scaled(int b, int x)
    {
      return (get_energy(b,x)-127.0)*bark[b].scale/255.0;
    }
  void set_energy(int x, int y, unsigned1 s) 
    {
      bark[y].set_energy(x,s);
    }
};
#endif // __loaded__composition_property_h__
