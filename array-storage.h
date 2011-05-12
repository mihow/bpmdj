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
#ifndef __loaded__array_storage_h__
#define __loaded__array_storage_h__
using namespace std;
#line 1 "array-storage.h++"
//---------------------------------------------------------------
//                          Array storage
//---------------------------------------------------------------
/**
 * @ingroup data
 */
template <class T> class ArrayStorage
{
 public:
  int  refcount;            // the number of times this data chunk is accessed
  T   *data;                // the actual data
  ArrayStorage()
    {
      refcount = 1;
      data = NULL;
    }
  virtual ~ArrayStorage()
    {
    }
};

/**
 * @ingroup data
 */
template <class T> class NormalArrayStorage: public ArrayStorage<T>
{
 public:
  virtual ~NormalArrayStorage()
    {
      delete[] ArrayStorage<T>::data;
      ArrayStorage<T>::data = NULL;
    }
  NormalArrayStorage<T>(int size)
    {
      ArrayStorage<T>::data = new T[size];
    }
};

/**
 * @ingroup data
 */
template <class T> class SpecialStorage: public ArrayStorage<T>
{
};
#endif // __loaded__array_storage_h__
