/****
 BpmDj v4.1pl1: Free Dj Tools
 Copyright (C) 2001-2010 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.

 See the authors.txt for a full list of people involved.
****/
#ifndef __loaded__md5_analyzer_cpp__
#define __loaded__md5_analyzer_cpp__
using namespace std;
#line 1 "md5-analyzer.c++"
#include <cstdio>
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
