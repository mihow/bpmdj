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
#ifndef __loaded__database_h__
#define __loaded__database_h__
using namespace std;
#line 1 "database.h++"
#include <vector>
#include <map>
#include <vector>
#include "qsong.h"

class Song;
class SongMetriek;
class SongSelectorLogic;
class QSong;

class DataBase
{
private:
  /**
   * The cache is the collection of songs that fall within the selected tags. 
   * This is a very important improvement in the speed of BpmDj since 
   * we often reduce the large song collection to a certain style.
   */
  vector<Song*> cache;
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
  bool filter(SongSelectorLogic* selector, Song* song, 
	      Song* main, float4 limit);
protected: 
  vector<Song*> all;
  map<QString,Song*> file2song;
  virtual void init();
  virtual void clear();
private:
  int get_unheaped_selection(SongSelectorLogic* selector, Song* main, 
			     QSong* target);
  /**
   * Will update the target QSong by calling setVector (which in turn
   * will sort the list and clear the selection). The show argument will be 
   * owned by the vectorview afterward. Before changing the songlist, the 
   * current item will be determined and reset after we changed the songlist.
   */
  signed4 set_answer(Song ** show, int length, QSong* target);
  bool     rebuild_cache;
public:
  void reset();
  void updateCache(SongSelectorLogic * selector);
  Song *   find(QString song_filename);
  const vector<Song*>& getAllSongs() 
  {
    return all;
  };
  /**
   * Will return the Song* object based on the mp3 filename
   */
  Song* file_to_song(QString file)
  {
    map<QString,Song*>::iterator sit;
    sit=file2song.find(file);
    if (sit!=file2song.end())
      return sit->second;
    return NULL;
  }
  map<QString,Song*> getFileTreeCopy()
  {
    return file2song;
  }
  DataBase();
  virtual ~DataBase();
  virtual void add(Song*);
  void flush_cache()
  {
    rebuild_cache = true;
  };
  int getSelection(SongSelectorLogic* selector, 
		   QSong* target, 
		   int nr  // if nr == 0 we will return an unheaped selection
		   );
  void addNewSongs(SongSelectorLogic* selector,  // for the search line
		   QSong* target,          // to schedule a repaint
		   vector<Song*> *collection);
  Song * * closestSongs(SongSelectorLogic * selector,
			Song * target1, float4 weight1, 
			Song * target2, float4 weight2, 
			SongMetriek * metriek, 
			int maximum, int &count);
};
#endif // __loaded__database_h__
