#ifndef __loaded__array_creator_h__
#define __loaded__array_creator_h__
using namespace std;
#line 1 "array-creator.h++"
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

#ifndef ARRAY_CREATOR_H
#define ARRAY_CREATOR_H
#include "array.h"
#include "numbers.h"

//===============================================================
//                    Array Creation 
//===============================================================
class NoElement{};

// this class keeps the next index in the first index
// the actual size can be extended through 
class Creator
{
  typedef Array<1,Data> Content;
  Content content;
  void init();
  void ensure_size(int S);
 private:
  template<int D, class T> Data createAndFillTarget(Size<8> s);
    void updateSize(Size<8>& size, int last_depth, int depth = 0 );
 public:
  Creator() : content(1) { content[0]=Unsigned4(1); };
  Creator(Data data) : content(Array<1,Data>(data)) {};
  operator Data() {return content;};
  Data convertTo(Unsigned4 dimension, Symbol type);
  void append(Data& e);
};

#endif
#endif // __loaded__array_creator_h__
