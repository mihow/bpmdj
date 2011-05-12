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
#ifndef __loaded__albumitem_h__
#define __loaded__albumitem_h__
using namespace std;
#line 1 "albumitem.h++"
#include "historysong.h"

const unsigned4 ALBUM_NR=0;
const unsigned4 ALBUM_TITLE=1;
const unsigned4 ALBUM_AUTHOR=2;
const unsigned4 ALBUM_TIME=3;
const unsigned4 ALBUM_INDEX=4;
const unsigned4 ALBUM_FILE=5;

class AlbumItem: public TreeWidgetSong
{
public:
  AlbumItem(QString name, QTreeWidget * parent);
  AlbumItem(unsigned1 nr, Song *song, AlbumItem* parent);
  void init();
  void setNr(unsigned1 nr);
};
#endif // __loaded__albumitem_h__
