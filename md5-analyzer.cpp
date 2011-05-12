/****
 BpmDj v3.6: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

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
#ifndef __loaded__md5_analyzer_cpp__
#define __loaded__md5_analyzer_cpp__
using namespace std;
#line 1 "md5-analyzer.c++"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <qstring.h>
#include <qdialog.h>
#include "player-core.h"
#include "md5-analyzer.h"
#include "scripts.h"

void Md5Analyzer::analyze()
{
  char * a = escape(argument);
  if (!vexecute("md5sum %s | awk '{printf $1}' >sum.tmp",a))
    _exit(101);
  bpmdj_deallocate(a);
  
  FILE * kloink;
  kloink=fopen("sum.tmp","r");
  char s[40];
  int i=0;
  while(i<32)
    {
      int c = getc(kloink);
      assert(c>0);
      s[i]=c;
      i++;
    }
  s[32]=0;
  fclose(kloink);
  playing->set_md5sum(strdup(s));
}
#endif // __loaded__md5_analyzer_cpp__
