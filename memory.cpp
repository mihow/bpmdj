/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

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
#include <assert.h>
#include "memory.h"
#include "malloc.h"
#include "efence.h"

void myfree(void* a)
{
#ifdef EFENCE
  if (!efence_free(a))
#endif
    free(a);
}

void* myalloc(int length, char* file, int line)
{
  void * result;
  // printf("%s(%d): allocating %d bytes\n",file,line,length);
  assert(length>=0);
  if (length==0) return NULL;
#ifdef EFENCE
  result = efence_malloc(length);
#else
  result = malloc(length);
#endif
#ifdef COUNT_ALLOCATIONS
  static int nrallocs = 0;
  nrallocs++;
  if (nrallocs%100==0)
    printf("%d allocs ",nrallocs);
#endif
  if (!result)
    printf("Error: %s(%d): unable to allocate %d bytes \n",file,line,length);
  assert(result);
  return result;
}

void* myrealloc(void* thing, int size)
{
  void * result;
  assert(size);
#ifdef EFENCE
  result = efence_realloc(thing,size);
#else
  result = realloc(thing,size);
#endif
  assert(result);
  return result;
}

char* zeroable(char* in)
{
  if (!in) return NULL;
  if (!*in) 
    {
      bpmdj_deallocate(in);
      return NULL;
    }
  return in;
}

