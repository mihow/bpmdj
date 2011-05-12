/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle

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

#include "pixmap-cache.h"

PixmapCache::PixmapCache() : QIntCache<QPixmap>()
{
  main = NULL;
  width = -1;
  height = -1;
  setAutoDelete(true);
}

QPixmap * PixmapCache::insert(Song* a, QPixmap * pm)
{
  QIntCache<QPixmap>::insert((int)(void*)a,pm);
  return pm;
}

void PixmapCache::remove(Song * a)
{
  QIntCache<QPixmap>::remove((int)a);
}

QPixmap *PixmapCache::find(Song* a, Song* m, int w, int h)
{
  if (w!=width || h!=height || m!=main)
    {
      QIntCacheIterator<QPixmap> it(*this);
      while (!it.isEmpty())
      	QIntCache<QPixmap>::remove(it.currentKey());
      main = m;
      width = w;
      height = h;
    }
  QPixmap * answer = QIntCache<QPixmap>::find((int)a);
  if (answer) return answer;
  return insert(a,new QPixmap());
}
