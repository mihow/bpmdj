/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

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
