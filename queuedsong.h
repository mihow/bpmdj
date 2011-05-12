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

#include "config.h"
#include "cluster.h"

class QueuedSong: 
  public QListViewItem
{
  private:
    Song * song;
    int    pos;
    double distance;
    bool anker;
  public:
    bool mark;
  public:
    void setSong(Song* s, double d);
    Song* getSong() {return song;};
    void setPos(int p) {pos=p;};
    int getPos() {return pos;};
    QueuedSong(Song *song, QListView* parent);
    QueuedSong(QListView* parent, QListViewItem* after);
    bool ankered() {return anker;};
    void setAnkered(bool b) {anker = b; if (anker) distance=0;};
    virtual void paintCell(QPainter *p, const QColorGroup &cg, int col, int wid, int align);
    virtual QString text(int i) const;
};
