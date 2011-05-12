#ifndef __loaded__data_cpp__
#define __loaded__data_cpp__
using namespace std;
#line 1 "data.c++"
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

#ifndef OM_DATA_CPP
#define OM_DATA_CPP
#include "data.h"
#include "null.h"
#include "numbers.h"
#include "array.h"
#include "symbol.h"
#include "data-string.h"
#include "data-token.h"

//---------------------------------------------------------------
//                            Data 
//---------------------------------------------------------------
DataClass* DataClass::memory_copy() const
{
  if (allocation_count)
    {
      //      printf("Avoided heap copy %x\n",this);
      int * ac = (int*)(void*)&allocation_count;
      (*ac)++;
      return (DataClass*)(void*)this;
    }
  else
    {
      DataClass* answer = shallow_copy();
      answer->allocation_count = 1;
      //      printf("Translation to heap %x -> %x\n",this,answer);
      return answer;
    }
};

void DataClass::memory_release()
{
  //  printf("About to delete object %x\n", this);
  // we can never be not on the heap
  assert(allocation_count>0);
  if (--allocation_count==0)
    {
      //      printf("Autodeletion object %x\n", this);
      delete this;
    }
}

void Data::type_error(QString expected) const
{
  const char * a;
  const char * b;
#if QT_VERSION > 0x040000
  a = expected.toAscii();
  b = type().toAscii();
#else
  a = expected.ascii();
  b = type().ascii();
#endif
  printf("Expected %s, got %s\n",a,b);
  assert(0);
  _exit(0);
}

Data::Data()
{
  content = new Null();
  content->allocation_count = 1;
}

Data::Data(const Data& other)
{
  assert(other.content);
  content = other.content->memory_copy();
}

Data::Data(const DataClass & blah)
{
  content = blah.memory_copy();
}

Data::~Data()
{
  assert(content);
  //  printf("Deleting data %x\n",content);
  content->memory_release();
  content = NULL;
}

Data& Data::operator = (const Data& other)
{
  if (content!=other.content)
    {
      content -> memory_release();
      content =  other.content->memory_copy();
    }
  return *this;
}

bool Data::isNull() const
{
  return typeid(*content)==typeid(Null);
}

#define ARRAY_TYPE(T,D) \
Data::operator Array<T,D>() const \
{ \
  if (typeid(*content)!=typeid(Array<T,D>)) \
    type_error("Array<"#T ","#D ">"); \
  return *(Array<T,D>*)content; \
}
ARRAY_TYPES
#undef ARRAY_TYPE

#define BASIC_TYPE(T,N,C) \
Data::operator C() const \
{\
  if (typeid(*content)!=typeid(C)) \
    type_error(#C); \
  return *(C*)content; \
}
BASIC_TYPES;
#undef BASIC_TYPE

Data::operator Symbol() const
{
  if (typeid(*content)!=typeid(Symbol))
    type_error("Symbol");
  return *(Symbol*)content;
}

Data::operator String() const
{
  assert(typeid(*content)==typeid(String));
  return *(String*)content;
}

Data::operator Token() const
{
  if (typeid(*content)!=typeid(Token))
    type_error("Token");
  return *(Token*)content;
}

void Data::visit(DataVisitor& v) 
{
  content->visit(v);
};

#endif
#endif // __loaded__data_cpp__
