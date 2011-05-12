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
#ifndef __loaded__qstring_factory_cpp__
#define __loaded__qstring_factory_cpp__
using namespace std;
#line 1 "qstring-factory.c++"
#include <stdlib.h>
#include <stdio.h>
#include <set>
#include "constants.h"
#include "qstring-factory.h"

QString tonumber(const int b)
{
  return ( b < 10 ?
	   zeroo+QString::number(b) :
	   ( b < 100 ?
	     zero+QString::number(b) :
	     QString::number(b)));
}

QString tonumber(const float4 f)
{
  return QString::number(f);
}

set<QString> QStringFactory::tree;
bool QStringFactory::killed = false;
QString QStringFactory::create(QString newly_allocated)
{
  if (killed) 
    return newly_allocated;
  if (newly_allocated.isEmpty())
    return QString::null;
  
  set<QString>::iterator found;
  found = tree.find(newly_allocated);
  if (found!=tree.end()) return *found;
  tree.insert(newly_allocated);
  return newly_allocated;
}

void QStringFactory::kill()
{
  killed=true;
}
#endif // __loaded__qstring_factory_cpp__
