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
#line 1 "basic-database.c++"
#include <qlabel.h>
#include <pthread.h>
#include "history.h"
#include "database.h"
#include "song-metric.h"
#include "qsong.h"
#include "songselector.logic.h"
#include "process-manager.h"
#include "tags.h"
#include "heap.h"
#include "song-statistics.h"
#include "kbpm-dj.h"
#include "vector-iterator.h"

BasicDataBase::BasicDataBase() : all()
{
  init();
}

BasicDataBase::~BasicDataBase()
{
  clear();
}

void BasicDataBase::reset()
{
  clear();
  init();
}

void BasicDataBase::init()
{
  all.clear();
  file2song = map<QString,Song*>();
}

void BasicDataBase::clear()
{
  vectorIterator<Song*> i(all); ITERATE_OVER(i) delete i.val(); }
  all.clear();
  file2song = map<QString,Song*>();
}

void BasicDataBase::add(Song* song)
{
  all.push_back(song);
  if (file2song.find(song->get_file())!=file2song.end())
    {
      printf("Fatal: The song %s occurs at least two times in different index files\n",
	     (const char*)song->get_file());
      printf("The first index file is %s\n",(const char*)song->get_storedin());
      song = file2song[song->get_file()];
      printf("The second index file is %s\n",(const char*)song->get_storedin());
      _exit(0);
    }
  file2song[song->get_file()]=song;
}

Song * BasicDataBase::find(QString song_filename)
{
  map<QString,Song*>::iterator ssbf;
  ssbf = file2song.find(song_filename);
  if (ssbf==file2song.end())
    return NULL;
  return ssbf->second;
}
