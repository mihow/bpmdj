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

#ifndef BDATABASE_H
#define BDATABASE_H

#include "avltree.h"
#include "growing-array.h"

class Song;
class SongMetriek;
class SongSelectorLogic;
class QVectorView;

class BasicDataBase
{
  protected:
    GrowingArray<Song*> all;
    AvlTree<QString>* file_tree;
    virtual void init();
    virtual void clear();
  public:
    BasicDataBase();
    virtual ~BasicDataBase();
    void     reset();
    void     add(Song*);
    Song *   find(QString song_filename);
    GrowingArray<Song*> * getAllSongs() { return &all;};
    AvlTree<QString> * getFileTreeRef();
    AvlTree<QString> * getFileTreeCopy();
};

#endif

