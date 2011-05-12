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

#ifndef ARRAY_H
#define ARRAY_H
#include "types.h"
#include "data-visitor.h"
#include "coordinates.h"
#include "array-meta.h"
#include "type-names.h"

//---------------------------------------------------------------
//                     Forwards
//---------------------------------------------------------------
// the [D]imension of the input matrix,
// the [T]ype of the data contained in the matrix,
// the fact whether [P]osition [T]racking should be done,
// the size of the [S]ub[M]atrix and
// do we want and [O]rdered iterator
template <int D, class T, bool PT, int SM, bool O> class ArrayIterator;

//---------------------------------------------------------------
//                     Multidimensional Arrays
//---------------------------------------------------------------
template <int D, class T> class Array: public DataClass
{
  // begin import
 protected:
  virtual QString type_name() const
    {
      return "Array<"
	+ QString::number(D)
	+ ","
	+ ::type_name(T())
	+ ">";
    };
  virtual DataClass* shallow_copy() const 
    {
      return new Array<D,T>(*this);
    };
  virtual void visit(DataVisitor& v)
    {
      if (D>0) v.visit(*this);
    }
  Array(ArrayMeta<D,T> * content_with_correct_refcount)
    {
      content = content_with_correct_refcount;
    }
 private:
  void init(Size<D> size)
    {
      content = new ArrayMeta<D,T>(size);
    }
  void deref()
    {
      if (content->decref())
	delete content;
    }
 public:
  ArrayMeta<D,T> *content;
  // construct, copy, assign, refcount & destruct
  Array()
    {
      init(Size<D>());
    }
  
  Array(Size<D> size) 
    {
      init(size);
    };
  Array(int x) 
    {
      init(Size<D>(x));
    }
  Array(int x, int y) 
    {
      init(Size<D>(x,y));
    };
  Array(int x, int y, int z) 
    {
      init(Size<D>(x,y,z));
    };
  Array(int x, int y, int z, int a) 
    {
      init(Size<D>(x,y,z,a));
    };
  Array(int x, int y, int z, int a, int b, int c) 
    {
      init(Size<D>(x,y,z,a,b,c));
    };
  Array(const Array<D,T> & m) : DataClass()
    {
      content = m.content;
      content->incref();
    }

  /**
   * This constructor accepts a SpecialStorage type (see array-storage). It will increase
   * the reference counter immediatelly with one, effectively taking ownership of the data
   * since it will also delete the data when the counter reaches zero.
   */
  Array(SpecialStorage<T> *store, Size<D> sizes)
    {
      content = new ArrayMeta<D,T>(store,sizes);
    }
      
  /**
   * this constructor will refer to the same data but assume the array has a different dimensionality
   * remaining dimensions will be padded with size 1. O must be smaller than T;
   */
  template <int O> Array(const Array<O,T> &m);

  /**
   * this constructor can be used when O is larger than T. In that case only the selected dimensions
   * will remain.
   */
  template<int O> Array(const Array<O,T>& other, const Select<D>& selected);
  Array<D,T> &operator =(const Array<D,T> &m);
  Array<D,T> &operator =(const T &m);
  Array<D,T> &operator +=(const T &m);
  Array<D,T> &operator +=(const Array<D,T> &m);
  Array<D,T> &operator -=(const T &m);
  Array<D,T> &operator /=(const T &m);
  Array<D,T> &operator *=(const T &m);
  Array<D,T> operator *(const T m) const;
  Array<D,T> operator /(const T m) const;
  Array<D,T> operator +(const Array<D,T> &m) const;
  
  ~Array() 
    { 
      deref(); 
    };

  // range functions
  Array<D,T> operator ()(const From<D> & from) const 
    {
      return Array<D,T>(new ArrayMeta<D,T>(content,from)); 
    };
  Array<D,T> operator ()(const To<D>& to) const
    {
      return Array<D,T>(new ArrayMeta<D,T>(content,to));
    }
  Array<D,T> operator()(const Size<D>& s) const
    {
      return Array<D,T>(new ArrayMeta<D,T>(content,s));
    }
  Array<D,T> operator ()(const From<D> & from, const To<D>& to) const 
    {
      return Array<D,T>(new ArrayMeta<D,T>(content,from,to)); 
    };
  Array<D,T> operator ()(const From<D> & from, const Size<D>& size) const 
    {
      return Array<D,T>(new ArrayMeta<D,T>(content,from,size));
    }
  template <int O> Array<O,T> operator()(const Select<O> keep) const
    {
      return Array<O,T>(*this,keep); 
    };
  
  void setAddress(T* start) 
    {
      content->setAddress(start);
    };
  int size(int i) const 
    {
      return content->size[i]; 
    };
  Size<D> size() const   
    { 
      return content->size; 
    };
  bool empty() const        
    {
      return content->size.has(0); 
    };
  Delta<D> delta() const    
    {
      return content->delta;
    };
  int  delta(int i) const   
    {
      return content->delta[i];
    };
  T*   address(Position<D> i) const 
    {
      return content->address(i); 
    };
  T & operator [](Position<D> i) const 
    {
      return *address(i); 
    };
  bool operator ==(const Array<D,T> &m) const;
  bool operator !=(const Array<D,T> &m) const 
    {
      return !(*this==m);
    };
  void printMetaInfo() const;
  
  /**
   * will copy the data in source to target using the asignment operator
   * Both sizes must be equal and valid
   */
  template <class F> void copyFrom(const Array<D,F> &source);
  
  /**
   * Will set an area of this area to the array given in o.
   * No boundary warings are given, but no ill result should happen either.
   * In other words, the copy area will be limited to the size of the possible
   * target area.
   */
  void copyFrom(const Array<D,T> &source, const From<D>& where);
  
  // handy types
  typedef                                T value;
  typedef                       Array<1,T> vector;
  typedef                       Array<2,T> matrix;
  
  typedef ArrayIterator<D,T,false,0,false> values;
  typedef ArrayIterator<D,T,false,0,true>  ordered;
  typedef ArrayIterator<D,T,true ,0,false> positions;
  typedef ArrayIterator<D,T,false,1,false> vector_values;
  typedef ArrayIterator<D,T,true ,1,false> vector_positions;
  typedef ArrayIterator<D,T,true ,1,true>  ordered_vectors;
  typedef ArrayIterator<D,T,false,2,false> matrix_values;
  typedef ArrayIterator<D,T,true ,2,false> matrix_positions;
};

/**
 * The assignment operator will set the content of the array
 * to the given value using the = operator. This function is optimized in cases 
 * the array can be treated in linear blocks.
 */
template <int D, class T> Array<D,T> &Array<D,T>::operator =(const T &m)
{
  ArrayIterator<D,T,false,0,false> values(*this);
  // big chunk approach ?
  if (values.linear() && values.size()>8)
    {
      if (sizeof(T)==sizeof(char))
	{
	  char C = *(char*)(void*)(&m);
	  for(int s = values.size(); values.more(); values.skipBlock())
	    memset((char*)(T*)values, C, s);
	  return *this;
	}
      if (sizeof(T)==sizeof(wchar_t))
	{
	  wchar_t C = *(wchar_t*)(void*)(&m);
	  for(int s = values.size(); values.more() ; values.skipBlock())
	    wmemset((wchar_t*)(T*)values, C, s);
	  return *this;
	}
    }
  // no big set, simply a fast iterator
  for( ; values.more() ; values++) values=m;
  return *this;
}

template <int D, class T>
void Array<D,T>::copyFrom(const Array<D,T> &source, const From<D> &where)
{
   // limit the area size
   Size<D> copySize(size());
   copySize(where);
   Size<D> source_size(source.size());
   copySize.min(source_size);
   // copy the area
   Array<D,T> target_area((*this)(where,copySize));
   Array<D,T> source_area(source(copySize));
   target_area.copyFrom(source_area);
}

template <int D, class T>
template <class F>
void Array<D,T>::copyFrom(const Array<D,F> &source)
{
   // two ordered iterator that ignores positions
   ArrayIterator<D,T,false,0,true> d(this);
   ArrayIterator<D,F,false,0,true> s(source);
   // TODO: if T == F we don't need the extra cast
   while(d.more() && s.more())
     {
       d=(T)(F&)s;
       d++;
       s++;
     }
}

template <int D, class T> Array<D,T> &Array<D,T>::operator +=(const T &m)
{
  for(values value(*this); value.more() ; value++)
    value+=m;
  return *this;
}

template <int D, class T> Array<D,T> &Array<D,T>::operator +=(const Array<D,T> &m)
{
  for(positions v(*this); v.more() ; ++v)
    v+=m[v];
  return *this;
}

template <int D, class T> Array<D,T> &Array<D,T>::operator -=(const T &m)
{
  for(values pos(*this); pos.more() ; ++pos)
    pos-=m;
  return *this;
}

template <int D, class T> Array<D,T> &Array<D,T>::operator /=(const T &m)
{
  for(values pos(*this); pos.more(); ++pos)
    pos/=m;
  return *this;
}

template <int D, class T> Array<D,T> &Array<D,T>::operator *=(const T &m)
{
  for(values pos(*this); pos.more(); ++pos)
    pos*=m;
  return *this;
}

template <int D, class T> bool Array<D,T>::operator ==(const Array<D,T> &m) const
{
  if (content==m.content) return true;
  if (content->size!=m.content->size) return false;
  for(positions pos(*this); pos.more() ; ++pos)
    if (pos!=m[pos]) return false;
  return true;
}

template <int D, class T> 
template <int O> 
Array<D,T>::Array(const Array<O,T> &m, const Select<D>& selected)
{
  content = new ArrayMeta<D,T>(*m.content,selected);
}

template <int D, class T> 
template <int O> 
Array<D,T>::Array(const Array<O,T> &m)
{
  content = new ArrayMeta<D,T>(*m.content);
}

template <int D, class T> Array<D,T>& Array<D,T>::operator =(const Array<D,T> &m)
{
  m.content->incref();
  deref();
  content = m.content;
  return *this;
}

template <int D, class T> 
void Array<D,T>::printMetaInfo() const
{
  content->printMetaInfo();
};

template <int D, class T>
Array<D,T> Array<D,T>::operator *(const T val) const
{
  Array<D,T> out(size());
  out.copyFrom(*this);
  out*=val;
  return out;
}

template <int D, class T>
Array<D,T> Array<D,T>::operator /(const T val) const
{
  Array<D,T> out(size());
  out.copyFrom(*this);
  out/=val;
  return out;
}

template <int D, class T>
Array<D,T> Array<D,T>::operator +(const Array<D,T>& in2) const
{
  assert(size()==in2.size());
  Array<D,T> out(size());
  out.copyFrom(*this);
  out+=in2;
  return out;
}

#endif
