/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle
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
#include <string.h>

typedef signed   long long  int signed8;
typedef unsigned long long  int unsigned8;
typedef signed        long  int signed4;
typedef unsigned      long  int unsigned4;
typedef signed        short int signed2;
typedef unsigned      short int unsigned2;
typedef                   float float4;
typedef                  double float8;

typedef union
{
  unsigned4 value;
  struct 
  {
    signed short int left;
    signed short int right;
  } leftright;
} longtrick;

// spectrum typering
const             int   spectrum_size=24;
typedef         float   spectrum_freq;
typedef spectrum_freq * spectrum_type;
const   spectrum_type   no_spectrum = NULL;
#define                 allocate_spectrum() allocate(spectrum_size,float)
// tag typering
typedef   signed char   tag_type;
typedef      tag_type * tags_type;
const        tag_type   tag_end = 0;

void   common_init();
void * myalloc(int size, char* file, int line);
void * myrealloc(void* thing, int size);
void   myfree(void*);
long   fsize(FILE * f);
long   readsamples(void* target, int count, FILE* file);
long   writesamples(void* target, int count, FILE* file);
bool   strxeq(const char* a, const char* b); // also works when a or b or NULL
char * findUniqueName(const char* filename); // will prefix the extention of filename with a number until a unique filename is found
bool   exists(const char* fn);
double minimum(double a, double b);
double abs_minimum(double a, double b);
double atod(const char* str);
char * tohex(long i);
long   toint(const char* name);
int    clip(int val);
void file_long(long i, FILE * f);
inline bool option(char* arg, char* name) { return strcmp(arg,name)==0; };
inline bool option(char* arg, char* namea, char* nameb) { return (strcmp(arg,namea)==0 || strcmp(arg,nameb)==0); };
#endif
