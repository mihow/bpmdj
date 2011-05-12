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

#include <qmultilineedit.h>
#include <qpushbutton.h>
#include "importscanner.h"
#include "qsong.h"
#include "kbpm-dj.h"
#include "avltree.cpp"
#include "index.h"

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
  database = selector->database->getFileTree();
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
      char *temp;
      char indexname[500];
      char halfindexname[500];
      temp=strdup(basename(strdup(filename)));
      temp[strlen(temp)-4]=0;
      sprintf(halfindexname,"%s.idx",temp);
      sprintf(indexname,"./index/%s.idx",temp);
      int nr=2;
      while(exists(indexname))
	{
	  sprintf(halfindexname,"%s%d.idx",temp,nr);
	  sprintf(indexname,"./index/%s%d.idx",temp,nr++);
	}
      
      Index *index = new Index();
      index->meta_writeto(indexname);
      
      char log[500];
      sprintf(log,"Writing %s",index->meta_readfrom());
      Created->insertLine(log);
      Created->setCursorPosition(Created->numLines(),1);
      
      app->processEvents();
      
      index->index_file=strdup(filename);
      index->index_changed=true;
      index->index_tags=strdup("New");
      index->set_period(-1);  // writes immediatelly to disk
      delete index;
      
      selector -> acceptNewSong( new Song( ( const QString ) halfindexname, IndexDir, true ) );
    }
}
