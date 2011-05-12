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
#ifndef __loaded__doc_cpp__
#define __loaded__doc_cpp__
using namespace std;
#line 1 "doc.c++"
#include <map>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <vector>
#include "do-lexer.h"
#include "do-grammar.h"
#include "do-syntax.h"
#include "do-printer.h"
outputbuf object;
outputbuf methods;
outputbuf meta;
outputbuf msgs;
outputbuf msgs1;
outputbuf globhead;
string field2init;
char* current_do=NULL;
char* current_pa=NULL;
char* makers_emitted_for=NULL;
bool access_checks=true;
extern void doset_in(FILE * i);
int doparse();
int main(int argc,char* argv[])
{
  assert(argc==3 || argc==4);
  char * ifn;
  char * ofn;
  if (strcmp(argv[1],"--noaccess")==0)
    {
      access_checks=false;
      ifn=argv[2];
      ofn=argv[3];
    }
  else
    {
      ifn=argv[1];
      ofn=argv[2];
    }
  FILE* f=fopen(ifn,"rb");
  assert(f);
  doset_in(f);
  doparse();
  fclose(f);
  ofstream out(ofn);
  out << "/****\n"
    "Data Object compiled file\n"
    "Copyright (C) 2006-2010 Werner Van Belle\n"
    "Do not change. Changes might be lost\n"
    "------------------------------------------\n"
    "\n"
    "This program is free software; you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation; either version 2 of the License, or\n"
    "(at your option) any later version.\n"
    "\n"
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n"
    "\n"
    "You should have received a copy of the GNU General Public License\n"
    "along with this program; if not, write to the Free Software\n"
    "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA\n"
    "****/\n\n";
  
  char* s = strdup(ofn);
  for(unsigned int j = 0 ; j < strlen(s) ; j++)
    {
      s[j]=toupper(s[j]);
      if (!isalnum(s[j]))
	s[j]='_';
    }
  
  out << "#ifndef __" << s <<"\n";
  out << "#define __" << s <<"\n";
  out << "#include <typeinfo>\n";
  if (access_checks)
    out << "#define DATA_RO_FIELD\n";
  out << "#include \"data-reference-count.h\"\n";
  //  out << "using namespace std;\n";
  for(unsigned i = 0 ; i < globhead.size(); i++)
    out << globhead[i];
  //  out << "\n//-------------------------------------"
  //    "\n// Data Object base messaging classes"
  //    "\n//-------------------------------------\n";
  //  for(unsigned i = 0 ; i < msgs1.size(); i++)
  //    out << msgs1[i];
  out << "\n//-------------------------------------"
    "\n// Data Meta Definition "
    "\n//-------------------------------------\n";
  for(unsigned i = 0 ; i < meta.size(); i++)
    out << meta[i];
  out << 
    "\n//-------------------------------------"
    "\n// Data Object Definition"
    "\n//-------------------------------------\n";
  for(unsigned i = 0 ; i < object.size(); i++)
    out << object[i];
  out << "\n//-------------------------------------"
    "\n// Data Methods"
    "\n//-------------------------------------\n";
  for(unsigned i = 0 ; i < methods.size(); i++)
    out << methods[i];
  out << "#endif // __" << s <<"\n";
  out.close();
  exit(0);
}
#endif // __loaded__doc_cpp__
