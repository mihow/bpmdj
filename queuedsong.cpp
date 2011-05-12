/****
 BpmDj v4.1pl1: Free Dj Tools
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
#ifndef __loaded__queuedsong_cpp__
#define __loaded__queuedsong_cpp__
using namespace std;
#line 1 "queuedsong.c++"
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <qlistview.h>
#include "qsong.h"
#include "history.h"
#include "dirscanner.h"
#include "queuedsong.h"
#include "tags.h"
#include "config.h"
#include "scripts.h"
#include "qstring-factory.h"
#include "qt-helpers.h"

#define QUEUED_ANKER 0
#define QUEUED_DLINE 1
#define QUEUED_TEMPO 2
#define QUEUED_SPECTRUM 3
#define QUEUED_TIME 4
#define QUEUED_CUES 5
#define QUEUED_TITLE 6
#define QUEUED_AUTHOR 7
#define QUEUED_VERSION 8
#define QUEUED_TAGS 9
#define QUEUED_ONDISK 10
#define QUEUED_INDEX 11
#define QUEUED_MD5SUM 12
#define QUEUED_FILE 13

QueuedAnalSong::QueuedAnalSong(QTreeWidget* parent, Song * s) :
  TreeWidgetSong(s,parent)
{
  setText( ANAL_TITLE ,song->get_title());
  setText( ANAL_AUTHOR ,song->get_author());
  setText( ANAL_INDEX ,song->get_storedin());
  setText( ANAL_RMS ,needs_energy() ? ANAL_NEC : ANAL_NOTNEC);
  setText( ANAL_TEMPO ,needs_tempo() ? ANAL_NEC : ANAL_NOTNEC);
  setText( ANAL_SPECTRUM ,needs_spectrum() ? ANAL_NEC : ANAL_NOTNEC);
  setText( ANAL_ECHO ,needs_echo() ? ANAL_NEC : ANAL_NOTNEC);
  setText( ANAL_RHYTHM ,needs_rhythm() ? ANAL_NEC : ANAL_NOTNEC);
  setText( ANAL_COMPOSITION ,needs_composition() ? ANAL_NEC : ANAL_NOTNEC);
}

QueuedSong::QueuedSong(Song * s, QTreeWidget* parent) :
  TreeWidgetSong(s,parent)
{
  setText(QUEUED_TEMPO,song->tempo_str());
  anker = true;
  distance = 0; 
  mark = false;
  pos = parent->topLevelItemCount();
  init();
}

QueuedSong::QueuedSong(QTreeWidget* parent, QTreeWidgetItem *after) :
  TreeWidgetSong(NULL,parent,after)
{
  anker = false;
  distance = -1; 
  mark = false;
  pos = parent->topLevelItemCount();
  init();
}

void QueuedSong::init()
{
  setText(QUEUED_ANKER,tonumber(pos));
  if (anker)
    setBackground(QUEUED_ANKER,QColor(255,0,0));
  else
    setBackground(QUEUED_ANKER,QColor(255,255,0));
  if (!song) 
    {
      for(int i = 0 ; i < columnCount(); i++)
	if (i!=QUEUED_ANKER)
	  setText(i,EMPTY);
      return;
    }
  //  if (!song && i != QUEUED_ANKER) 
  //    return; //   return QString::null;
  setText(QUEUED_VERSION,song->get_version());
  setText(QUEUED_TITLE,song->get_title());
  setText(QUEUED_AUTHOR,song->get_author());
  setText(QUEUED_INDEX,song->get_storedin());
  setText(QUEUED_TAGS,Tags::full_string(song->get_tags()));
  setText(QUEUED_TIME,song->get_time());
  setText(QUEUED_MD5SUM,song->get_md5sum());
  setText(QUEUED_DLINE,QString::number(distance,'f',3));
  setText(QUEUED_SPECTRUM,song->get_spectrum_string());
  setText(QUEUED_FILE,song->get_file());
  setText(QUEUED_ONDISK,song->get_ondisk() ? TRUE_TEXT : FALSE_TEXT);

  setText(QUEUED_CUES,song->get_has_cues() ? TRUE_TEXT : FALSE_TEXT);
  if (Config::color_cues && !song->get_has_cues())
    setBackground(QUEUED_CUES,QColor(0,0,255));
  
  if (Config::color_played && song->get_played())
    setBackground(QUEUED_TITLE,Config::get_color_played_song());
  
  setBackground(QUEUED_AUTHOR,QSong::colorOfAuthorCol(song));
  
  int yellow = 255;
  if (distance<=1)
    yellow = (int)(distance*255.0);
  setBackground(QUEUED_DLINE,QColor(yellow,255-yellow,0));
  
  if (Config::color_spectrum)
    if (song->get_spectrum()!=no_spectrum)
      setBackground(QUEUED_SPECTRUM,song->get_color());
  
  if (Config::color_ondisk && !song->get_ondisk())
    for(int i = 0 ; i < columnCount(); i++)
      setBackground(i,Config::get_color_unavailable());
}

void QueuedSong::setSong(Song* s, float8 d)
{
  song=s;
  distance=d;
  if (song)
    setText(QUEUED_TEMPO, song->tempo_str());
  init();
}

QString QueuedAnalSong::getCommand(SongSlot &analyzer)
{
  return analyzer.getAnalCommand(needs_tempo(),
		 Config::get_anal_bpm_technique(),
		 Config::get_anal_bpm_from(), Config::get_anal_bpm_to(),
		 needs_spectrum() || needs_echo(),
		 needs_energy(),
		 needs_rhythm() || needs_composition(),
		 song->get_storedin());
}

QString QueuedAnalSong::getDescription(SongSlot &analyzer)
{
  return "Analysis " + song->getDisplayTitle() + " at " + analyzer.getName();
}
#endif // __loaded__queuedsong_cpp__
