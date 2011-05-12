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

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
typedef signed   long long  int signed8;
typedef unsigned long long  int unsigned8;
typedef signed        long  int signed4;
typedef unsigned      long  int unsigned4;
typedef signed        short int signed2;
typedef unsigned      short int unsigned2;

typedef union
{
  unsigned4 value;
  struct 
  {
    signed short int left;
    signed short int right;
  } leftright;
} longtrick;

#define allocate(size, type) (type*)myalloc(sizeof(type)*(size), __FILE__, __LINE__)
#define array(name,size,type) type * name = allocate(size,type)
#define reallocate(thing, size, type) (type*)myrealloc(thing,sizeof(type)*(size))
void  common_init();
void* myalloc(int size, char* file, int line);
void* myrealloc(void* thing, int size);
long  fsize(FILE * f);
long  readsamples(void* target, int count, FILE* file);
long  writesamples(void* target, int count, FILE* file);
#endif
