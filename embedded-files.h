/****
 BpmDj v4.0: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.
****/
#ifndef __loaded__embedded_files_h__
#define __loaded__embedded_files_h__
using namespace std;
#line 1 "embedded-files.h++"
/**
 * the pointers refers to files that are embedded as object files
 * before they are usefull init_embedded_files should be called
 */
void init_embedded_files();

#define EMBEDDED_FILES \
FILE(logo_png)  \
FILE(bpmdj_ogg) \
FILE(bpmdj_mp3)

#define FILE(name) \
  extern         int name## _size; \
  extern const char* name;
EMBEDDED_FILES
#undef FILE
#endif // __loaded__embedded_files_h__
