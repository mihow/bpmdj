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

#ifndef PIXMAP_CACHE_H
#define PIXMAP_CACHE_H

#include <qintcache.h>
#include <qpixmap.h>
#include "song.h"

/**
 * This pixmap cache allows the songselector to create pixmaps
 * and store them for later reference. The cache automatically deletes
 * pixmaps which are no longer necessary.
 */
class QPixmapCache: private QIntCache<QPixmap>
{
  private:
    int width;
    int height;
    Song * main;
    void insert(Song* a, QPixmap * pm);
  public:
    QPixmapCache();
    void insert(Song* a, const QPixmap & pm);
    void remove(Song* a);
    const QPixmap find(Song* a, Song* m, int w, int h);
};

#endif
