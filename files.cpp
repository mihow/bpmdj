/****
 BpmDj v3.6: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

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
#ifndef __loaded__files_cpp__
#define __loaded__files_cpp__
using namespace std;
#line 1 "files.c++"
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "stereo-sample2.h"
#include "files.h"
#include "types.h"
#include "memory.h"

long fsize(FILE * f)
{
  long answer, pos;
  assert(f);
  pos = ftell(f);
  fseek(f,0,SEEK_END);
  answer=ftell(f);
  fseek(f,pos,SEEK_SET);
  return answer;
}

long readsamples(Array<1,float4>& signal, FILE* file)
{
  int count = signal.size(0);
  if (count==0) return 0;
  int result;
  assert(file);
  stereo_sample2 * target = bpmdj_allocate(count, stereo_sample2);
  result = fread(target,sizeof(unsigned4),count,file);
  if (result<=0)
    {
      int err = ferror(file);
      if (feof(file)) 
	return 0;
      printf("file: Could not read %d samples, errno = %d (%s)\n",count,err,strerror(err));
      assert(0);
    }
  Array<1,float4>::positions it(signal);
  while(it.more())
    {
      int p = it.position[0];
      int v = target[p].summed();
      it = v;
      it++;
    }
  bpmdj_deallocate(target);
  return result;
}

long writesamples(void* target, int count, FILE* file)
{
  int result;
  assert(target);
  assert(file);
  if (!count)
    return count;
  assert(count>0);
  result = fwrite(target,sizeof(unsigned4),count,file);
  if (result<=0)
    {
      int err = ferror(file);
      printf("file: Could not write %d samples, errno = %d (%s)\n",count,err,strerror(err));
    }
  assert(result>0);
  return result;
}

#endif // __loaded__files_cpp__
