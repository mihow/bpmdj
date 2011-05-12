/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001 Werner Van Belle
 See 'BeatMixing.ps' for more information

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

class Song;
class SongMetriek;
class SongSelectorLogic;
class QListView;

class DataBase
{
  private:
    Song * *    all;
    int         all_size;
    int         all_count;
    Song * *    cache;
    int         cache_size;
    int         cache_count;
    bool *      and_include;
    bool *      or_include;
    bool        and_includes_checked;
    bool        excludes_checked;
    bool *      exclude;
    QString *   tag;
    int         tag_size;
    void clearCache();
    void addToCache(Song*);
    bool cacheValid(SongSelectorLogic * selector);
    void copyTags(SongSelectorLogic * selector);
    bool tagFilter(Song*);
    void updateCache(SongSelectorLogic * selector);
    bool filter(SongSelectorLogic* selector, Song* song, Song* main);
 public:
    DataBase();
    void     add(Song*);
    void     del(Song*);
    // bool     lookfor(const QString z);
    int      getSelection(SongSelectorLogic* selector, Song* main, QListView* target);
    Song * * getAllSongs(int &cnt) {cnt=all_count; return all;};
    AvlTree<QString> * getFileTree();
    Song * * closestSongs(SongSelectorLogic * selector, Song * target, SongMetriek * metriek, int maximum, int &count);
};

#endif

