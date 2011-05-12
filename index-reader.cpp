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
#include "index-reader.h"
#include "kbpm-dj.h"
#include "qsong.h"
#include "config.h"
#include "spectrum.h"
#include "database.h"


IndexReader::IndexReader(Loader *l, DataBase * db) : 
  DirectoryScanner(".idx")
{ 
  database = db;
  loader = l;
  total_files = 0;
  loader->progressBar1->setTotalSteps(Config::file_count);
  scan(IndexDir,IndexDir);
  loader->progressBar1 -> setProgress(total_files);
  lastSpectrum();
};


void IndexReader::checkfile(const QString prefix, const QString  filename)
{
  Song * song = new Song(filename,prefix);
  database->add(song);
  ++total_files;
  if (total_files%10 == 0)
    {
      loader->readingFile -> setText(filename);
      loader->progressBar1 -> setProgress(total_files);
      app->processEvents();
    }
}

