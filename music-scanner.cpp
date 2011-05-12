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
#line 1 "music-scanner.c++"
#include <qtextedit.h>
#include <qpushbutton.h>
#include "music-scanner.h"
#include "qsong.h"
#include "bpmdj.h"
#include "memory.h"
#include "capacity.h"

void MusicScanner::recursing(const QString dirname)
{
  Log->setText("Importing songs from "+dirname);
  app->processEvents();
}

MusicScanner::MusicScanner(SongSelectorLogic* root) : 
  QDialog(root),
  DirectoryScanner(NULL)
{
  setupUi(this);
  reset(MusicDir);
  selector = root;
  database = selector->database->getFileTreeCopy();
}

bool MusicScanner::matchextension(const QString filename)
{
  bool res = goodExtension(filename);
  if (!res)
    {
      ignored->insertPlainText(filename+"\n");
      ignored->moveCursor(QTextEdit::MoveEnd);
    }
  return res;
}

void MusicScanner::checkfile(const QString pathname, const QString filen)
{
  QString filename;
  if (pathname.isEmpty())
    filename = filen;
  else
    // we need to get rid of the ./music/
    // ./music is contained in the prefix_length, so we need to add one
    filename = pathname.mid(prefix_length+1) + slash + filen;
  map<QString,Song*>::iterator it = database.find(filename);
  if (it!=database.end())
    {
      it->second->set_ondisk(true);
      database.erase(filename);
    }
  else
    {    
      Index * index = createNewIndexFor(filename,"./index/");
      Song  * song = new Song(index,true,true);
      selector -> acceptNewSong( song );
      created->insertPlainText(index->get_storedin().mid(8)+"\n");
      created->moveCursor(QTextEdit::MoveEnd);
      app->processEvents();
      delete index;
    }
}
