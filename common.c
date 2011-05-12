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

#include <assert.h>
#include <errno.h>
#include <string.h>
#include "common.h"
#include "malloc.h"

void* myalloc(int length, char* file, int line)
{
  void * result;
  // printf("%s(%d): allocating %d bytes\n",file,line,length);
  assert(length>=0);
  result = malloc(length);
  if (!result)
    printf("Error: %s(%d): unable to allocate %d bytes \n",file,line,length);
  assert(result);
  return result;
}

void* myrealloc(void* thing, int size)
{
  void * result;
  assert(size);
  result = realloc(thing,size);
  assert(result);
  return result;
}

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

void common_init()
{
   assert(sizeof(int)==4);
   assert(sizeof(signed2)==2);
   assert(sizeof(signed4)==4);
   assert(sizeof(signed8)==8);
}

long readsamples(void* target, int count, FILE* file)
{
  int result;
  assert(target);
  assert(file);
  assert(count>0);
  result = fread(target,sizeof(unsigned4),count,file);
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


