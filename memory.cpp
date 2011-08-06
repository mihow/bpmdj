/****
 BpmDj v4.2-pl2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

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
#ifndef __loaded__memory_cpp__
#define __loaded__memory_cpp__
using namespace std;
#include <cstdio>
#include <stdlib.h> // # 1033
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

void* myalloc(int length, const char* file, int line)
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
#endif // __loaded__memory_cpp__
