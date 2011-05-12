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

#ifndef TOKEN_H
#define TOKEN_H
#include <vector>
#include <map>
#include "symbol.h"
using namespace std;

//---------------------------------------------------------------
//                            Token
//---------------------------------------------------------------
class Token: public DataClass
{
  struct TokenContent
  {
    map<Symbol, Data, symbolLesser> content;
    int refcount;
    TokenContent() : content() 
    {
      refcount = 1;
    };
  } * content;
 protected:
  virtual QString type_name() const 
    {
      return type(); 
    };
  virtual DataClass* shallow_copy() const 
    {
      return new Token(*this);
    };
  virtual void visit(DataVisitor&v);
 public:
  static QString type() 
    {
      return "Token";
    }
  Token();
  Token(const Token& dc);
  Token& operator = (const Token& dc);
  virtual ~Token();

  int count() const 
    {
      return content->content.size(); 
    };
  void define(Symbol key, Data val) 
    {
      content->content[key]=val;
    };
  /*
    Data get(const string s) 
    {
    return content->content[Symbol(s)]; 
    };
  Data& operator[](const string s) const
    {
      return content->content[Symbol(s)]; 
    };
  */
  Data& operator[](Symbol s) const
    {
      return content->content[s]; 
    };
  Data& operator[](Symbol *s) const
    {
      return content->content[*s]; 
    };
  vector<Symbol> keys() const;
};

#endif
