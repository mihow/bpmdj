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
#ifndef __loaded__data_visitor_cpp__
#define __loaded__data_visitor_cpp__
using namespace std;
#line 1 "data-visitor.c++"
#include "data-visitor.h"
#include "numbers.h"

void DataVisitor::visit(Data & data)
{
  data.visit(*this);
}

void DataVisitor::visit(DataClass & data)
{
  data.visit(*this);
}

void DataVisitor::visit(Signed1 &nr)   
{
  visitSignedNumber(nr(),nr.letter());
};

void DataVisitor::visit(Signed2 &nr)  
{
  visitSignedNumber(nr(),nr.letter());
};

void DataVisitor::visit(Signed4 &nr) 
{
  visitSignedNumber(nr(),nr.letter());
};

void DataVisitor::visit(Signed8 &nr) 
{
  visitSignedNumber(nr(),nr.letter());
};

void DataVisitor::visit(Unsigned1 &nr)
{
  visitUnsignedNumber(nr(),nr.letter());
};

void DataVisitor::visit(Unsigned2 &nr)
{
  visitUnsignedNumber(nr(),nr.letter());
};

void DataVisitor::visit(Unsigned4 &nr)
{
  visitUnsignedNumber(nr(),nr.letter());
};

void DataVisitor::visit(Unsigned8 &nr) 
{
  visitUnsignedNumber(nr(),nr.letter());
};

#endif // __loaded__data_visitor_cpp__
