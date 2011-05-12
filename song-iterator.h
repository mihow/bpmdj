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
  int idx;
  bool once;
public:
  inline int key()
  {
    return idx;
  }
  inline Song* val()
  {
    return QSong::get_songs(idx);
  }
  inline bool valid()
  {
    return idx<QSong::get_song_count();
  }
  inline void skipNonValids()
  {
    while(valid() && !QSong::get_selected(idx))
      idx++;
  }
  inline void step()
  {
    idx++;
    skipNonValids();
  }
  inline bool prepare_block()
  {
    if (once) return false;
    once=true;
    skipNonValids();
    return valid();
  }
  inline void need_fields()
  {
  }
  inline void start()
  {
  }
  selectedSongIterator(): idx(0), once(false)
  {
  };
};
#endif // __loaded__song_iterator_h__
