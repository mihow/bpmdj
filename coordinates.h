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
#ifndef __loaded__coordinates_h__
#define __loaded__coordinates_h__
using namespace std;
#line 1 "coordinates.h++"
#include <cstdio>
//---------------------------------------------------------------
//                          Forwards
//---------------------------------------------------------------
template <int D> class Select;
template <int D> class Size;
template <int D> class From;
template <int D> class To;

//---------------------------------------------------------------
//                          Coordinates
//---------------------------------------------------------------
#define iterate_dimensions(VAR,LIMIT,BLOCK) \
for(int VAR = LIMIT -1 ; VAR>=0 ; VAR--) {BLOCK;}

#ifdef WINDOWS
/**
 * The mingw gcc compiler seems not want to instantiate an array of 0 elements
 * so we add an extra element which will never be accessed
 */
# define NO_EMPTY_ARRAYS
#endif

template <int D> class _Coordinate
{
 protected:
#ifdef NO_EMPTY_ARRAYS
   int coordinate[D+1];
#else
  int coordinate[D];
#endif
 public:
  _Coordinate() {for(int d = 0 ; d < D; d++) coordinate[d]=-1;};
  _Coordinate(int x)
    {
      iterate_dimensions(d,D,coordinate[d]=x);
    };
  _Coordinate(int x, int y) 
    { 
      assert(D==2);
      coordinate[0]=x;
      coordinate[1]=y;
    };
  _Coordinate(int x, int y, int z) 
    { 
      assert(D==3);
      coordinate[0]=x;
      coordinate[1]=y;
      coordinate[2]=z;
    };
  _Coordinate(int x, int y, int z, int a) 
    { 
      assert(D==4);
      coordinate[0]=x;
      coordinate[1]=y;
      coordinate[2]=z;
      coordinate[3]=a;
    };
  _Coordinate(int x, int y, int z, int a, int b) 
    { 
      assert(D==5);
      coordinate[0]=x;
      coordinate[1]=y;
      coordinate[2]=z;
      coordinate[3]=a;
      coordinate[4]=b;
    };
  _Coordinate(int x, int y, int z, int a, int b, int c) 
    { 
      assert(D==6);
      coordinate[0]=x;
      coordinate[1]=y;
      coordinate[2]=z;
      coordinate[3]=a;
      coordinate[4]=b;
      coordinate[5]=c;
    };
  bool has(int val) const
    {
      iterate_dimensions(i,D,
        if (coordinate[i]==val) return true;);
      return false;
    }
  void operator =(int val) 
    {
      iterate_dimensions(d,D,coordinate[d]=val);
    };
  void operator =(const _Coordinate<D>& val) 
    {
      iterate_dimensions(d,D,coordinate[d]=val.coordinate[d]);
    };
  _Coordinate<D> operator +(const _Coordinate<D>& other) const
    {
      _Coordinate<D> result; iterate_dimensions(d,D,
        result.coordinate[d]=coordinate[d]+other.coordinate[d]);
      return result;
    };
  _Coordinate<D>& operator +=(const _Coordinate<D>& other)
    {
      iterate_dimensions(d,D,coordinate[d]+=other.coordinate[d]);
      return *this;
    };
  _Coordinate<D> operator -(const _Coordinate<D>& other) const
    {
      _Coordinate<D> result; iterate_dimensions(d,D,
       result.coordinate[d]=coordinate[d]-other.coordinate[d]);
      return result;
    };
  _Coordinate<D>& operator -=(const _Coordinate<D>& other)
    {
      iterate_dimensions(d,D,coordinate[d]-=other.coordinate[d]);
      return *this;
    };
  bool operator ==(const _Coordinate<D>& other) const
    {
      iterate_dimensions(d,D,
        if (coordinate[d]!=other.coordinate[d]) return false);
      return true;
    };
  bool operator !=(const _Coordinate<D>& other) const
    {
      iterate_dimensions(d,D,
        if (coordinate[d]!=other.coordinate[d]) return true);
      return false;
    };
  int& operator[](int idx)       
    {
      return coordinate[idx];
    };
  int  operator[](int idx) const 
    {
      return coordinate[idx];
    };
  void print()
    {
      for(int i = 0 ; i < D ; i ++)
	printf("%d ",coordinate[i]);
    }
  template <int SEL> _Coordinate<SEL> select(const Select<SEL>& sel);
  void set(const _Coordinate<D>& s) 
    {
      _Coordinate<D>::operator=(s);
    };
  template <int BROL>
    void set(const _Coordinate<BROL>&) 
    {
      assert(0);
    };
};

template <int D>
template <int SEL> 
_Coordinate<SEL> _Coordinate<D>::select(const Select<SEL>& sel)
{
  _Coordinate<SEL> result;
  for(int i = 0 ; i < SEL ; i ++)
    result[i]=coordinate[sel[i]];
  return result;
}

template <int D> class Select:   public _Coordinate<D> 
{
 public:
  Select() : _Coordinate<D>() 
  {
    for(int i = 0 ; i < D ; i++) _Coordinate<D>::coordinate[i]=i;
  };
  Select(int x) : _Coordinate<D>(x) {assert(D==1);};
  Select(int x, int y) : _Coordinate<D>(x,y) {assert(D==2);};
  Select(int x, int y, int z) : _Coordinate<D>(x,y,z) {assert(D==3);};
  Select(int x, int y, int z, int a) : _Coordinate<D>(x,y,z,a) {assert(D==4);};
};

template <int D> class Delta:    public _Coordinate<D> 
{
 public:
  Delta() : _Coordinate<D>(0) {};
};

template <int D> class To:       public _Coordinate<D>
{
 public:
  To(int x) : _Coordinate<D>(x) {};
  To(int x, int y) : _Coordinate<D>(x,y) {};
  To(int x, int y, int z) : _Coordinate<D>(x,y,z) {};
  To(int x, int y, int z, int a) : _Coordinate<D>(x,y,z,a) {};
};

template <int D> class Position: public _Coordinate<D> 
{
 public:
  Position() : _Coordinate<D>(0) {};
  Position(int x) : _Coordinate<D>(x) {};
  Position(int x, int y) : _Coordinate<D>(x,y) {};
  Position(int x, int y, int z) : _Coordinate<D>(x,y,z) {};
  Position(int x, int y, int z, int a) : _Coordinate<D>(x,y,z,a) {};
  Position<D> operator /(const int div) const
    {
      Position<D> result; iterate_dimensions(d,D,
	result.coordinate[d]=_Coordinate<D>::coordinate[d]/div);
      return result;
    };
};

template <class T>
T min(T a, T b)
{
   if (a<b) return a;
   return b;
}

template <int D> class Size: public _Coordinate<D> 
{
 public:
  Size() : _Coordinate<D>(0) {};
  Size(int x) : _Coordinate<D>(x) {};
  Size(int x, int y) : _Coordinate<D>(x,y) {};
  Size(int x, int y, int z) : _Coordinate<D>(x,y,z) {};
  Size(int x, int y, int z, int a) : _Coordinate<D>(x,y,z,a) {};
  Size(int x, int y, int z, int a, int b) : _Coordinate<D>(x,y,z,a,b) {};
  Size(int x, int y, int z, int a, int b, int c) : _Coordinate<D>(x,y,z,a,b,c)
  {
  };

  /**
   * Calculates the maximum size a subarray can have 
   * when we start at position pos
   */
  void operator()(const From<D>& pos)
  {
    for(int i = 0 ; i < D ; i++) 
      {
	int ws = _Coordinate<D>::coordinate[i];
	ws -= pos[i];
	if(ws<0) ws=0;
	_Coordinate<D>::coordinate[i]=ws;
      }
  }
  
  /**
   * Will take the intersection of the two size boxes
   */
  void min(const Size<D> & o)
  {
    for(int i = 0 ; i < D ; i++)
      _Coordinate<D>::coordinate[i]=::min(_Coordinate<D>::coordinate[i],o[i]);
  }
   
  template<int R> void takeFrom(const Size<R> &from) 
  {
    if(R<D)
      for(int i = 0 ; i < R ; i++) _Coordinate<D>::coordinate[i]=from[i];
    else
      for(int i = 0 ; i < D ; i++) _Coordinate<D>::coordinate[i]=from[i];
  }
};

typedef Position<2> XY;

template <int D> class From:     public _Coordinate<D> 
{
 public:
  From(const Position<D> &pos) : _Coordinate<D>(pos) {};
  From(int x) : _Coordinate<D>(x) {};
  From(int x, int y) : _Coordinate<D>(x,y) {};
  From(int x, int y, int z) : _Coordinate<D>(x,y,z) {};
  From(int x, int y, int z, int a) : _Coordinate<D>(x,y,z,a) {};
};
#endif // __loaded__coordinates_h__
