/****
 BpmDj: Free Dj Tools
 Copyright (C) 1995-2006 Werner Van Belle

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

/**
 * This heap will only contaion a maximum amount of songs
 * As soon as a new song comes in the largest song is thrown 
 * out.
 */

#ifndef HEAP_H
#define HEAP_H
#include <assert.h>
#include "avltree.h"

class Song;
class SongHeap: private AvlTree<Song*>
{
  int maxsize;
 public:
  float maximum;
  int size;
  SongHeap(int nr)
    {
      maxsize = nr;
      maximum = 1000000;
      size = 0;
    }
  void add(Song * s);
  int copy_to(Song** target);
};
#endif
