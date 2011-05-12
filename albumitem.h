/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

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

#include "config.h"
#include "cluster.h"
#include "data.h"

#define ALBUM_NR 0
#define ALBUM_TITLE 1
#define ALBUM_AUTHOR 2
#define ALBUM_TIME 3
#define ALBUM_INDEX 4
#define ALBUM_FILE 5

class AlbumItem: 
  public QListViewItem
{
  accessors(Song*, song, NULL);
 public:
  AlbumItem(const char* name, QListView * parent);
  AlbumItem(int nr, Song *song, QListViewItem * parent);
  //    virtual void paintCell(QPainter *p, const QColorGroup &cg, int col, int wid, int align);
  // virtual QString text(int i) const;
  void fixNr();
};
