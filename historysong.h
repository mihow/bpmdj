/****
 BpmDj v4.2 beta: Free Dj Tools
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
#ifndef __loaded__historysong_h__
#define __loaded__historysong_h__
using namespace std;
#line 1 "historysong.h++"
#include <qtreewidget.h>
#include "config.h"
#include "cluster.h"

class TreeWidgetSong: public QTreeWidgetItem
{
protected:
  Song * song;
public:
  TreeWidgetSong(Song *_, QTreeWidget* parent): QTreeWidgetItem(parent), song(_)
  {    
  };
  TreeWidgetSong(Song *_, QTreeWidgetItem* after): QTreeWidgetItem(after), song(_)
  {
  };
  TreeWidgetSong(Song *_, QTreeWidget* parent, QTreeWidgetItem* after): QTreeWidgetItem(parent,after), song(_)
  {
  };
  inline Song *getSong() 
  {
    return song;
  };
};

class HistorySong: public TreeWidgetSong
{
  QString relation;
  QString comment;
 public:
  HistorySong(Song *song, QString relation, 
	      QString comment, QTreeWidget* parent);
  HistorySong(QTreeWidget* parent, QTreeWidgetItem* after);
};
#endif // __loaded__historysong_h__
