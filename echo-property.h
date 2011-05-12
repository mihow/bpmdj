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

#ifndef ECHO_PROPERTY_H
#define ECHO_PROPERTY_H

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

#endif

