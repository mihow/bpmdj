/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001 Werner Van Belle
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

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <assert.h>
#include <stdio.h>
#include <qlistview.h>
#include <qlabel.h>
#include <qprogressbar.h>
#include <qfile.h>
#include <qdatastream.h>
#include "index-reader.h"
#include "kbpm-dj.h"
#include "qsong.h"
#include "config.h"
#include "spectrum.h"
#include "database.h"
#include "songtree.h"
#include "avltree.cpp"

IndexReader::IndexReader(Loader *l, DataBase * db) : 
  DirectoryScanner(".idx"),
  tree()
{ 
  database = db;
  loader = l;
  total_files = 0;
  readCache();
  validateCache();
  writeCache();
  lastSpectrum();
}

void IndexReader::validateCache()
{
  // first we check all existing filenames
  loader->progressBar1->setTotalSteps(Config::file_count);
  scan(IndexDir,IndexDir);
  loader->progressBar1 -> setProgress(total_files);
  // then we remove all the filenames found in the tree but no longer on disk
  SongSortedByIndex * top;
  while( (top = (SongSortedByIndex*)tree.top()) )
    {
      database->del(top->song);
      tree.del(top->getKey());
    }
};

void IndexReader::recursing(const QString dirname)
{
  loader->readingDir->setText(dirname);
}

void IndexReader::checkfile(const QString prefix, const QString  filename)
{
  // If the index does not exist remove the song from the database
  QString fullname = prefix + "/" + filename;
  SongSortedByIndex * songintree = (SongSortedByIndex*)tree.search(fullname);
  Song * song;
  if (!songintree)
    {
      song = new Song(filename,prefix,false);
      database->add(song);
      // printf("%s has been added\n",(const char*)fullname);
    }
  else 
    {
      song = songintree->song;
    }
  tree.del(fullname);
  
  if (song->modifiedOnDisk())
    {
      song->reread(false);
      // printf("%s has been modified\n",(const char*)fullname);
    }
  song->checkondisk();
  
  if (++total_files%10 == 0)
    {
      loader->readingFile -> setText(filename);
      loader->progressBar1 -> setProgress(total_files);
      app->processEvents();
    }
}

void IndexReader::writeCache()
{
  /*  QFile f("bpmdj-cache.tmp");
  f.open(IO_WriteOnly);
  QDataStream s(&f);
  int count;
  Song ** all = database->getAllSongs(count);
  s << MAGIC_NOW;
  s << count;
  for(int i = 0 ; i < count ; i ++)
    all[i]->toStream(s);
  */
}

void IndexReader::readCache()
{
  /* int magic;
  int count;
  if (!QFile::exists("bpmdj-cache.tmp")) return;
  QFile f("bpmdj-cache.tmp");
  printf("If the program aborts, please remove the file bpmdj-cache.tmp\n");
  f.open(IO_ReadOnly);
  QDataStream s(&f);
  s >> magic;
  if (magic != MAGIC_NOW) return;
  s >> count;
  for(int i = 0 ; i < count ; i ++)
    {
      Song * song = new Song();
      song->fromStream(s);
      database->add(song);
      tree.add(new SongSortedByIndex(song));
    }
  */

  // tree.print();
  // if( !tree.valid() )
  // printf("TREE IS NOT VALID !!!\n");
}
