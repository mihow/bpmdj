/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle
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

static int songs_already_in_database = 0;

IndexReader::IndexReader(QProgressBar * b, QLabel *l, DataBase * db) : 
  DirectoryScanner(".bib"),tree()
{ 
  database = db;
  progress = b;
  reading = l;
  total_files = 0;
  reading_bib = true;
  progress->setTotalSteps(Config::file_count);
  songs_already_in_database = 0;
  scan(IndexDir,IndexDir);
  // printf("Debug: %d songs already in database when scanning .bib files\n",songs_already_in_database);
  required_extension = ".idx";
  reading_bib = false;
  songs_already_in_database = 0;
  scan(IndexDir,IndexDir);
  // printf("Debug: %d songs already in database when scanning .idx files\n",songs_already_in_database);
  progress -> setProgress(total_files);
  last_spectrum();  
  QStringFactory::kill();
}

void IndexReader::recursing(const QString dirname)
{
  reading -> setText(dirname);
}

void IndexReader::add(Song * song)
{
   database->add(song);
   song->checkondisk();
   if ( ++ total_files % (Config::file_count/100 > 0 ? Config::file_count / 100 : 10) == 0)
     {
      progress -> setProgress(total_files);
	app -> processEvents();
     }
}

void IndexReader::checkfile(const QString prefix, const QString  filename)
{
  int meta_version;
  QString fullname = prefix + "/" + filename;
  if (reading_bib)
    {
      Index::init_bib_batchread(fullname);
      QString shortname = fullname.left(fullname.findRev(".bib"));
      long position = 0;
      while(!Index::batch_has_ended())
	{
	  Index index;
	  // printf("position = %ld\n",position);
	  position = index.read_bib_field(position,shortname);
	  Song * song = database->find(index.get_filename());
	  if (song)
	    {
	      song->refill(index);
	      songs_already_in_database++;
	    }
	  else
	    add(new Song(&index,false,false,true));
	}
      Index::done_bib_batchread();
    }
  else
    {
      Index index(fullname);
      Song * song = database->find(index.get_filename());
      if (song)
	{
	  song->refill(index,true);
	  songs_already_in_database++;
	}
      else
	add(new Song(&index,true,false,true));
    }
  
}
