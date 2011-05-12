/****
 Active Objects
 Copyright (C) 2006-2010 Werner Van Belle

 http://werner.yellowcouch.org/Borg4/group__ao.html

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
#ifndef __loaded__aoc_cpp__
#define __loaded__aoc_cpp__
using namespace std;
#line 1 "aoc.c++"
#include <map>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <vector>
#include "ao-lexer.h"
#include "ao-grammar.h"
#include "ao-syntax.h"
#include "sao-printer.h"
outputbuf object;
outputbuf object2;
outputbuf meta;
outputbuf msgs;
outputbuf msgs1;
outputbuf globhead;
string field2init;
char* current_ao;
void aoset_in(FILE * i);
int aoparse();
int main(int argc,char* argv[])
{
  assert(argc==3);
  FILE* f=fopen(argv[1],"rb");
  assert(f);
  aoset_in(f);
  aoparse();
  fclose(f);
  ofstream out(argv[2]);
  out << "/****\n"
    " Active Object compiled file\n"
    " Copyright (C) 2006-2010 Werner Van Belle\n"
    " Do not modify. Changes might be lost\n"
    " --------------------------------------------\n"
    " This program is free software; you can redistribute it and/or modify\n"
    " it under the terms of the GNU General Public License as published by\n"
    " the Free Software Foundation; either version 2 of the License, or\n"
    " (at your option) any later version.\n"
    "\n"
    " This program is distributed in the hope that it will be useful,\n"
    " but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    " MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    " GNU General Public License for more details.\n"
    "****/\n\n";
  
  char* s = strdup(argv[2]);
  for(unsigned int j = 0 ; j < strlen(s) ; j++)
    {
      s[j]=toupper(s[j]);
      if (!isalnum(s[j]))
	s[j]='_';
    }

  out << "#ifndef __" << s <<"\n";
  out << "#define __" << s <<"\n";
  out << "#include \"active-objects.h\"\n";
  out << "#include \"reference-count.h\"\n";
  out << "using namespace std;\n";
  for(unsigned i = 0 ; i < globhead.size(); i++)
    out << globhead[i];
  out << "\n//-------------------------------------"
    "\n// Active Object base messaging classes"
    "\n//-------------------------------------\n";
  for(unsigned i = 0 ; i < msgs1.size(); i++)
    out << msgs1[i];
  out << "\n//-------------------------------------"
    "\n// Main object definition"
    "\n//-------------------------------------\n";
  for(unsigned i = 0 ; i < object.size(); i++)
    out << object[i];
  out << "\n//-------------------------------------"
    "\n// Specific messaging classes "
    "\n//-------------------------------------\n";
  for(unsigned i = 0 ; i < msgs.size(); i++)
    out << msgs[i];
  out << "\n//-------------------------------------"
    "\n// Active Object wrapper "
    "\n//-------------------------------------\n";
  for(unsigned i = 0 ; i < meta.size(); i++)
    out << meta[i];
  out << "\n//-------------------------------------"
    "\n// Active Object Methods"
    "\n//-------------------------------------\n";
  for(unsigned i = 0 ; i < object2.size(); i++)
    out << object2[i];
  out << "#endif // __" << s <<"\n";
  out.close();
  exit(0);
}
#endif // __loaded__aoc_cpp__
