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
#ifndef __loaded__files_h__
#define __loaded__files_h__
using namespace std;
#line 1 "files.h++"
#include <stdio.h>
#include "array.h"
#include "stereo-sample2.h"

long fsize(FILE * f);

template <class sample_type>
long readsamples(sample_type* target, int count, FILE* file)
{
  int result;
  assert(target);
  assert(file);
  assert(count>0);
  result = fread(target,sizeof(sample_type),count,file);
  if (result<=0)
    {
      int err = ferror(file);
      if (feof(file)) 
	return 0;
      printf("file: Could not read %d samples, errno = %d (%s)\n",count,err,strerror(err));
      assert(0);
    }
  return result;
}

long   readsamples(Array<1,float4>& target, FILE* file);
long   writesamples(void* target, int count, FILE* file);
#endif // __loaded__files_h__
