/****
 Active Objects v4.3
 Copyright (C) 2006-2011 Werner Van Belle
 http://active.yellowcouch.org/

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
outputbuf useslist;
outputbuf impl;
map<string,string> field2init;
char* current_ao;
void aoset_in(FILE * i);
int aoparse();

bool pascal=false;

void dopascal(int, char* argv[])
{
  pascal=true;
  print(useslist,"Active_Objects");

  FILE* f=fopen(argv[1],"rb");
  assert(f);
  aoset_in(f);
  aoparse();
  fclose(f);
  
  
  char* s = strdup(argv[2]);
  char* s2=strdup(s);
  s2[strlen(s2)-4]=0;
  
  char pascal_fwd[100];
  char pascal_ifc[100];
  char pascal_imp[100];
  strcpy(pascal_fwd,s2);
  strcpy(pascal_ifc,s2);
  strcpy(pascal_imp,s2);
  strcat(pascal_fwd,"_fwd.inc");
  strcat(pascal_ifc,"_ifc.inc");
  strcat(pascal_imp,"_imp.inc");

  ofstream fwd(pascal_fwd);
  ofstream ifc(pascal_ifc);
  ofstream imp(pascal_imp);
  
  fwd << "{Active Object compiled file\n"
    "Do not modify. Changes might be lost\n"
      " --------------------------------------------}\n";
  ifc << "{Active Object compiled file\n"
    "Do not modify. Changes might be lost\n"
      " --------------------------------------------}\n";
  imp << "{Active Object compiled file\n"
    "Do not modify. Changes might be lost\n"
    " --------------------------------------------}\n";
  
  //  out << "Unit " << s2 << ";\n";
  //  out << "{$mode objfpc}{$H+}\n";
  
  //  out << "\nINTERFACE\nUses ";
  //  for(unsigned i = 0 ; i < useslist.size(); i++)
  //  out << useslist[i];
  //  out << ";\n";
  
  for(unsigned i = 0 ; i < globhead.size(); i++)
    fwd << globhead[i];
  
  
  ifc << "\n//-------------------------------------"
    "\n// Active Object base messaging classes"
    "\n//-------------------------------------\n";
  for(unsigned i = 0 ; i < msgs1.size(); i++)
    ifc << msgs1[i];
  
  ifc << "\n//-------------------------------------"
    "\n// Main object definition"
    "\n//-------------------------------------\n";
  for(unsigned i = 0 ; i < object.size(); i++)
    ifc << object[i];
  ifc << "\n//-------------------------------------"
    "\n// Specific messaging classes "
    "\n//-------------------------------------\n";
  for(unsigned i = 0 ; i < msgs.size(); i++)
    ifc << msgs[i];
  ifc << "\n//-------------------------------------"
    "\n// Active Object wrapper "
    "\n//-------------------------------------\n";
  for(unsigned i = 0 ; i < meta.size(); i++)
    ifc << meta[i];

  imp << "\n//-------------------------------------"
    "\n// Active Object Methods "
    "\n//-------------------------------------\n";
  for(unsigned i = 0 ; i < object2.size(); i++)
    imp << object2[i];
  
  //out << "\n\nIMPLEMENTATION\n\n";
  for(unsigned i = 0 ; i < impl.size(); i++)
    imp << impl[i];

  ifc.close();
  imp.close();
  fwd.close();
}

void cpp(int, char* argv[])
{
  FILE* f=fopen(argv[1],"rb");
  assert(f);
  aoset_in(f);
  aoparse();
  fclose(f);
  
  ofstream out(argv[2]);
  if (!pascal)
    out << "/****\n"
      " Active Object compiled file\n"
      " Copyright (C) 2006-2011 Werner Van Belle\n"
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
  else
    out << "{Active Object compiled file\n"
      "Do not modify. Changes might be lost\n"
      " --------------------------------------------}\n";
  
  char* s = strdup(argv[2]);
  
  char* s2=strdup(s);
  s2[strlen(s2)-4]=0;
  
  char pascal_fwd[100];
  char pascal_ifc[100];
  char pascal_imp[100];
  strcpy(pascal_fwd,s2);
  strcpy(pascal_ifc,s2);
  strcpy(pascal_imp,s2);
  strcat(pascal_fwd,"_fwd.inc");
  strcat(pascal_ifc,"_ifc.inc");
  strcat(pascal_imp,"_imp.inc");

  for(unsigned int j = 0 ; j < strlen(s) ; j++)
    {
      s[j]=toupper(s[j]);
      if (!isalnum(s[j]))
	s[j]='_';
    }

  if (!pascal)
    {
      out << "#ifndef __" << s <<"\n";
      out << "#define __" << s <<"\n";     
      char* tmp=strdup(argv[0]);
      tmp[strlen(tmp)-3]=0;
      out << "#include \"" << tmp << "active-objects.h\"\n";
      free(tmp);
      out << "using namespace std;\n";
    }
  else 
    {
      out << "Unit " << s2 << ";\n";
      out << "{$mode objfpc}{$H+}\n";
      out << "\nINTERFACE\nUses ";
      for(unsigned i = 0 ; i < useslist.size(); i++)
	out << useslist[i];
      out << ";\n";
    }
  
  for(unsigned i = 0 ; i < globhead.size(); i++)
    out << globhead[i];
  
  if (!pascal)
    {
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
    }
  else
    {
      // pascal
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
	"\n// Active Object Methods "
	"\n//-------------------------------------\n";
      for(unsigned i = 0 ; i < object2.size(); i++)
	out << object2[i];
      
      out << "\n\nIMPLEMENTATION\n\n";
      for(unsigned i = 0 ; i < impl.size(); i++)
	out << impl[i];
      out << "end.\n";
    }
  out.close();
  exit(0);
}

int main(int argc,char* argv[])
{
  assert(argc==3 || argc==4);
  if (argc==4)
    {
      assert(strcmp(argv[3],"--pascal")==0);
      dopascal(argc,argv);
    }
  else
    {
      cpp(argc,argv);
    }
}
#endif // __loaded__aoc_cpp__
