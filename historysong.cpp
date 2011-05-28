/****
 BpmDj v4.2: Free Dj Tools
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
#ifndef __loaded__historysong_cpp__
#define __loaded__historysong_cpp__
using namespace std;
#line 1 "historysong.c++"
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <qlistview.h>
#include "selector.h"
#include "qsong.h"
#include "history.h"
#include "dirscanner.h"
#include "historysong.h"
#include "tags.h"

#define HISTORY_MAINRELATION 0
#define HISTORY_COMMENT 1
#define HISTORY_TEMPO 2
#define HISTORY_SPECTRUM 3
#define HISTORY_DCOLOR 4
#define HISTORY_TIME 5
#define HISTORY_CUES 6
#define HISTORY_TITLE 7
#define HISTORY_AUTHOR 8
#define HISTORY_VERSION 9
#define HISTORY_TAGS 10
#define HISTORY_ONDISK 11

HistorySong::HistorySong(Song * s, QString r, QString c, QTreeWidget* parent) :
  TreeWidgetSong(s,parent)
{
  relation = r;
  comment = c;

  setText(HISTORY_MAINRELATION,relation);
  setText(HISTORY_COMMENT,comment);
  setText(HISTORY_VERSION,song->get_version());
  setText(HISTORY_TITLE,song->get_title());
  setText(HISTORY_AUTHOR,song->get_author());
  setText(HISTORY_TEMPO,song->tempo_str());
  setText(HISTORY_TAGS,Tags::full_string(song->get_tags()));
  setText(HISTORY_TIME,song->get_time());
  setText(HISTORY_DCOLOR,song->get_distance_string());
  setText(HISTORY_SPECTRUM,song->get_spectrum_string());
  setText(HISTORY_ONDISK,song->get_ondisk() ? TRUE_TEXT : FALSE_TEXT);
  setText(HISTORY_CUES,song->get_has_cues() ? TRUE_TEXT : FALSE_TEXT);

  if (Config::color_ondisk && !song->get_ondisk())
    for(int i = 0 ; i < columnCount(); i++)
      setBackground(i,Config::get_color_unavailable());

  setBackground(HISTORY_CUES,QColor(0,0,255));
  
  Song * main = ::main_song;
  setBackground(HISTORY_TEMPO,QSong::colorOfTempoCol(main,song));
  
  if (Config::color_played && song->get_played())
    setBackground(HISTORY_TITLE,Config::get_color_played_song());
  
  setBackground(HISTORY_AUTHOR,QSong::colorOfAuthorCol(song));
  setBackground(HISTORY_DCOLOR,QSong::colorOfdColorCol(song));
  
  if (Config::color_spectrum)
    if (song->get_spectrum()!=no_spectrum)
      setBackground( HISTORY_SPECTRUM,song->get_color());  
}
#endif // __loaded__historysong_cpp__
