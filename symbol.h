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
#ifndef __loaded__symbol_h__
#define __loaded__symbol_h__
using namespace std;
#line 1 "symbol.h++"
#include <Qt/qstring.h>
#include "data.h"

//---------------------------------------------------------------
//                            Symbol
//---------------------------------------------------------------
// symbols are represented as strings. Their
// unique quality is to be found in the quick
// comparators due to storing all symbols in a 
// symbol table. The actual order might not be 
// alphabetically.
class symbolLesser;
class Symbol: public DataClass
{
 private:
  QString text;
  void init(const QString& s);
 protected:
  virtual DataClass* shallow_copy() const
    {
      return new Symbol(*this);
    };
  virtual QString type_name() const 
    {
      return type(); 
    };
  virtual void visit(DataVisitor& v);
 public:
  static QString type()
    {
      return "Symbol";
    }
  Symbol() :  text(NULL) 
    {
    }; 
  Symbol(const QString &s)
    {
      init(s);
    }
  Symbol(const Symbol& s) : DataClass(), text(s.text)
    {
    }; 
  Symbol &operator =(const Symbol &s) 
    {
      text=s.text;
      return *this;
    };
  const QString& operator ()() const 
    {
      return text; 
    };
  operator const QString&() const
    {
      return text;
    }
  operator QString() const
    {
      return text;
    }
  int length() const 
    {
      return text.length();
    };
  bool operator <   (const Symbol & other)
    {
      return text < other.text;
    }
  bool operator ==  (const Symbol & other)
    {
      return text < other.text;
    }
};

struct symbolLesser
{
  bool operator()(Symbol s1, Symbol s2) const 
  {
    return s1<s2;
  };
};
#endif // __loaded__symbol_h__
