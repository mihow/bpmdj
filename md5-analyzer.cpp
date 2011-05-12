/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001 Werner Van Belle
 See 'BeatMixing.ps' for more information

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player-core.h"
#include "md5-analyzer.h"
#include "scripts.h"
#include "index.h"

void Md5Analyzer::run()
{
  FILE * kloink;
  if (!vexecute("md5sum \"%s\" | awk '{printf $1}' >sum.tmp\n",argument))
    exit(101);
  kloink=fopen("sum.tmp","r");
  char s[40];
  int i=0;
  while(i<32)
    {
      int c = getc(kloink);
      s[i]=c;
      i++;
    }
  s[32]=0;
  fclose(kloink);
  if (!Index::index->index_md5sum ||
      strcmp(Index::index->index_md5sum,s)!=0)
    {
      Index::index->index_md5sum = strdup(s);
      Index::index->index_changed=1;
    }
}
