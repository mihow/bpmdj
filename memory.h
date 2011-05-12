/****
 BpmDj v4.2 beta: Free Dj Tools
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
#ifndef __loaded__memory_h__
#define __loaded__memory_h__
using namespace std;
#line 1 "memory.h++"
#define bpmdj_allocate(size, type) \
  (type*)myalloc(sizeof(type)*(size), __FILE__, __LINE__)
#define bpmdj_array(name,size,type) \
  type * name = bpmdj_allocate(size,type)
#define bpmdj_reallocate(thing, size, type) \
  (type*)myrealloc(thing,sizeof(type)*(size))
#define bpmdj_deallocate(thing) myfree(thing);

void * myalloc(int size, const char* file, int line);
void * myrealloc(void* thing, int size);
void   myfree(void*);
 
#endif // __loaded__memory_h__
