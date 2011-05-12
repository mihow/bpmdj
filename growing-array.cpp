/****
 BpmDj: Free Dj Tools
 Copyright (C) 1995 Werner Van Belle
 See 'BeatMixing.ps' for more information

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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "common.h"
#include "growing-array.h"

template <class T> GrowingArray<T>::GrowingArray()
{
  init();
}

template <class T> void GrowingArray<T>::init()
{
  allocated = 0;
  count = 0;
  elements = NULL;
  reset();
}

template <class T> void GrowingArray<T>::reset()
{
  if (allocated>0)
    deallocate(elements);
  allocated = 1;
  elements = new T[allocated]();
  count = 0;
}

template <class T> int GrowingArray<T>::add(T e)
{
  if (count == allocated)
    {
      allocated*=2;
      T *new_elements = new T[allocated]();
      for(int i = 0 ; i < count ; i ++)
	new_elements[i]=elements[i];
      delete[] elements;
      elements = new_elements;
      // elements = reallocate(elements,allocated,T);
    }
  elements[count]=e;
  return count++;
};

template <class T> void GrowingArray<T>::swap_del(int i)
{
  elements[i]=elements[--count];
};
