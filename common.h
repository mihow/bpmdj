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
#ifndef __loaded__common_h__
#define __loaded__common_h__
using namespace std;
#include <stdio.h>
#include <qstring.h>
#include <string.h>
#include "flowdb.h"

void   common_init();

//bool   exists(const char* fn)
bool   exists(QString str);

float8 minimum(float8 a, float8 b);
float8 abs_minimum(float8 a, float8 b);
int    clip(int val);
inline bool option(char* arg, const char* name) 
{
  return strcmp(arg,name)==0; 
};

inline bool option(char* arg, const char* namea, const char* nameb)
{
  return (strcmp(arg,namea)==0 || strcmp(arg,nameb)==0); 
};

QString readable(unsigned8);
#endif // __loaded__common_h__
