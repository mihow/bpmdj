/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle

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

#ifndef DATABASE_H
#define DATABASE_H

#include "avltree.h"
#include "growing-array.h"

class Song;
class SongMetriek;
class SongSelectorLogic;
class QVectorView;

class DataBase
{
  private:
    GrowingArray<Song*> all;
    GrowingArray<Song*> cache;
    bool *      and_include;
    bool *      or_include;
    bool        and_includes_checked;
    bool        excludes_checked;
    bool *      exclude;
    tag_type *  tag;
    int         tag_size;
    bool cacheValid(SongSelectorLogic * selector);
    void copyTags(SongSelectorLogic * selector);
    bool tagFilter(Song*);
    void updateCache(SongSelectorLogic * selector);
    bool filter(SongSelectorLogic* selector, Song* song, Song* main, float limit);
    AvlTree<QString>* file_tree;
    void     init();
    void     clear();
    int      get_unheaped_selection(SongSelectorLogic* selector, Song* main, QVectorView* target);
    int      set_answer(Song * * show, int length, QVectorView* target);
  public:
    DataBase();
    virtual ~DataBase();
    void     reset();
    void     add(Song*);
    // void     del(Song*);
    Song *   find(QString song_filename);
    void     start_existence_check();
   // bool     lookfor(const QString z);
    int      getSelection(SongSelectorLogic* selector, Song* main, QVectorView* target, int nr = 0);
    GrowingArray<Song*> * getAllSongs() { return &all;};
    AvlTree<QString> * getFileTreeRef();
    AvlTree<QString> * getFileTreeCopy();
    Song * * closestSongs(SongSelectorLogic * selector,Song * target1, float weight1, Song * target2, float weight2, SongMetriek * metriek, int maximum, int &count);
};

#endif

