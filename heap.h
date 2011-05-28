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
#ifndef __loaded__heap_h__
#define __loaded__heap_h__
using namespace std;
#line 1 "heap.h++"
#include <assert.h>
#include <set>

class Song;
class SongDistComparator
{
public: 
  bool operator()(Song* song, Song* other) const
  {
    /**
     * Important !
     * The order in which we compare the distances is important because
     * if we compare A with B we might get that A is smaller than B but
     * if we compare B with A we don't get that B is bigger than A because
     * the equality was checked first !
     */      
    unsigned4 a = (unsigned4)(song->get_color_distance() * 1000000.0);
    unsigned4 b = (unsigned4)(other->get_color_distance() * 1000000.0);
    if ( a < b ) return -1;
    if ( a > b ) return 1;
    if (song<other) return -1;
    if (song==other) return 0;
    return 1;
  }
};

/**
 * This heap contains a maximum number of songs
 * When there are too many songs, the largest ones are removed.
 */
class SongHeap: set<Song*,SongDistComparator>
{
  int maxsize;
 public:
  float4 maximum;
  int size;
  /**
   * Initializes the heap to a maximum of @arg nr songs.
   */
  SongHeap(int nr)
    {
      maxsize = nr;
      maximum = 1000000;
      size = 0;
    }
  /**
   * adds the song. If full the largest one is removed.
   */
  void add(Song * s);
  /**
   * Will copy all the songs in order to the target array.
   */
  int copy_to(Song** target);
};
#endif // __loaded__heap_h__
