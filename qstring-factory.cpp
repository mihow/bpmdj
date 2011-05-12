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
#include "qstring-factory.h"

const QString TRUE_TEXT("Yes");
const QString FALSE_TEXT("No");
const QString TAG_TRUE("Yes"); 
const QString TAG_FALSE("___");
const QString zero("0");
const QString zeroo("00");
const QString slash("/");
const QString EMPTY("");
const QString ANAL_NOTNEC("___");
const QString ANAL_NEC("Missing");

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

set<QString*,PtrQStringLesser> QStringFactory::tree;
bool QStringFactory::killed = false;

QString QStringFactory::create(QString newly_allocated)
{
  if (killed) 
    return newly_allocated;
  if (newly_allocated.isEmpty())
    return QString::null;
  
  set<QString*,PtrQStringLesser>::iterator found;
  found = tree.find(&newly_allocated);
  if (found!=tree.end())
    return **found;
  QString * text = new QString(newly_allocated);
  tree.insert(text);
  return *text;
}

void QStringFactory::kill()
{
  killed=true;
}
