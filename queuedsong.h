/****
 BpmDj v4.1: Free Dj Tools
 Copyright (C) 2001-2010 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.

 See the authors.txt for a full list of people involved.
****/
#ifndef __loaded__queuedsong_h__
#define __loaded__queuedsong_h__
using namespace std;
#line 1 "queuedsong.h++"
#include "historysong.h"
#include "config.h"
#include "cluster.h"

class QueuedSong: public TreeWidgetSong
{
private:
  unsigned4 pos;
  float8 distance;
  bool anker;
public:
  bool mark;
public:
  void setSong(Song* s, float8 d);
  void setPos(int p) 
  {
    pos=p;
    init();
  };
  int getPos() 
  {
    return pos;
  };
  void init();
  QueuedSong(Song *song, QTreeWidget* parent);
  QueuedSong(QTreeWidget* parent, QTreeWidgetItem* after);
  bool ankered() 
  {
    return anker;
  };
  void setAnkered(bool b) 
  {
    anker = b; 
    if (anker) distance=0;
    init();
  };
};

#define ANAL_TITLE 0
#define ANAL_AUTHOR 1
#define ANAL_RMS 2
#define ANAL_SPECTRUM 3
#define ANAL_ECHO 4
#define ANAL_TEMPO 5
#define ANAL_RHYTHM 6
#define ANAL_COMPOSITION 7
#define ANAL_INDEX 8

class SongSlot;
class QueuedAnalSong: public TreeWidgetSong
{
  private:
    bool needs_energy() const { return  song->needs_energy(); };
    bool needs_tempo() const { return song->needs_tempo(); };
    bool needs_spectrum() const { return song->needs_spectrum();};
    bool needs_echo() const { return song->needs_echo();};
    bool needs_rhythm() const { return song->needs_rhythm();};
    bool needs_composition() const { return song->needs_composition();};
  public:
    QString getDisplayTitle() {return song->getDisplayTitle();};
    QueuedAnalSong(QTreeWidget* parent,Song *song);
    QString getCommand(SongSlot& p);
    QString getDescription(SongSlot &analyzer);
};
#endif // __loaded__queuedsong_h__
