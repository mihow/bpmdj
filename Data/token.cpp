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

#include "token.h"
#include "data-visitor.h"

//---------------------------------------------------------------
//                            Token
//---------------------------------------------------------------
Token::Token() 
{
  content = new TokenContent();
  //  printf("constructor: Init content %x(%d)\n",content,content->refcount);
};

Token::Token(const Token& dc) : DataClass()
{
  content = dc.content;
  content -> refcount++;
  //  printf("copy: Increased content %x(%d)\n",content,content->refcount);
}

Token::~Token()
{
  if (--content->refcount==0)
    {
      //      printf("~: Deleting content %x(%d)\n",content,content->refcount);
      delete content;
    }
  else
    {
      //      printf("~: Decreased content %x(%d)\n",content,content->refcount);
    }
}

Token& Token::operator = (const Token& dc)
{
  if (content!=dc.content)
    {
      if(--content->refcount==0) 
	{
	  //	  printf("=: Deleting content %x(%d)\n",content,content->refcount);
	  delete content;
	}
      else
	{
	  //	  printf("=: Decreased content %x(%d)\n",content,content->refcount);
	}
      content = dc.content;
      content->refcount++;
      //      printf("=: Increased content %x(%d)\n",content,content->refcount);
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

