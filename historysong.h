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
#ifndef __loaded__historysong_h__
#define __loaded__historysong_h__
using namespace std;
#line 1 "historysong.h++"
#include "config.h"
#include "cluster.h"

class HistorySong: 
  public Q3ListViewItem
{
 private:
  Song * song;
  QString relation;
  QString comment;
 public:
  HistorySong(Song *song, QString relation, 
	      QString comment, Q3ListView* parent);
  HistorySong(Q3ListView* parent, Q3ListViewItem* after);
  Song * getSong() {return song;};
  virtual void paintCell(QPainter *p, const QColorGroup &cg, 
			 int col, int wid, int align);
  virtual QString text(int i) const;
};
#endif // __loaded__historysong_h__
