/****
 BpmDj: Free Dj Tools
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

QString tonumber(const float f)
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
