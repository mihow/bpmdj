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

#ifndef ARRAY_META_H
#define ARRAY_META_H
#include "array-storage.h"
#include "coordinates.h"

//---------------------------------------------------------------
//                     Array Bounds / Access
//---------------------------------------------------------------
template <int D, class T> class ArrayMeta
{
  int refcount;                   // reference count to this access module
public:
  ArrayStorage<T> * storage;     // data storage
  T * offset;                     // the start of the array
  Size<D> size;                   // size of this access area
  Delta<D> delta;                 // the number of elements to move along this dimension

  ArrayMeta();
  ArrayMeta(ArrayMeta<D,T> *other, const From<D> &from);
  /**
   * this function will return a meta description based on other but with a reduced size
   */
  ArrayMeta(ArrayMeta<D,T> *other, const Size<D> &sized_to);
  ArrayMeta(ArrayMeta<D,T> *other, const To<D>   &to);
  ArrayMeta(ArrayMeta<D,T> *other, const From<D> & from, const Size<D> &sized_to);
  ArrayMeta(ArrayMeta<D,T> *other, const From<D> & from, const To<D> &to);
  ArrayMeta(const ArrayMeta&) 
    {
      assert(0);
    };
  /**
   * this function will return a meta description based on other
   * but with a new larger dimensionality
   */
  template <int O> ArrayMeta(const ArrayMeta<O,T>& o);
  /**
   * this function returns a meta description based on o
   * but with only the selected dimensions remaining
   */
  template <int O> ArrayMeta(const ArrayMeta<O,T>& o, const Select<D> & selected);
  ArrayMeta &operator = (const ArrayMeta&) 
    {
      assert(0);
      return *this;
    };
  ~ArrayMeta() 
    {
      deref();
    };
  void deref() 
    {
      if (--storage->refcount==0) 
	delete storage;
    };
  void incref() 
    {
      refcount++; 
    }
  bool decref() 
    {
      return --refcount==0;
    }
  void replaceAccess(ArrayMeta<D,T> *other);
  void setSize(const Size<D> & s);
  void setAddress(T * start) 
    {
      offset = start;
    };
  T *  address(const Position<D> & c);
  T *  address(const From<D> & c);
  void printMetaInfo();
};

template <int D, class T> void ArrayMeta<D,T>::replaceAccess(ArrayMeta<D,T> *other)
{
  other->storage->refcount++;
  deref();
  storage=other->storage;
  offset = other->offset;
  size = other->size;
  delta = other->delta;
}

template <int D, class T> void ArrayMeta<D,T>::printMetaInfo()
{
  printf(  " Size: "); size.print(); 
  printf("\n Delta: "); delta.print();
  printf("\n Refcount: %d", storage->refcount);
  printf("\n Offset: %d\n",offset-storage->data);
}

template <int D, class T> T * ArrayMeta<D,T>::address(const Position<D> & c)
{
  T* result = offset;
  iterate_dimensions(d,D,result+=c[d]*delta[d]);
  return result;
}

template <int D, class T> T * ArrayMeta<D,T>::address(const From<D> & c)
{
  T* result = offset;
  iterate_dimensions(d,D,result+=c[d]*delta[d]);
  return result;
}

template <int D, class T> 
ArrayMeta<D,T>::ArrayMeta()
{
  refcount = 1;
  storage = new ArrayStorage<T>();
  offset = storage->data;
}

template <int D, class T> ArrayMeta<D,T>::ArrayMeta(ArrayMeta<D,T> * other, const From<D> &from)
{
  refcount = 1;
  other->storage->refcount++;
  storage=other->storage;
  offset = other->offset;
  size = other->size;
  delta = other->delta;
  offset=address(from);
  size-=from;
}

template <int D, class T> 
ArrayMeta<D,T>::ArrayMeta(ArrayMeta<D,T> * other, const Size<D> &to)
{
  refcount = 1;
  other->storage->refcount++;
  storage = other->storage;
  offset = other->offset;
  delta = other->delta;
  size.set(to);
}

template <int D, class T> 
ArrayMeta<D,T>::ArrayMeta(ArrayMeta<D,T> * other, const To<D> &to)
{
  refcount = 1;
  other->storage->refcount++;
  storage = other->storage;
  offset = other->offset;
  delta = other->delta;
  size.set(to);
  size += 1;
}

template <int D, class T>
ArrayMeta<D,T>::ArrayMeta(ArrayMeta<D,T> * other, const From<D> &from, const Size<D> &sized)
{
  refcount = 1;
  other->storage->refcount++;
  storage = other->storage;
  offset = other->offset;
  size = sized;
  delta = other->delta;
  offset = address(from);
}

template <int D, class T> 
ArrayMeta<D,T>::ArrayMeta(ArrayMeta<D,T> * other, const From<D> &from, const To<D> &to)
{
  refcount = 1;
  other->storage->refcount++;
  storage = other->storage;
  offset = other->offset;
  size.set(to);
  size -= from;
  size += 1;
  delta = other->delta;
  offset = address(from);
}

template <int D, class T>
template <int O>
ArrayMeta<D,T>::ArrayMeta(const ArrayMeta<O,T>& o, const Select<D> & selected)
{
  refcount = 1;
  o.storage->refcount++;
  storage = o.storage;
  offset = o.offset;
  assert(D<=O);
  for(int i = 0 ; i < D ; i ++)
    {
      int d = selected[i];
      assert(d<O);
      delta[i] = o.delta[d];
      size[i]  = o.size[d];
    }
}

template <int D, class T>
template <int O>
ArrayMeta<D,T>::ArrayMeta(const ArrayMeta<O,T>& o)
{
  assert(O<D);
  refcount = 1;
  o.storage->refcount++;
  storage = o.storage;
  offset = o.offset;
  int stride=1;
  for(int i = 0 ; i < O ; i ++)
    {
      delta[i+D-O] = o.delta[i];
      size[i+D-O]  = o.size[i];
      stride*=o.delta[i];
    }
  for(int i = 0 ; i < D-O ; i++)
    {
      size[i]=1;
      delta[i]=stride;
    }
}

template <int D, class T> void ArrayMeta<D,T>::setSize(const Size<D> & s)
{
  long int totsize = 1;
  size = s;
  for(int i = D-1 ; i >=0 ; i --)
    {
      delta[i]=totsize;
      totsize*=s[i];
    };
  storage->allocate(totsize);
  offset=storage->data;
}

#endif
