/****
 BpmDj v3.6: Free Dj Tools
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
#ifndef __loaded__song_copier_cpp__
#define __loaded__song_copier_cpp__
using namespace std;
#line 1 "song-copier.c++"
#include <qdir.h>
#include "scripts.h"
#include "song-copier.h"

elementResult ActiveSongCopier::setTarget(QString trgt)
{
  target=trgt;
  return Done;
};

elementResult ActiveSongCopier::fetch(Song* song)
{
  if (tocopy.size()==0)
    queue_fetchNextSong();
  tocopy.push_back(song);
  return Done;
};

elementResult ActiveSongCopier::fetchNextSong()
{
  if (tocopy.size()) 
    {
      Song* s = tocopy.front();
      tocopy.pop_front();
      QString from = "./music/"+s->get_file();
      QString to = target+"/"+s->get_file();
      // does the directory exist ?
      QString todir = to.left(to.lastIndexOf("/"));
      if (!QDir(todir).exists())
	start_mkdir(todir);
      // does the target file already exist ?
      start_cp(from,to);
      queue_fetchNextSong();
    }
  return Done;
} 

elementResult ActiveSongCopier::terminate()
{
  tocopy.clear();
  deactivate();
  return Done;
}

SongCopier songCopier;
#endif // __loaded__song_copier_cpp__
