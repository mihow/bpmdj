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
#ifndef __loaded__albumitem_h__
#define __loaded__albumitem_h__
using namespace std;
#line 1 "albumitem.h++"
#include <Qt/q3listview.h>
#include "config.h"
#include "cluster.h"
#include "data.h"

#define ALBUM_NR 0
#define ALBUM_TITLE 1
#define ALBUM_AUTHOR 2
#define ALBUM_TIME 3
#define ALBUM_INDEX 4
#define ALBUM_FILE 5

class AlbumItem: public Q3ListViewItem
{
  accessors(Song*, song, NULL);
 public:
  AlbumItem(const char* name, Q3ListView * parent);
  AlbumItem(int nr, Song *song, Q3ListViewItem * parent);
  void fixNr();
};
#endif // __loaded__albumitem_h__
