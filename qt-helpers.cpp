/****
 BpmDj v4.2: Free Dj Tools
 Copyright (C) 2001-2010 Werner Van Belle

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
#ifndef __loaded__qt_helpers_cpp__
#define __loaded__qt_helpers_cpp__
using namespace std;
#line 1 "qt-helpers.c++"
#include "qt-helpers.h"
void selectAll(QTreeWidget* lst, bool yes)
{
  assert(lst);
  QTreeWidgetItemIterator it(lst);
  while(*it)
    {
      (*it)->setSelected(yes);
      it++;
    }
}

QColor mixColor(QColor a, float4 d, QColor b)
{
  float4 ar = a.red();
  float4 br = b.red();
  float4 ag = a.green();
  float4 bg = b.green();
  float4 ab = a.blue();
  float4 bb = b.blue();
  return QColor((int)(ar+d*(br-ar)),
		(int)(ag+d*(bg-ag)),
		(int)(ab+d*(bb-ab)));
}

void setBackgroundColor(QWidget* widget, QColor color)
{
  QPalette palette;
  palette.setColor(widget->backgroundRole(), color);
  widget->setPalette(palette);
}

QColor backgroundColor(QWidget* widget)
{
  assert(widget);
  return widget->palette().color(widget->backgroundRole());
}

float4 qtof4(QString str)
{
  return atof(str.toAscii().data());
}

float8 qtof8(QString str)
{
  return atof(str.toAscii().data());
}

signed4 qtos4(QString str)
{
  return atoi(str.toAscii().data());
}
#endif // __loaded__qt_helpers_cpp__
