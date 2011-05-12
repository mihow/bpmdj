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
      content = new ArrayMeta<D,T>();
      setSize(size);
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
      content = new ArrayMeta<D,T>();
    }
  Array(Size<D> size) 
    {
      init(size);
    };
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
  
  template<int O> Array(const Array<O,T>& other, const Select<D>& selected);
  Array<D,T> &operator =(const Array<D,T> &m);
  Array<D,T> &operator =(const T &m);
  Array<D,T> &operator +=(const T &m);
  Array<D,T> &operator +=(const Array<D,T> &m);
  Array<D,T> &operator -=(const T &m);
  Array<D,T> &operator /=(const T &m);
  Array<D,T> &operator *=(const T &m);
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
  
  // the sizes of the matrix. Once set this cannot be changed
  void setSize(const Size<D>& s);
  void setAddress(T* start) 
    {
      content->setAddress(start);
    };
  int  size(int i) const 
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
  for( ; values ; values++) values=m;
  return *this;
}

template <int D, class T> Array<D,T> &Array<D,T>::operator +=(const T &m)
{
  for(values value(this); value ; value++)
    value+=m;
  return *this;
}

template <int D, class T> Array<D,T> &Array<D,T>::operator +=(const Array<D,T> &m)
{
  for(values v(*this); v.more() ; ++v)
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

template <int D, class T> Array<D,T>& Array<D,T>::operator =(const Array<D,T> &m)
{
  m.content->incref();
  deref();
  content = m.content;
  return *this;
}

template <int D, class T> void Array<D,T>::setSize(const Size<D> & s)
{
  content->setSize(s);
};

template <int D, class T> void Array<D,T>::printMetaInfo() const
{
  content->printMetaInfo();
};

#endif
