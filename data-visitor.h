/****
 Borg4 Data Library
 Copyright (C) 2005-2009 Werner Van Belle

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
#ifndef __loaded__data_visitor_h__
#define __loaded__data_visitor_h__
using namespace std;
#line 1 "data-visitor.h++"
#include <assert.h>
#include "data.h"

//===============================================================
//                         Data Visitor
//===============================================================
class DataVisitor
{
  friend class Token;
  friend class String;
  friend class Symbol;
  friend class IntegerClass<signed1,true,'b'>;   
  friend class IntegerClass<signed2,true,'w'>;    
  friend class IntegerClass<signed4,true,'i'>;   
  friend class IntegerClass<signed8,true,'l'>;   
  friend class IntegerClass<unsigned1,false,'b'>;
  friend class IntegerClass<unsigned2,false,'w'>;
  friend class IntegerClass<unsigned4,false,'i'>;
  friend class IntegerClass<unsigned8,false,'l'>;
  friend class FloatClass<float4,'f'>;
  friend class FloatClass<float8,'d'>;
 protected:
  // token visitors
  virtual void visit(Token &) {assert(0);}
  // array visitors
#define ARRAY_TYPE(D,T) \
  friend class Array<D,T>; \
  virtual void visit(Array<D,T> &) {assert(0);};
  ARRAY_TYPES;
#undef ARRAY_TYPE
  virtual void visit(String &)  {assert(0);};
  virtual void visit(Symbol &)  {assert(0);};
  // number visitors
  // the numbers are standard rerouted to the largest possible representation
  virtual void visit(Signed1 &nr);
  virtual void visit(Signed2 &nr);
  virtual void visit(Signed4 &nr);
  virtual void visit(Signed8 &nr);
  virtual void visit(Unsigned1 &nr);
  virtual void visit(Unsigned2 &nr);
  virtual void visit(Unsigned4 &nr);
  virtual void visit(Unsigned8 &nr);
  virtual void visitSignedNumber(signed8, char) {assert(0);};
  virtual void visitUnsignedNumber(unsigned8, char) {assert(0);};
  virtual void visit(Float4 &) { assert(0); };
  virtual void visit(Float8 &) { assert(0); };
 public:
  void visit(Data & data);
  void visit(DataClass & data);
  virtual ~DataVisitor() {};
};
#endif // __loaded__data_visitor_h__
