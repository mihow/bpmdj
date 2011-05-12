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
#ifndef __loaded__memory_h__
#define __loaded__memory_h__
using namespace std;
#line 1 "memory.h++"
#define bpmdj_allocate(size, type) (type*)myalloc(sizeof(type)*(size), __FILE__, __LINE__)
#define bpmdj_array(name,size,type) type * name = bpmdj_allocate(size,type)
#define bpmdj_reallocate(thing, size, type) (type*)myrealloc(thing,sizeof(type)*(size))
#define bpmdj_deallocate(thing) myfree(thing);

void * myalloc(int size, char* file, int line);
void * myrealloc(void* thing, int size);
void   myfree(void*);
 
#endif // __loaded__memory_h__
