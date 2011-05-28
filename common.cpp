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
#ifndef __loaded__common_cpp__
#define __loaded__common_cpp__
using namespace std;
#line 1 "common.c++"
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "dirscanner.h"
#include "common.h"
#include "memory.h"

void common_init()
{
   assert(sizeof(int)==4);
   assert(sizeof(signed2)==2);
   assert(sizeof(signed4)==4);
   assert(sizeof(signed8)==8);
   assert(sizeof(float8)==8);
   assert(sizeof(float4)==4);
}

bool exists(QString fn)
{
  FILE * f = fopen(fn.toAscii().data(),"rb");
  if (f)
    {
      fclose(f);
      return true;
    }
  return false;
}

float8 minimum(float8 a, float8 b)
{
  if ( a < b ) return a;
  else return b;
}

float8 abs_minimum(float8 a, float8 b)
{
  if ( fabs(a) < fabs(b) ) return a;
  else return b;
}

int clip(int val)
{
  if (val<0) return -1;
  if (val>0) return +1;
  return 0;
}

QString readable(unsigned8 L)
{
  if (L < 1024) return QString::number(L)+" bytes";
  L/=1024;
  if (L < 1024) return QString::number(L)+" kilobytes";
  L/=1024;
  if (L < 1024) return QString::number(L)+" megabytes";
  L/=1024;
  if (L < 1024) return QString::number(L)+" gigabytes";
  L/=1024;
  return QString::number(L)+" terabytes";
}

#endif // __loaded__common_cpp__
