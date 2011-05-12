#ifndef __loaded__numbers_h__
#define __loaded__numbers_h__
using namespace std;
#line 1 "numbers.h++"
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

#ifndef OM_NUMBERS_H
#define OM_NUMBERS_H
#include "data.h"
#include "data-visitor.h"

//===============================================================
//                            Numbers
//===============================================================

//---------------------------------------------------------------
//                            Integer
//---------------------------------------------------------------
template <class T, bool S, char C> class IntegerClass: public DataClass
{
 private:
  T value;
 protected:
  virtual QString type_name() const 
    {
      return "Integer"; 
    };
  virtual void visit(DataVisitor& v) 
    {
      v.visit(*this);
    }
 public:
  IntegerClass() : value(0) 
    {
    };
  IntegerClass(T i) : value(i) 
    {
    };
  IntegerClass(const char* text)
    {
      unsigned8 number;
      if (S)
	number = atoll(text+1);
      else
	number = atoll(text);
      if (S && text[0]=='-') 
	{
	  value=(T)-number;
	  //      assert(-value==number);
	}
      else 
	{
	  value = (T)number;
	  //      assert(value==number);
	}
    }
  virtual DataClass* shallow_copy() const 
    {
      return new IntegerClass<T,S,C>(*this);
    };
  operator T() const 
    {
      return value; 
    };
  T operator()() const 
    {
      return value; 
    };
  char letter() const 
    { 
      return C; 
    };
  IntegerClass<T,S,C>& operator= (const IntegerClass<T,S,C> & o) 
    {
      value=o.value; 
      return *this;
    };
  IntegerClass<T,S,C>& operator= (T t) 
    {
      value=t; 
      return *this;
    };
};

//---------------------------------------------------------------
//                            Floats
//---------------------------------------------------------------
template <class T, char C> class FloatClass: public DataClass
{
 private:
  T value;
 protected:
  virtual QString type_name() const
    { 
      return "Float"; 
    };
  virtual void visit(DataVisitor& v)
    {
      v.visit(*this);
    };
 public:
  FloatClass() : value(0) 
    {
    };
  FloatClass(T i) : value(i) 
    {
    };
  FloatClass(const char* text)
    {
      value = atof(text);
    }
  virtual DataClass* shallow_copy() const 
    {
      return new FloatClass<T,C>(*this);
    };
  T operator()() const 
    {
      return value; 
    };
  operator T() const 
    {
      return value; 
    };
  char letter() const 
    { 
      return C; 
    };
  FloatClass<T,C>& operator= (const FloatClass<T,C> & o) 
    {
      value=o.value; 
      return *this;
    };
  FloatClass<T,C>& operator= (T t) 
    {
      value=t; 
      return *this;
    };
};

#endif
#endif // __loaded__numbers_h__
