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

#ifndef ARRAY_STORAGE_H
#define ARRAY_STORAGE_H
//---------------------------------------------------------------
//                          Array storage
//---------------------------------------------------------------
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
  ~ArrayStorage()
    {
      delete[] data;
      data = NULL;
    }
  void allocate(int size)
    {
      assert(!data);
      data = new T[size];
      //  printf("Allocated %d bytes\n",sizeof(T)*size);
    }
};
#endif

