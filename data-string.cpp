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
#ifndef __loaded__data_string_cpp__
#define __loaded__data_string_cpp__
using namespace std;
#line 1 "data-string.c++"
#include "data-string.h"
#include "data-visitor.h"

//---------------------------------------------------------------
//                            String
//---------------------------------------------------------------
// strings start with a ' and end with a '. 
// if a ' is to be placed in a string it should be done as ''
// newlines in a string are printed and then at an appropriate
// column a > is placed. When String prints a string it can
// insert appropriate newlines by printing a newline and starting
// the next line with a |.
void frontup(char* target, char * source)
{
  memmove(target,source,strlen(source)+1);
}

// When reading a string all data is read and afterward filtered out. 
// '' is replaced with '
// \n\ *| is removed from the string.
// \n\ *> is replaced with \n  
String String::parse(const char *i)
{
  assert(i);
  char * final = strdup(i);
  char * cur = final;
  while(*cur)
    {
      if (cur[0]=='\'' && cur[1]=='\'')
	{
	  frontup(cur,cur+1);
	  cur++;
	}
      else if (*cur=='\n')
	{
	  char * end=cur;
	  while(*end && !(*end=='|' || *end=='>'))
	    end++;
	  assert(*end);
	  if (*end=='|')
	    frontup(cur,end+1);
	  else if (*end=='>')
	    {
	      frontup(cur+1,end+1);
	      cur++;
	    }
	}
      else
	cur++;
    }
  return String(QString::fromUtf8(final));
}

void String::visit(DataVisitor &v)
{
  v.visit(*this);
}
#endif // __loaded__data_string_cpp__
