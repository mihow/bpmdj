/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

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
#ifndef __BPMDJ___FRAGMENT_CACHE_H__
#define __BPMDJ___FRAGMENT_CACHE_H__
using namespace std;
#line 1 "fragment-cache.h++"
#include <qstring.h>
#include <vector>
#include <map>
#include <set>
#include "bpmdj-event.h"
#include "song.h"
#include "fragment.h"

class FragmentCreated: public BpmDjEvent
{
  QString f;
public:
  Song* song;
  bool isEmpty() 
  {
    return f.isEmpty();
  }
  FragmentCreated(): f(""), song(NULL)
  {
  }
  FragmentCreated(Song *s, QString f=""): f(f), song(s)
  {
  };
  Fragment getFragment();
  virtual void run(SongSelectorLogic * song_selector_window);
};

class FragmentCache
{
  map<Song*,FragmentCreated> song2fragment;
public:
  void get(Song* song);
};

extern FragmentCache fragmentCache;
#endif
