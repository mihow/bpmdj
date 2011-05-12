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
#include "song.h"
#include "spectrum-type.h"
#include "database.h"

IndexReader::IndexReader(QProgressBar * b, QLabel *l, BasicDataBase * db, int expected) : 
  DirectoryScanner(".idx")
{
  database = db;
  progress = b;
  reading = l;
  total_files = 0;
  idx_files = 0;
  step_size = expected / 100;
  if (step_size<=0) step_size = 10;
  if (progress) progress->setTotalSteps(expected);
  scan(IndexDir,IndexDir);
  if (progress) progress -> setProgress(total_files);
  QStringFactory::kill();
}

void IndexReader::recursing(const QString dirname)
{
  if (reading) reading -> setText(dirname);
}

void IndexReader::add(Song * song)
{
  database->add(song);
  if ( progress && ++ total_files % step_size == 0)
    {
      progress -> setProgress(total_files);
      app -> processEvents();
    }
}

void IndexReader::checkfile(const QString prefix, const QString  filename)
{
  QString fullname = prefix + "/" + filename;
  idx_files++;
  Index index(fullname);
  Song * song = database->find(index.get_filename());
  if (song)
    song->refill(index,true);
  else
    add(new Song(&index,true,false));
}
