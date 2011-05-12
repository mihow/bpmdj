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

#include "song.h"
#include "heap.h"
#include "avltree.h"

class SongWithDistance: public Node<Song*>
{
public:
  Song * song;
  SongWithDistance(Song *s): Node<Song*>()
  {
    song = s;
  }
  void print() 
  {
    printf(" %s : %g",(const char*)song->getDisplayTitle(),song->get_color_distance());
  };
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
  /**
   * Important !
   * The order in which we compare the distances is important because
   * if we compare A with B we might get that A is smaller than B but
   * if we compare B with A we don't get that B is bigger than A because
   * the equality was checked first !
   */

  /**
   * Well even ordering the thing didn't work out too well. SO a convertion 
   * to an integer will save us here...
   */ 

  unsigned4 a = (unsigned4)(song->get_color_distance() * 1000000.0);
  unsigned4 b = (unsigned4)(other->get_color_distance() * 1000000.0);
  if ( a < b ) return -1;
  if ( a > b ) return 1;
  if (song<other) return -1;
  if (song==other) return 0;
  return 1;
}

void SongHeap::add(Song *s)
{
  assert(s);
  // is there space ?
  if (size<maxsize)
    {
      AvlTree<Song*>::add(new SongWithDistance(s));
      size ++;
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
  assert(!last->isNull());
  //  printf("Shifting out %g: %s\n",(double)last->song->get_color_distance(), (const char*)last->song->getDisplayTitle());
  del(last->song);
  // update the maximum last value
  last = cur = (SongWithDistance*)top();
  assert(cur);
  while(! cur -> isNull() )
    {
      last = cur;
      cur = (SongWithDistance*) cur -> right;
    }
  assert(last);
  maximum = last -> song -> get_color_distance();
  //  printf("  Maximum is now %g: %s\n",maximum,(const char*)last->song->getDisplayTitle());
}

static int copy_to(Song** &target, SongWithDistance * who)
{
  assert(who!= NULL);
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

