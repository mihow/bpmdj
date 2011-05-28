/****
 BpmDj v4.2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

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
#ifndef __loaded__fragment_cache_h__
#define __loaded__fragment_cache_h__
using namespace std;
#line 1 "fragment-cache.h++"
#include <qstring.h>
#include <vector>
#include <map>
#include <set>
#include "bpmdj-event.h"
#include "song.h"
#include "do-fragment.h"

class FragmentCreated: public BpmDjEvent
{
  FragmentFile cnt;
public:
  bool isEmpty() 
  {
    return cnt.isEmpty();
  }
  FragmentCreated()
  {
  }
  FragmentCreated(FragmentFile f): cnt(f)
  {
  };
  Song* get_song()
  {
    return cnt.get_song();
  }
  FragmentInMemory getFragment()
  {
    return cnt.getFragment();
  }
  virtual void run();
};

class FragmentCache
{
  map<Song*,FragmentFile> song2fragment;
public:
  void get(Song* song);
};

extern FragmentCache fragmentCache;
#endif // __loaded__fragment_cache_h__
