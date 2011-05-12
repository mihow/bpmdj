/****
 BpmDj: Free Dj Tools
 Copyright (C) 1995-2005 Werner Van Belle
 See 'BeatMixing.ps' for more information

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

#include "song.h"
#include "heap.h"
#include "avltree.cpp"

class SongWithDistance: public Node<Song*>
{
public:
  Song * song;
  SongWithDistance(Song *s): Node<Song*>()
  {
    song = s;
  }
  int SongWithDistance::compareSearchData(Song* w);
  int SongWithDistance::compareAddData(Node<Song*>* n);
};

int SongWithDistance::compareAddData(Node<Song*>* n)
{
  SongWithDistance *othernode = (SongWithDistance*)n;
  return compareSearchData(othernode->song);
}

int SongWithDistance::compareSearchData(Song* other)
{
  if (song->get_color_distance() < other->get_color_distance()) return -1;
  if (song->get_color_distance() == other->get_color_distance()) 
    {
      if (song<other) return -1;
      if (song==other) return 0;
    }
  return 1;
}

void SongHeap::add(Song *s)
{
  assert(s);
  // is there space ?
  if (count<maxsize)
    {
      AvlTree<Song*>::add(new SongWithDistance(s));
      count ++;
      return;
    }
  // does it not belong in the set ?
  if (s->get_color_distance() >= maximum)
    return;
  // no, then add it
  AvlTree<Song*>::add(new SongWithDistance(s));
  // remove the last
  SongWithDistance *cur = (SongWithDistance*)top();
  assert(cur);
  SongWithDistance *last = cur;
  while(!cur -> isNull())
    {
      last = cur;
      cur = (SongWithDistance*) cur -> right;
    }
  assert(last);
  assert(last->song);
  del(last->song);
  // update the maximum last value
  last = cur = (SongWithDistance*)top();
  assert(cur);
  while(!cur -> isNull() )
    {
      last = cur;
      cur = (SongWithDistance*) cur -> right;
    }
  assert(last);
  maximum = last -> song -> get_color_distance();
}

static int copy_to(Song** &target, SongWithDistance * who)
{
  if (who->isNull()) return 0;
  int count = copy_to(target,(SongWithDistance*)who->left);
  target[0] = who->song;
  target++;
  count = count + 1 + copy_to(target,(SongWithDistance*)who->right);
  return count;
}

int SongHeap::copy_to(Song** target)
{
  SongWithDistance *d = (SongWithDistance*)top();
  if (d==NULL || d->isNull()) return 0;
  return ::copy_to(target,d);
}

