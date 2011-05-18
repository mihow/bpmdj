/****
 Hierarchical Data Objects
 Copyright (C) 2005-2011 Werner Van Belle
 http://flow.yellowcouch.org/data/

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
#ifndef __loaded__data_token_cpp__
#define __loaded__data_token_cpp__
using namespace std;
#line 1 "data-token.c++"
#include "data-token.h"
#include "data-visitor.h"

//---------------------------------------------------------------
//                            Token
//---------------------------------------------------------------
Token::Token() 
{
  content = new TokenContent();
};

Token::Token(const Token& dc) : DataClass()
{
  content = dc.content;
  content -> refcount++;
}

Token::~Token()
{
  if (--content->refcount==0)
    delete content;
}

Token& Token::operator = (const Token& dc)
{
  if (content!=dc.content)
    {
      if(--content->refcount==0) 
	delete content;
      content = dc.content;
      content->refcount++;
    }
  return *this;
};

vector<Symbol> Token::keys() const
{
  vector<Symbol> result;
  map<Symbol, Data, symbolLesser>::const_iterator cur = content->content.begin();
  while(cur!=content->content.end())
    {
      pair<Symbol,Data> v=*cur;
      result.push_back(v.first);
      cur++;
    }
  return result;
};

void Token::visit(DataVisitor&v)
{
  v.visit(*this);
}

#endif // __loaded__data_token_cpp__
