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

#ifndef OM_STRING_H
#define OM_STRING_H
#include "data.h"

//---------------------------------------------------------------
//                            String
//---------------------------------------------------------------
class String: public DataClass
{
  QString text;
 protected:
  virtual QString type_name() const 
    {
      return type(); 
    };
  virtual DataClass* shallow_copy() const 
    {
      return new String(text);
    };
 public:
  static QString type()
    {
      return "String";
    }
  String(QString s) : text(s) 
    {
    };
  QString operator ()() const 
    {
      return text;
    };
  operator QString() const 
    {
      return text;
    }
  static String parse(const char *);
 protected:
  virtual void visit(DataVisitor& v);
};

#endif
