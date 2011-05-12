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

#ifndef DATABASE_H
#define DATABASE_H

#include "basic-database.h"
#include "avltree.h"
#include "growing-array.h"

class Song;
class SongMetriek;
class SongSelectorLogic;
class QVectorView;

class DataBase: public BasicDataBase
{
  private:
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
 protected: 
    virtual void init();
    virtual void clear();
 private:
    int      get_unheaped_selection(SongSelectorLogic* selector, Song* main, QVectorView* target);
    int      set_answer(Song * * show, int length, QVectorView* target);
    bool     rebuild_cache;
 public:
    DataBase();
    virtual ~DataBase();
    void     flush_cache() 
      {
	rebuild_cache = true;
      };
    void     start_existence_check();
    int      getSelection(SongSelectorLogic* selector, Song* main, QVectorView* target, int nr = 0);
    Song * * closestSongs(SongSelectorLogic * selector,Song * target1, float weight1, Song * target2, float weight2, SongMetriek * metriek, int maximum, int &count);
};

#endif

