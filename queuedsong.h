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
#ifndef __BPMDJ___QUEUEDSONG_H__
#define __BPMDJ___QUEUEDSONG_H__
using namespace std;
#line 1 "queuedsong.h++"
#include <q3listview.h>
#include "config.h"
#include "cluster.h"

class QueuedSong: 
  public Q3ListViewItem
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
    QueuedSong(Song *song, Q3ListView* parent);
    QueuedSong(Q3ListView* parent, Q3ListViewItem* after);
    bool ankered() {return anker;};
    void setAnkered(bool b) {anker = b; if (anker) distance=0;};
    virtual void paintCell(QPainter *p, const QColorGroup &cg, int col, int wid, int align);
    virtual QString text(int i) const;
};


#define ANAL_TITLE 0
#define ANAL_AUTHOR 1
#define ANAL_RMS 2
#define ANAL_SPECTRUM 3
#define ANAL_ECHO 4
#define ANAL_TEMPO 5
#define ANAL_RYTHM 6
#define ANAL_COMPOSITION 7
#define ANAL_INDEX 8

class SongProcess;
class QueuedAnalSong: 
  public Q3ListViewItem
{
  private:
    Song * song;
    bool needs_energy() const { return  song->needs_energy(); };
    bool needs_tempo() const { return song->needs_tempo(); };
    bool needs_spectrum() const { return song->needs_spectrum();};
    bool needs_echo() const { return song->needs_echo();};
    bool needs_rythm() const { return song->needs_rythm();};
    bool needs_composition() const { return song->needs_composition();};
  public:
    //    void setSong(Song* s, double d);
    Song* getSong() { return song; };
    QString getDisplayTitle() {return song->getDisplayTitle();};
    QueuedAnalSong(Q3ListView* parent,Song *song);
    virtual QString text(int i) const;
    QString getCommand(SongProcess& p);
};
#endif
