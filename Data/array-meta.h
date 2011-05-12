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
  ArrayMeta(ArrayMeta<D,T> *other, const Size<D> &sized_to);
  ArrayMeta(ArrayMeta<D,T> *other, const To<D>   &to);
  ArrayMeta(ArrayMeta<D,T> *other, const From<D> & from, const Size<D> &sized_to);
  ArrayMeta(ArrayMeta<D,T> *other, const From<D> & from, const To<D> &to);
  ArrayMeta(const ArrayMeta&) {assert(0);};
  template <int O> ArrayMeta(const ArrayMeta<O,T>& o, const Select<D> & selected);
  ArrayMeta &operator = (const ArrayMeta&) 
    {
      assert(0); 
      return *this;
    };
  ~ArrayMeta() {};
  void deref() {if (--storage->refcount==0) delete storage;};
  void incref() {refcount++; storage->refcount++;}
  bool decref() {deref(); return --refcount==0;}
  void replaceAccess(ArrayMeta<D,T> *other);
  void setSize(const Size<D> & s);
  void setAddress(T * start) {offset = start;};
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
  for(int i = 0 ; i < D ; i ++)
    {
      int d = selected[i];
      delta[i] = o.delta[d];
      size[i]  = o.size[d];
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
