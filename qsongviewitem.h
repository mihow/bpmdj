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

#define LIST_DIR 0
#define LIST_VERSION 4
#define LIST_TITLE 1
#define LIST_AUTHOR 2
#define LIST_TEMPO 3
#define LIST_TAGS 5
#define LIST_FILE 6
#define LIST_INDEX 7
#define LIST_ONDISK 8
#define LIST_MD5SUM 9

class QSongViewItem: public QListViewItem
{
 private:
   Song* song;
 public:
   QSongViewItem(QListViewItem* parent, Song* content);
   QSongViewItem(QListView* parent, Song* content);
   Song* getSong() {return song;};
   virtual void paintCell(QPainter *p,const QColorGroup &cg, int col, int wid, int align);
   virtual QString text(int i) const;
   void reread() {song->reread();};
};
