/****
 Om-Data
 Copyright (C) 2005-2006 Werner Van Belle

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

#include "set.h"
#include "array-iterator.h"
#include "symbol.h"
#include "numbers.h"
#include "data-io.h"

//---------------------------------------------------------------
//             The set interpretation of a 1D array
//---------------------------------------------------------------
void Set::ensure_size(int s)
{
   if (s<=allocated_size()) return;
   int newsize = allocated_size();
   if (newsize==0) newsize=8;
   while(s>newsize) newsize*=2;
   Content newcontent(newsize);
   newcontent.copyFrom(content,0);
   content = newcontent;
};

void Set::add(const Data& val)
{
   ensure_size(next_element+1);
   content[next_element]=val;
   next_element++;
}
