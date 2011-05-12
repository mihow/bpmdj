/****
 BpmDj v3.8: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

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
#ifndef __loaded__common_h__
#define __loaded__common_h__
using namespace std;
#line 1 "common.h++"
#include <stdio.h>
#include <Qt/qstring.h>
#include <string.h>
#include "types.h"

void   common_init();
bool   exists(const char* fn);
float8 minimum(float8 a, float8 b);
float8 abs_minimum(float8 a, float8 b);
int    clip(int val);
inline bool option(char* arg, const char* name) { return strcmp(arg,name)==0; };
inline bool option(char* arg, const char* namea, const char* nameb) { return (strcmp(arg,namea)==0 || strcmp(arg,nameb)==0); };
QString readable(unsigned8);
#endif // __loaded__common_h__
