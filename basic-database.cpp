/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

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

#include <qlabel.h>
#include <pthread.h>
#include "history.h"
#include "database.h"
#include "song-metric.h"
#include "qsong.h"
#include "songselector.logic.h"
#include "process-manager.h"
#include "songtree.h"
#include "avltree.h"
#include "growing-array.h"
#include "tags.h"
#include "heap.h"
#include "song-statistics.h"
#include "kbpm-dj.h"

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
  all.reset();
  file_tree = new AvlTree<QString>();
}

void BasicDataBase::clear()
{
  for(int i = 0 ; i < all.count ; i ++)
    delete all.elements[i];
  all.reset();
  delete file_tree;
}

void BasicDataBase::add(Song* song)
{
  all.add(song);
  if (file_tree->search(song->get_file()))
    {
      printf("Fatal: The song %s occurs at least two times in different index files\n",(const char*)song->get_file());
      printf("The first index file is %s\n",(const char*)song->get_storedin());
      song = ((SongSortedByFile*)(file_tree->search(song->get_file())))->song;
      printf("The second index file is %s\n",(const char*)song->get_storedin());
      _exit(0);
    }
  file_tree->add(new SongSortedByFile(song));
}

Song * BasicDataBase::find(QString song_filename)
{
  SongSortedByFile* ssbf = (SongSortedByFile*)file_tree->search(song_filename);
  if (!ssbf) return NULL;
  return ssbf->song;
}

AvlTree<QString>* BasicDataBase::getFileTreeCopy()
{
  AvlTree<QString> * file_tree = new AvlTree<QString>();
  for(int i = 0 ; i < all.count ; i ++)
    if (!file_tree->search(all.elements[i]->get_file()))
      file_tree->add(new SongSortedByFile(all.elements[i]));
  return file_tree;
}

AvlTree<QString>* BasicDataBase::getFileTreeRef()
{
  return file_tree;
}
