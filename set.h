/****
 Borg4 Data Library
 Copyright (C) 2005-2009 Werner Van Belle

 http://werner.yellowcouch.org/Borg4/group__data.html

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
#ifndef __loaded__set_h__
#define __loaded__set_h__
using namespace std;
#line 1 "set.h++"
#include "array.h"
#include "numbers.h"

//===============================================================
//         A Set Interpretation of a 1 dimensional array
//===============================================================
class NoElement{};

/**
 * A set interpretation of an array. The add
 * function will extend the array by doubling its size.
 */
class Set
{
   /**
    * A set is represented as an 1 dimensional array of data
    */
   typedef Array<1,Data> Content;
   
   /**
    * The content of the set (a 1dimensional atrray of data)
    */
   Content content;
   
   /**
    * The position of the enxt_element. This might
    * be outside the current size, in which case a resize
    * must be caleld before placing an element.
    */
   int next_element;
 private:
   
   /**
    * initializes the content with the given array
    */
   void init(Array<1,Data> array)
     {
	content=array;
	next_element=content.size()[0];
     }

   /**
    * Resizes the content to be at least S elements in size
    */
   void ensure_size(int S);
   
   /**
    * Returns the allocated size of the set
    */
   int allocated_size()
     {
	return content.size()[0];
     }
 public:
   
   /**
    * Initializes a set with no elements
    */
   Set() 
     { 
	init(Content());
     };
   
   /**
    * Initialized the set with the given datam, which is casted
    * to an array.
    */
   Set(Data data)
     {
	init(data);
     };
   
   /**
    * The conversion to a data array is done by taking the appropriate view
    * on the
    */
   operator Data() 
     {
	return content(Size<1>(next_element));
     };

   /**
    * Sets the content of the set to the given array
    */
   
   Set & operator=(Array<1,Data> & arr)
     { 
	init(arr);
	return *this;
     }
  
   /**
    * Will add a new data element to the set
    */
   void add(const Data & element);
   
   /**
    * Returns the size of the set
    */
   int size()
     {
	return next_element;
     }
   
   /**
    * Returns the data element at the given position
    */
   Data& operator[](int i)
     {
	return content[i];
     }
   
};
#endif // __loaded__set_h__
