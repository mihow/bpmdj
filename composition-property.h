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
#ifndef __loaded__composition_property_h__
#define __loaded__composition_property_h__
using namespace std;
#line 1 "composition-property.h++"
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
  void set_scale(int b, double s) 
    {
      bark[b].set_scale(s);
    }
  unsigned1 get_energy(int b, int x)
    {
      return bark[b].get_energy(x);
    }
  float get_energy_scaled(int b, int x)
    {
      return (get_energy(b,x)-127.0)*bark[b].scale/255.0;
    }
  void set_energy(int x, int y, unsigned1 s) 
    {
      bark[y].set_energy(x,s);
    }
};
#endif // __loaded__composition_property_h__
