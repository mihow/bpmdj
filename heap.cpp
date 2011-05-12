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
  setIterator<Song*,SongDistComparator> song(this); ITERATE_OVER(song)
    cur[0]=song.val();
    cur++;
  }
  return cur-target;
}
