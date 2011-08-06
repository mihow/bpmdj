/****
 BpmDj v4.2-pl2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

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
#ifndef __loaded__albumitem_cpp__
#define __loaded__albumitem_cpp__
using namespace std;
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "albumitem.h"
#include "qsong.h"
#include "dirscanner.h"
#include "qstring-factory.h"

AlbumItem::AlbumItem(QString name, QTreeWidget* parent) :
  TreeWidgetSong(NULL,parent)
{
  setText(ALBUM_NR,name);
  setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

AlbumItem::AlbumItem(unsigned1 nr, Song * s, AlbumItem* parent) :
  TreeWidgetSong(s,parent)
{
  setNr(nr);
  init();
  setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

void AlbumItem::init()
{
  if (!song)
    {
      setText(ALBUM_TITLE,EMPTY);
      setText(ALBUM_AUTHOR,EMPTY);
      setText(ALBUM_TIME,EMPTY);
      setText(ALBUM_INDEX,EMPTY);
      setText(ALBUM_FILE,EMPTY);
    }
  else
    {
      setText(ALBUM_TITLE,song->get_title());
      setText(ALBUM_AUTHOR,song->get_author());
      setText(ALBUM_TIME,song->get_time());
      setText(ALBUM_INDEX,song->get_storedin());
      setText(ALBUM_FILE,song->get_file());
    }
}

void AlbumItem::setNr(unsigned1 nr)
{
  setText(ALBUM_NR,tonumber((unsigned4)nr));
}
#endif // __loaded__albumitem_cpp__
