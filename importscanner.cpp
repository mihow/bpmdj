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

#include <qmultilineedit.h>
#include <qpushbutton.h>
#include "importscanner.h"
#include "qsong.h"
#include "kbpm-dj.h"
#include "avltree.h"
#include "memory.h"
#include "capacity.h"

void ImportScanner::recursing(const QString  dirname)
{
  Log->insertLine("Importing songs from "+dirname);
  Log->setCursorPosition(Log->numLines(),1);
  app->processEvents();
}

ImportScanner::ImportScanner(SongSelectorLogic* root) : 
  DirectoryScanner(NULL),
  ScanningProgress()
{
  selector = root;
  database = selector->database->getFileTreeCopy();
}

bool ImportScanner::matchextension(const QString filename)
{
  return goodExtension(filename);
}

void ImportScanner::scan(const QString dirname, const QString checkname)
{
  show();
  DirectoryScanner::scan(dirname, checkname);
  Okay->setEnabled(true);
}

void ImportScanner::checkfile(const QString pathname, const QString filen)
{
  QString filename;
  if (pathname != QString::null)
    filename = pathname + "/" + filen;
  else
    filename = filen;
  if (database->search(filename))
    {
      database->del(filename);
    }
  else
    {    
      Index * index = createNewIndexFor(filename,"./index/");
      Song  * song = new Song(index,true,true);
      selector -> acceptNewSong( song );
      
      char log[500];
      sprintf(log,"Writing %s",(const char*)index->get_storedin());
      Created->insertLine(log);
      Created->setCursorPosition(Created->numLines(),1);
      
      app->processEvents();
      
      delete index;
    }
}
