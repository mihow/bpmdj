/****
 BpmDj v3.8: Free Dj Tools
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
#ifndef __loaded__pixmap_cache_cpp__
#define __loaded__pixmap_cache_cpp__
using namespace std;
#line 1 "pixmap-cache.c++"
#include "pixmap-cache.h"

PixmapCache::PixmapCache() : QCache<intptr_t,QPixmap>()
{
  main = NULL;
  width = -1;
  height = -1;
}

QPixmap * PixmapCache::insert(Song* a, QPixmap * pm)
{
  QCache<intptr_t,QPixmap>::insert((intptr_t)(void*)a,pm);
  return pm;
}

void PixmapCache::remove(Song * a)
{
  QCache<intptr_t,QPixmap>::remove((intptr_t)a);
}

QPixmap *PixmapCache::find(Song* a, Song* m, int w, int h)
{
  if (w!=width || h!=height || m!=main)
    {
      QCache<intptr_t,QPixmap>::clear();
      main = m;
      width = w;
      height = h;
    }
  QPixmap * answer = (*this)[((intptr_t)a)];
  if (answer) return answer;
  return insert(a,new QPixmap());
}
#endif // __loaded__pixmap_cache_cpp__
