/****
 BpmDj v3.6: Free Dj Tools
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
