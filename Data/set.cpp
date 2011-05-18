/****
 Hierarchical Data Objects
 Copyright (C) 2005-2011 Werner Van Belle
 http://flow.yellowcouch.org/data/

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
#ifndef __loaded__set_cpp__
#define __loaded__set_cpp__
using namespace std;
#line 1 "set.c++"
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
#endif // __loaded__set_cpp__
