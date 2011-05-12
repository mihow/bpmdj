/****
 Borg4 Data Library
 Copyright (C) 2005-2010 Werner Van Belle

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
#ifndef __loaded__array_iterator_h__
#define __loaded__array_iterator_h__
using namespace std;
#line 1 "array-iterator.h++"
/**
 * @ingroup data
 * @brief backend multidimensional iterator
 * 
 * The array iterator is a very fast but flexible iterator it is used by
 * combining different iteration walks into one iterator (extend). This is
 * then assigned in the ArrayIterator. After trying to use this a couple of 
 * times it seems that it is somewhat too complex. We might want to rethink 
 * this one.
 */
template <class T, bool PT> class ArrayIteratorBacking
{
 public:
  int idx;
  int delta;
  int next;
  T* first;
  ArrayIteratorBacking<T,PT>  * _more;
  int * pos;
  // will fill this iterator with the right information
  ArrayIteratorBacking(T* _start, int _delta, int _s, int* target);
  ArrayIteratorBacking() : idx(0), delta(0), next(0), _more(NULL), pos(NULL) 
    {
    };
  operator T&() const          
    {
      return first[idx]; 
    };
  T& operator*() const         
    {
      return first[idx]; 
    };
  T* current() const           
    {
      return first+idx; 
    };
  T& operator[](int) const  
    {
      assert(0); 
    };
  operator T*() const         
    {
      return first; 
    };
  bool more()                  
    { 
      return idx<next || needmore();
    }
  bool needmore();
  void operator ++()
    {
      inc();
    };
  void operator ++(int)
    {
      inc();
    };
  void skipBlock()
    {
      idx = next;
      if(PT) (*pos)=-1;
    };
  bool linear() const
    {
      return delta == 1;
    };
  int  size() const
    {
      return next/delta;
    };
  void print() const;
  T&   operator=(const T& v)
    {
      first[idx]=v;
      return first[idx];
    };
  template <bool PT2> 
    void init(const ArrayIteratorBacking<T,PT2> & b)
    {
      init(b.current());
      if (_more) _more->init(b);
    }
 private:
  void init(T* start)
    {
      idx = 0;
      first=start;
      if (PT) (*pos) = 0;
    }
  void inc()                   
    { 
      idx += delta; 
      if(PT) (*pos)++;
    };
};

/**
 * @ingroup data
 * @brief fast multidimensional iterator
 * 
 * The ArrayIterator keeps track of the position within a matrix of dimension D
 * The submatrix it returns is of dimension SM
 * position tracking can be enabled with PT, and ordered mode can be
 * selected with ORDERED. The type must be the same for all
 */
template <int D, class T, bool PT, int SM, bool ORDERED>
class ArrayIterator: public ArrayIteratorBacking<T,PT>
{ 
 private:
  Array<SM,T> submatrix;
  void setup(const Array<D,T>& m, const Select<D-SM> sel, T* start);
 public:
  Position<D> position;
  Array<D,T> matrix;
  /**
   * The empty iterator creates its own empty matrix
   */
  ArrayIterator<D,T,PT,SM,ORDERED>()
    {
    }

  /**
   * When submitting an array, we will walk through
   * every position, so the submatrix has actual dimension 0
   */
  ArrayIterator<D,T,PT,SM,ORDERED>(const Array<D,T> &m)
    {
      reset(m);
    }
  ArrayIterator<D,T,PT,SM,ORDERED>(Array<D,T> *m)
    { 
      reset(*m); 
    }
  void reset(const Array<D,T> &m)
    {
      assert(SM==0);
      setup(m, Select<D>(), NULL); 
    }
  void reset()
    {
      reset(matrix);
    }
  ArrayIterator<D,T,PT,SM,ORDERED>(const Array<D,T> &m, const Select<D-SM> sel)
    {
      setup(m, sel, NULL);
    }
  ArrayIterator<D,T,PT,SM,ORDERED>(const Array<D,T> &m, int d)
    { 
      setup(m, Select<1>(d), NULL);
    };
  ArrayIterator<D,T,PT,SM,ORDERED>(const Array<D,T> &m, int x, int y)
    {
      setup(m, Select<2>(x,y), NULL);
    };
  int operator[](int i) const  
    {
      assert(PT); 
      return position[i]; 
    };
  operator Position<D>() const 
    {
      assert(PT); 
      return position; 
    };
  operator Array<SM,T>&() 
    {
      return subArray(); 
    };
  // wrappers around various operators
  T&   operator =  (const T& v) 
    {
      return ArrayIteratorBacking<T,PT>::operator=(v); 
    };
  T&   operator += (const T& v) 
    {
      return ((T&)(*this)) += v; 
    };
  T&   operator -= (const T& v) 
    { 
      return ((T&)(*this)) -= v; 
    };
  T&   operator /= (const T& v) 
    {
      return ((T&)(*this)) /= v; 
    };
  T&   operator *= (const T& v) 
    { 
      return ((T&)(*this)) *= v; 
    };
  bool operator == (const T& v) 
    { 
      return ((T&)(*this)) == v; 
    };
  bool operator != (const T& v) 
    { 
      return ((T&)(*this)) != v; 
    };
 private:
  Array<SM,T>& subArray()
    {
      assert(SM);
      submatrix . setAddress(ArrayIteratorBacking<T,PT>::current());
      return submatrix; 
    };
};

template<class T, bool PT>
bool ArrayIteratorBacking<T,PT>::needmore()
{
  if (_more)
    {
      _more->inc();
      if (_more->more())
	{
	  init(_more->current());
	  return true;
	}
    }
  return false;
};

template <class T, bool PT>
ArrayIteratorBacking<T,PT>::ArrayIteratorBacking(T* _start, int _delta, 
						 int _s, int* target) : 
  idx(0), delta(_delta), next(_s*_delta), first(_start), 
  _more(NULL), pos(target)
{
  if (PT)
    {
      assert(pos);
      (*pos)=0;
    }
};

//---------------------------------------------------------------
//                   The Array Iterators
//---------------------------------------------------------------
template <int D, class T, bool PT, int SM, bool ORDERED>
void ArrayIterator<D,T,PT,SM,ORDERED>::setup(const Array<D,T>& m, 
					     const Select<D-SM> selected, 
					     T* start)
{
  matrix = m;
  const int SEL = D - SM;
  // is the matrix empty ?
  if(matrix.empty()) 
    {
      ArrayIteratorBacking<T,PT>::idx = 0;
      ArrayIteratorBacking<T,PT>::first = NULL;
      ArrayIteratorBacking<T,PT>::delta = 0;
      ArrayIteratorBacking<T,PT>::next = 0;
      ArrayIteratorBacking<T,PT>::_more = NULL;
      return;
    }
  // select the dimensions
  Delta<SEL>    deltas;
  Size<SEL>     sizes;
  if (SEL==D)
    {
      deltas.set(matrix.delta());
      sizes.set(matrix.size());
    }
  else
    {
      deltas.set(matrix.delta().select(selected));
      sizes.set(matrix.size().select(selected));
    }
  Position<SEL> addr;
  if (PT)
    for(int i = 0 ; i < SEL ; i++) addr[i]=i;
  
  if (!ORDERED)
    // sort them 
    for(int d = SEL - 1 ; d >= 1 ; d--)
      {
	int mi = d, mv = deltas[mi];
	for(int i = mi - 1 ; i>=0 ; i--)
	  if (deltas[i]<mv)
	    mv=deltas[mi=i];
	if (mi!=d)
	  {
	    int t = deltas[mi];
	    int u = sizes[mi];
	    deltas[mi]=deltas[d];
	    sizes[mi]=sizes[d];
	    deltas[d]=t;
	    sizes[d]=u;
	    if (PT)
	      {
		int v = addr[mi];
		addr[mi]=addr[d];
		addr[d]=v;
	      }
	  }
      }
  
  /** 
   * Now we group them together as good as possible
   * This is only possible when we don't do position
   * tracking. In that case we can go from left to 
   * right and combine the field together if possible
   * the fields that become empty are filled with -1
   * At the last moment we can resort them to ensure
   * an optimal iteration, with the most long inner
   * loop.
   */
  if (!PT)
    {
      bool combined = false;
      for(int dim = 0 ; dim < SEL - 1 ; dim++)
	{
	  // can we combine them ?
	  const int into = dim + 1;
	  if (deltas[dim] == deltas[into]*sizes[into])
	    {
	      // yes, we combine them, which means
	      // that the delta remains the lower value
	      // while the size increases
	      sizes[into]*=sizes[dim];
	      deltas[dim]=0;
	      sizes[dim]=0;
	      combined=true;
	    }
	}
      // if we combined multiple indices
      // it might pay of to resort them
      // this time however we sort on the number
      // of steps that will be taken by that iterator
      if (!ORDERED)
	if (combined)
	  for(int d = SEL - 1 ; d >= 1 ; d--)
	    {
	      int ma = d, mv = sizes[ma];
	      for(int i = ma - 1 ; i>=0 ; i--)
		if (sizes[i]>mv)
		  mv=sizes[ma=i];
	      if (ma!=d)
		{
		  int t = deltas[ma];
		  int u = sizes[ma];
		  deltas[ma]=deltas[d];
		  sizes[ma]=sizes[d];
		  deltas[d]=t;
		  sizes[d]=u;
		}
	    }
    }

  /** 
   * The next step creates the proper iterator tail
   * and attaches it to the current iterator
   * which is set in accordance with the last element
   */
  if (start==NULL) start = matrix.address(0);
  ArrayIteratorBacking<T,PT> * next_iterator = NULL;
  for(int dim = 0; dim < SEL - 1 ; dim++)
    {
      if (!sizes[dim]) continue;
      int * posfield = NULL;
      if (PT) posfield = &(position[selected[addr[dim]]]);
      ArrayIteratorBacking<T,PT> * new_iterator;
      new_iterator = new ArrayIteratorBacking<T,PT>(start, deltas[dim], 
						    sizes[dim], posfield);
      new_iterator->_more = next_iterator;
      next_iterator = new_iterator;
    }
  // the last index goes into this
  ArrayIteratorBacking<T,PT>::idx = 0;
  ArrayIteratorBacking<T,PT>::first = start;
  ArrayIteratorBacking<T,PT>::delta = deltas[SEL-1];
  ArrayIteratorBacking<T,PT>::next  = deltas[SEL-1]*sizes[SEL-1];
  if (PT) ArrayIteratorBacking<T,PT>::pos = &(position[selected[addr[SEL-1]]]);
  ArrayIteratorBacking<T,PT>::_more = next_iterator;

  /** 
   * finally we initialize the submatrix by figuring
   * out which dimensions remain variable
   */
  if (SM)
    {
      Select<SM> remaining;
      int j = 0;
      for(int i = 0 ; i < D ; i ++)
	if (!selected.has(i)) remaining[j++]=i;
      assert(j==SM);
      submatrix = matrix(remaining);
    }
}

template<class T, bool PT>
void ArrayIteratorBacking<T,PT>::print() const
{
  if (PT)
    {
      assert(pos);
      printf("Idx = %d; Delta = %d; Size = %d; Next = %d Pos = %d\n",
	     idx,delta,next/delta,next,*pos);
    }
  else
    printf("Idx = %d; Delta = %d; Size = %d; Next = %d\n",
	   idx,delta,next/delta,next);
  if (_more) _more->print();
  else printf("-------------------\n");
}
#endif // __loaded__array_iterator_h__
