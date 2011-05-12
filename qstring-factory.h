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
#ifndef __loaded__qstring_factory_h__
#define __loaded__qstring_factory_h__
using namespace std;
#line 1 "qstring-factory.h++"
#include <qstring.h>
#include <set>
#include "types.h"

QString tonumber(const int b);
QString tonumber(const float4 f);

class QStringFactory
{
private:
  static bool killed;
  static set<QString> tree;
public:
  static QString create(QString str);
  static void kill();
};
#endif // __loaded__qstring_factory_h__
