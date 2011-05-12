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

bool exists(const char* fn)
{
  FILE * f = fopen(fn,"rb");
  if (f)
    {
      fclose(f);
      return true;
    }
  return false;
}

double minimum(double a, double b)
{
  if ( a < b ) return a;
  else return b;
}

double abs_minimum(double a, double b)
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
  return QString::number(L)+" terrabytes";
}

#endif // __loaded__common_cpp__
