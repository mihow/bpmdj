/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle

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

#ifndef RYTHM_PROPERTY_H
#define RYTHM_PROPERTY_H

#include "memory.h"
#include "histogram-property.h"

#define rythm_prop_sx 96
class rythm_property: public histogram_property<rythm_prop_sx>
{
 public:
  rythm_property() : histogram_property<rythm_prop_sx>() {};
  virtual void init()
    {
      histogram_property<rythm_prop_sx>::init();
      for(int i=0; i<24;i++)
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
  void set_energy(int x, int y, unsigned1 s) 
    {
      bark[y].set_energy(x,s);
    }
};

#endif
