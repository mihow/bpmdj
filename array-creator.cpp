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
#ifndef __loaded__array_creator_cpp__
#define __loaded__array_creator_cpp__
using namespace std;
#line 1 "array-creator.c++"
#include "array-creator.h"
#include "array-iterator.h"
#include "symbol.h"
#include "numbers.h"
#include "data-io.h"

//---------------------------------------------------------------
//               The Array Creator
//---------------------------------------------------------------
// this class is used by the lexer to read arrays in memory
// it is not the most efficient class since it relies on conversion
// to solve this the parser must be changed to fill in the specific
// positions during parsing.
void Creator::ensure_size(int S)
{
  if (S >= content.size(0))
    {
      Unsigned4 next_index = content[0];
      int prev_allocation = content.size(0);
      int new_allocation = prev_allocation*2;
      while(S<new_allocation) S*=2;
      assert(new_allocation);
      Array<1,Data> before = content;
      content = Array<1,Data>(new_allocation);
      for(unsigned int i = 0 ; i < next_index ; i ++)
	content[i]=before[i];
    }
};

void Creator::append(Data& val)
{
  Unsigned4 next_index=(Unsigned4)content[0];
  ensure_size(next_index+1);
  content[(int)next_index]=val;
  next_index = next_index+1;
  content[0]=next_index;
}

template<int D, class T>
Data Creator::createAndFillTarget(Size<8> s)
{
   // allocate array of correct size
   Size<D> correct_size;
   correct_size.takeFrom(s);
   typedef Array<D,T> Result;
   Result result(correct_size);
   // iterate through the entire array
   typename Result::positions position(result);
   for(;position.more(); ++position)
     {
	Content cur = getContentData();
	/**
	 * Gere we navigate down to the right dimension. 
	 * Because the first element is ourselves we need to add one to the
	 * coordinate for all the sub-elements we can be confident that the
	 * content is a proper matrix already
	 */
	int idx;
	for(int d = 0 ; d < D-1 ; d++)
	  {
	     idx=position[d];
	     cur=(Content)cur[idx];
	  }
	Data data = cur[position[D-1]];
	::convertTo(data,*position.current());
     }
   return result;
}

Array<1,Data> Creator::getContentData()
{
   Unsigned4 next_index=(Unsigned4)content[0];
   return content(From<1>(1),Size<1>(next_index-1));
}

void updateSize(Array<1,Data> content, Size<8>& size, int last_depth, int depth)
{
   // get the sizes of all the children
   int s = content.size()[0];
   if (s > size[depth])
     size[depth]=s;
   if (depth!=last_depth)
     {
	Array<1,Data>::values element(content);
	int i = 0;
	for ( ; element.more();  ++element)
	  {
	     ++i;
	     Array<1,Data> e=*element;
	     updateSize(e,size,last_depth,1+depth);
	  }
     }
}

void Creator::updateSize(Size<8>& size, int last_depth, int depth)
{
   ::updateSize(getContentData(),size,last_depth,depth);
}

Data Creator::convertTo(Unsigned4 dimension, Symbol type)
{
  // get the appropriate size
  assert(dimension);
  Size<8> size;
  updateSize(size,dimension-1);
  // instantiate the right type
#define ARRAY_TYPE(D,T) \
if (D==dimension && QString(type) == QString(#T)) \
  return createAndFillTarget<D,T>(size);
  ARRAY_TYPES
#undef ARRAY_TYPE
  assert(0);
  return Data();
}
#endif // __loaded__array_creator_cpp__
