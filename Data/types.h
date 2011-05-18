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
#ifndef __loaded__types_h__
#define __loaded__types_h__
using namespace std;
#line 1 "types.h++"
#include <typeinfo>
#include <Qt/qstring.h>

// can be used to limit the maximum array size to 4 dimensions
// #define SHORT_ONE 

//===============================================================
//                         Data Types
//===============================================================
//---------------------------------------------------------------
//                            Forwards
//---------------------------------------------------------------
class DataTexter;
class Token;
class Symbol;
class Data;
class String;
class QStringLesser;
class DataVisitor;

template <int D, class T> class Array;
template <class T, bool S, char L> class  IntegerClass;
template <class T, char L> class FloatClass;

//---------------------------------------------------------------
//                     Typedefs & Typelists
//---------------------------------------------------------------
#define BASIC_TYPES \
BASIC_TYPE(signed            char, signed1,   Signed1) \
BASIC_TYPE(signed       short int, signed2,   Signed2) \
BASIC_TYPE(signed             int, signed4,   Signed4) \
BASIC_TYPE(signed   long long int, signed8,   Signed8) \
BASIC_TYPE(unsigned          char, unsigned1, Unsigned1) \
BASIC_TYPE(unsigned     short int, unsigned2, Unsigned2) \
BASIC_TYPE(unsigned           int, unsigned4, Unsigned4) \
BASIC_TYPE(unsigned long long int, unsigned8, Unsigned8) \
BASIC_TYPE(                 float, float4,    Float4) \
BASIC_TYPE(                double, float8,    Float8) 

#define BASIC_TYPE(A,B,C) typedef A B;
BASIC_TYPES
#undef BASIC_TYPE

typedef IntegerClass<signed1,true,'b'>    Signed1;
typedef IntegerClass<signed2,true,'w'>    Signed2;
typedef IntegerClass<signed4,true,'i'>    Signed4;
typedef IntegerClass<signed8,true,'l'>    Signed8;
typedef IntegerClass<unsigned1,false,'b'> Unsigned1;
typedef IntegerClass<unsigned2,false,'w'> Unsigned2;
typedef IntegerClass<unsigned4,false,'i'> Unsigned4;
typedef IntegerClass<unsigned8,false,'l'> Unsigned8;
typedef FloatClass<float4,'f'>   Float4;
typedef FloatClass<float8,'d'>   Float8;

// A list of useful array types
#ifdef SHORT_ONE
#define ARRAY_TYPES \
 ARRAY_TYPE(1,signed1)  ARRAY_TYPE(2,signed1)  ARRAY_TYPE(3,signed1)  ARRAY_TYPE(4,signed1) \
ARRAY_TYPE(1,signed2)  ARRAY_TYPE(2,signed2)  ARRAY_TYPE(3,signed2)  ARRAY_TYPE(4,signed2) \
ARRAY_TYPE(1,signed4)  ARRAY_TYPE(2,signed4)  ARRAY_TYPE(3,signed4)  ARRAY_TYPE(4,signed4) \
ARRAY_TYPE(1,signed8)  ARRAY_TYPE(2,signed8)  ARRAY_TYPE(3,signed8)  ARRAY_TYPE(4,signed8) \
ARRAY_TYPE(1,unsigned1) ARRAY_TYPE(2,unsigned1) ARRAY_TYPE(3,unsigned1) ARRAY_TYPE(4,unsigned1) \
ARRAY_TYPE(1,unsigned2) ARRAY_TYPE(2,unsigned2) ARRAY_TYPE(3,unsigned2) ARRAY_TYPE(4,unsigned2) \
ARRAY_TYPE(1,unsigned4) ARRAY_TYPE(2,unsigned4) ARRAY_TYPE(3,unsigned4) ARRAY_TYPE(4,unsigned4) \
ARRAY_TYPE(1,unsigned8) ARRAY_TYPE(2,unsigned8) ARRAY_TYPE(3,unsigned8) ARRAY_TYPE(4,unsigned8) \
ARRAY_TYPE(1,float4)  ARRAY_TYPE(2,float4)  ARRAY_TYPE(3,float4)  ARRAY_TYPE(4,float4) \
ARRAY_TYPE(1,float8)  ARRAY_TYPE(2,float8)  ARRAY_TYPE(3,float8)  ARRAY_TYPE(4,float8) \
ARRAY_TYPE(1,Data)   ARRAY_TYPE(2,Data)   ARRAY_TYPE(3,Data)   ARRAY_TYPE(4,Data) 
#else
#define ARRAY_TYPES \
ARRAY_TYPE(1,signed1) ARRAY_TYPE(2,signed1) ARRAY_TYPE(3,signed1) \
ARRAY_TYPE(4,signed1) ARRAY_TYPE(5,signed1) ARRAY_TYPE(6,signed1) \
ARRAY_TYPE(7,signed1) ARRAY_TYPE(8,signed1) ARRAY_TYPE(1,signed2) \
ARRAY_TYPE(2,signed2) ARRAY_TYPE(3,signed2) ARRAY_TYPE(4,signed2) \
ARRAY_TYPE(5,signed2) ARRAY_TYPE(6,signed2) ARRAY_TYPE(7,signed2) \
ARRAY_TYPE(8,signed2) ARRAY_TYPE(1,signed4) ARRAY_TYPE(2,signed4) \
ARRAY_TYPE(3,signed4) ARRAY_TYPE(4,signed4) ARRAY_TYPE(5,signed4) \
ARRAY_TYPE(6,signed4) ARRAY_TYPE(7,signed4) ARRAY_TYPE(8,signed4) \
ARRAY_TYPE(1,signed8) ARRAY_TYPE(2,signed8) ARRAY_TYPE(3,signed8) \
ARRAY_TYPE(4,signed8) ARRAY_TYPE(5,signed8) ARRAY_TYPE(6,signed8) \
ARRAY_TYPE(7,signed8) ARRAY_TYPE(8,signed8) ARRAY_TYPE(1,unsigned1) \
ARRAY_TYPE(2,unsigned1) ARRAY_TYPE(3,unsigned1) ARRAY_TYPE(4,unsigned1) \
ARRAY_TYPE(5,unsigned1) ARRAY_TYPE(6,unsigned1) ARRAY_TYPE(7,unsigned1) \
ARRAY_TYPE(8,unsigned1) ARRAY_TYPE(1,unsigned2) ARRAY_TYPE(2,unsigned2) \
ARRAY_TYPE(3,unsigned2) ARRAY_TYPE(4,unsigned2) ARRAY_TYPE(5,unsigned2) \
ARRAY_TYPE(6,unsigned2) ARRAY_TYPE(7,unsigned2) ARRAY_TYPE(8,unsigned2) \
ARRAY_TYPE(1,unsigned4) ARRAY_TYPE(2,unsigned4) ARRAY_TYPE(3,unsigned4) \
ARRAY_TYPE(4,unsigned4) ARRAY_TYPE(5,unsigned4) ARRAY_TYPE(6,unsigned4) \
ARRAY_TYPE(7,unsigned4) ARRAY_TYPE(8,unsigned4) ARRAY_TYPE(1,unsigned8) \
ARRAY_TYPE(2,unsigned8) ARRAY_TYPE(3,unsigned8) ARRAY_TYPE(4,unsigned8) \
ARRAY_TYPE(5,unsigned8) ARRAY_TYPE(6,unsigned8) ARRAY_TYPE(7,unsigned8) \
ARRAY_TYPE(8,unsigned8) ARRAY_TYPE(1,float4) ARRAY_TYPE(2,float4) \
ARRAY_TYPE(3,float4) ARRAY_TYPE(4,float4) ARRAY_TYPE(5,float4) \
ARRAY_TYPE(6,float4) ARRAY_TYPE(7,float4) ARRAY_TYPE(8,float4) \
ARRAY_TYPE(1,float8) ARRAY_TYPE(2,float8) ARRAY_TYPE(3,float8) \
ARRAY_TYPE(4,float8) ARRAY_TYPE(5,float8) ARRAY_TYPE(6,float8) \
ARRAY_TYPE(7,float8) ARRAY_TYPE(8,float8) ARRAY_TYPE(1,Data) \
ARRAY_TYPE(2,Data) ARRAY_TYPE(3,Data) ARRAY_TYPE(4,Data) ARRAY_TYPE(5,Data) \
ARRAY_TYPE(6,Data) ARRAY_TYPE(7,Data) ARRAY_TYPE(8,Data) 
#endif
#endif // __loaded__types_h__
