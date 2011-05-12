/****
 BpmDj v3.8: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

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
#ifndef __loaded__database_h__
#define __loaded__database_h__
using namespace std;
#line 1 "database.h++"
#include <vector>
#include <map>
#include <vector>

class Song;
class SongMetriek;
class SongSelectorLogic;
class QVectorView;

class DataBase
{
private:
  /**
   * The cache is the collection of songs that fall within the selected tags. This is
   * a very important improvement in the speed of bpmdj since 
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
  bool filter(SongSelectorLogic* selector, Song* song, Song* main, float4 limit);
protected: 
  vector<Song*> all;
  map<QString,Song*> file2song;
  virtual void init();
  virtual void clear();
private:
  int      get_unheaped_selection(SongSelectorLogic* selector, Song* main, QVectorView* target);
  /**
   * Will update the target QVectorView by calling setVector (which in turn will sort the list
   * and clear the selection). The show argument will be owned by the vectorview afterwards.
   * Before changing the songilst, the current item will be determined and reset after we 
   * changed the songlist.
   */
  int      set_answer(Song ** show, int length, QVectorView* target);
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
		   QVectorView* target, 
		   int nr  // if nr == 0 we will return an ungheaped selection
		   );
  void addNewSongs(SongSelectorLogic* selector,  // necessary for the searchline
		   QVectorView* target,          // necessary to schedule a repaint
		   vector<Song*> *collection);
  Song * * closestSongs(SongSelectorLogic * selector,
			Song * target1, float4 weight1, 
			Song * target2, float4 weight2, 
			SongMetriek * metriek, 
			int maximum, int &count);
};
#endif // __loaded__database_h__
