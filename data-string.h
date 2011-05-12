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
#ifndef __loaded__data_string_h__
#define __loaded__data_string_h__
using namespace std;
#line 1 "data-string.h++"
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
  String(): text("")
    {
    };
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
#endif // __loaded__data_string_h__
