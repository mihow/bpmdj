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
#ifndef __loaded__symbol_cpp__
#define __loaded__symbol_cpp__
using namespace std;
#line 1 "symbol.c++"
#include "symbol.h"
#include "data-visitor.h"
#include <set>
using namespace std;

//---------------------------------------------------------------
//                            Symbol
//---------------------------------------------------------------
struct QStringLesser
{
  bool operator()(QString *s1, QString *s2) const {return (*s1)<(*s2);};
};

static set<QString> symbol_table;

void Symbol::init(const QString& s)
{      
   set<QString>::iterator pos = symbol_table.find(s);
   if (pos==symbol_table.end())
     symbol_table.insert(text=s);
   else 
     text = *pos;
}

void Symbol::visit(DataVisitor& v)
{
  v.visit(*this);
};
#endif // __loaded__symbol_cpp__
