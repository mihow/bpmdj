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

#include <qstring.h>
#include "song.h"
#include "avltree.h"

class QStringSortedNode: public Node<QString>
{
 public:
  virtual QString getKey() = 0;
  virtual int compareAddData(Node<QString>* other);
  virtual int compareSearchData(QString key);
  virtual void print();
  virtual ~QStringSortedNode() {};
};

class QStringNode: public QStringSortedNode
{
 public:
  QString content;
  QStringNode(QString c) : QStringSortedNode() {content=c;};
  virtual QString getKey() {return content;};
};

class SongNode: public QStringSortedNode
{
 public:
  Song * song;
  SongNode(Song* s) : QStringSortedNode() {song = s;};
};

class SongSortedByFile: public SongNode
{
 public:
  SongSortedByFile(Song* s) : SongNode(s) {};
  QString getKey() {return song->get_file();}
};

class SongSortedByIndex: public SongNode
{
 public:
  SongSortedByIndex(Song* s) : SongNode(s) {};
  QString getKey() {return song->get_storedin();}
};
