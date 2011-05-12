/****
 BpmDj v4.1: Free Dj Tools
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
#ifndef __loaded__song_iterator_h__
#define __loaded__song_iterator_h__
using namespace std;
#line 1 "song-iterator.h++"
#include <vector>
#include <assert.h>
#include "qsong.h"
#include "iterator.h"

class selectedSongIterator
{
  unsigned4 idx;
  bool once;
  vector<Song*> tolookat;
public:
  inline int key()
  {
    return idx;
  }
  inline Song* val()
  {
    return tolookat[idx];
  }
  inline bool valid()
  {
    return idx<tolookat.size();
  }
  inline void step()
  {
    idx++;
  }
  inline bool prepare_block()
  {
    if (once) return false;
    once=true;
    return valid();
  }
  inline void need_fields()
  {
  }
  inline void start()
  {
  }
  selectedSongIterator(QTableView* in=NULL): idx(0), once(false)
  {
    if (!in)
      in=selector->songListView;
    assert(in);
    QItemSelectionModel* sm=in->selectionModel();
    assert(sm);
    QModelIndexList all=sm->selectedRows(0);
    set<int> toretrieve;
    for(int i = 0 ; i < all.size(); i++)
      toretrieve.insert(all[i].row());
    tolookat.clear();
    for(set<int>::iterator it=toretrieve.begin(); it!=toretrieve.end(); it++)
      tolookat.push_back(QSong::songEssence(*it));
  };
};
#endif // __loaded__song_iterator_h__
