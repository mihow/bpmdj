/****
 BpmDj v4.2: Free Dj Tools
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
#ifndef __loaded__embedded_files_cpp__
#define __loaded__embedded_files_cpp__
using namespace std;
#line 1 "embedded-files.c++"
#include "embedded-files.h"

/**
 * Here we preform the linkage to the objects
 */
#define FILE(name) \
  extern const char _binary_##name## _start; \
  extern const char _binary_##name## _end; \
  int name## _size; \
  const char * name;
EMBEDDED_FILES
#undef FILE

void init_embedded_files()
{
/**
 * here we map the object addresses to proper defined variables
 */
#define FILE(name) \
  name## _size = & _binary_##name## _end - & _binary_##name## _start; \
  name         = & _binary_##name## _start;
EMBEDDED_FILES
#undef FILE
}
#endif // __loaded__embedded_files_cpp__
