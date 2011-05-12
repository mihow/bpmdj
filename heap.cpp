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
#ifndef __loaded__heap_cpp__
#define __loaded__heap_cpp__
using namespace std;
#line 1 "heap.c++"
#include "set-iterator.h"
#include "song.h"
#include "heap.h"

void SongHeap::add(Song *s)
{
  // is there space ?
  assert(s);
  if (size<maxsize)
    {
      insert(s);
      size++;
      return;
    }
  // no space. Should it be added ?
  if (s->get_color_distance() >= maximum)
    return;
  insert(s);
  // remove the last
  erase(--end());
  // update the maximum last value
  Song * last = *--end();
  maximum = last -> get_color_distance();
}

int SongHeap::copy_to(Song** target)
{
  Song** cur=target;
  setIterator<Song*,SongDistComparator> song(this);
ITERATE_OVER(song)

    cur[0]=song.val();
    cur++;
  }
  return cur-target;
}
#endif // __loaded__heap_cpp__
