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

#ifndef OM_SYMBOL_H
#define OM_SYMBOL_H
#include <qstring.h>
#include "data.h"

//---------------------------------------------------------------
//                            Symbol
//---------------------------------------------------------------
// symbols are represented as strings. Their
// unique quality is to be found in the quick
// comparators due to storing all symbols in a 
// symboltable. The actual order might not be 
// alphabetically.
class symbolLesser;
class Symbol: public DataClass
{
 private:
  QString *text;
  Symbol(QString * s) : text(s)
    {
    }
  void init(QString s);
 protected:
  virtual DataClass* shallow_copy() const
    {
      return new Symbol(text);
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
  Symbol(QString s);
  Symbol(const Symbol& s) : DataClass(), text(s.text)
    {
    }; 
  Symbol &operator =(const Symbol &s) 
    {
      text=s.text;
      return *this;
    };
  QString& operator ()() const 
    {
      return *text; 
    };
  operator QString&() const
    {
      return *text;
    }
  int length() const 
    {
      return text->length();
    };
  bool operator <   (const Symbol & other);
  bool operator ==  (const Symbol & other);
};

struct symbolLesser
{
  bool operator()(Symbol s1, Symbol s2) const 
  {
    return s1<s2;
  };
};

#endif
