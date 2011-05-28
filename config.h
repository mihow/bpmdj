/****
 BpmDj v4.2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

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
#ifndef __loaded__config_h__
#define __loaded__config_h__
using namespace std;
#line 1 "config.h++"
#include <qtreewidget.h>
#include <qstandarditemmodel.h>
#include <qtreeview.h>
#include <qlistview.h>
#include <qcolor.h>
#include <qheaderview.h>
#include "song-slot.h"
#include "accessors.h"

/**
 * Some constants that should not be modified (at the moment)
 * It would however make sense to allow a different directory 
 * through a configuration file or so
 */
const QString MusicDir = "./music";
const QString IndexDir = "./index";
const QString FragmentsDir = "./fragments";
void taglist2config(QTreeWidget*taglist);

class ConfigState: public QObject
{
  Q_OBJECT
protected:
  bool state;
  friend class Config;
  QAction* item;
  void update();
public:
  ConfigState(bool init = false);
  bool isOn() const;
  virtual QAction* addTo ( QMenu * w, QString menu_text );
  operator bool() const {return state;};  
  // the set function bypasses the signal emit phase and does not update the user interface
  void set(bool);
public slots:
  void toggle ();
  // setOn emits the toggled signal
  virtual void setOn ( bool );
signals:
  void toggled();
};

/**
 * In the list of fields below, we retain those that were present
 * in odler versions. This helps to understand why the config reader
 * sometimes reads a variable and then ignores it
 */
class Config
{
 public:
  // version 1.6
  singleton_accessors(int,file_count);
  singleton_accessors(int,yellowTime);   
  singleton_accessors(int,orangeTime);
  singleton_accessors(int,redTime);
  singleton_accessors(int,filterBpm);
  static ConfigState color_range;
  static ConfigState color_played;
  static ConfigState color_authorplayed;
  singleton_accessors(int,authorDecay);
  static SongSlot players[4];
  static ConfigState color_ondisk;
  static ConfigState color_cues;
  static ConfigState color_dcolor;
  static ConfigState color_spectrum;
  static ConfigState limit_ondisk;
  static ConfigState limit_nonplayed;
  static ConfigState limit_uprange;
  static ConfigState limit_downrange;
  static ConfigState limit_indistance;
  // version 1.8
  //   removed limit_inspectrum from version 1.7
  // version 1.9
 private:
  singleton_accessors(float4,distance_temposcale);
 public:
  singleton_accessors(float4,distance_spectrumweight);
  static ConfigState limit_authornonplayed;
  singleton_accessors(bool,shown_aboutbox);
  // version 2.1
  // removed tmp_directory from version 2.9
  // singleton_accessors(QString,mixer_command);
  // static ConfigState open_mixer;
  // version 2.2
  static ConfigState ask_mix;
  static ConfigState auto_popqueue;
  // version 2.4
  // singleton_accessors(QString,record_command);
  // singleton_accessors(QString,replay_command);
  // singleton_accessors(QString,record_mixer_command);
  // version 2.5
  singleton_accessors(QColor,color_tempo44);
  singleton_accessors(QColor,color_tempo54);
  singleton_accessors(QColor,color_tempo64);
  singleton_accessors(QColor,color_tempo74);
  singleton_accessors(QColor,color_tempo84);
 private:
  singleton_accessors(bool,show_tempo54);
  singleton_accessors(bool,show_tempo64);
  singleton_accessors(bool,show_tempo74);
  singleton_accessors(bool,show_tempo84);
  singleton_accessors(bool,show_unknown_tempo);
 public:
  singleton_accessors(QColor,color_green_time);
  singleton_accessors(QColor,color_yellow_time);
  singleton_accessors(QColor,color_orange_time);
  singleton_accessors(QColor,color_red_time);
  singleton_accessors(bool,color_main_window);
  singleton_accessors(bool,log_spectrum_distance);
  singleton_accessors(QColor,color_played_song);
  singleton_accessors(QColor,color_played_author);
  singleton_accessors(QColor,color_unavailable);
  singleton_accessors(QColor,color_dcolor_col);
  singleton_accessors(int,color_cluster_depth);
  // singleton_accessors(QStandardItemView*,taglist);
  singleton_accessors(QByteArray,header_state);
  // 2.6
  // singleton_accessors(QString,bpm_mixer_command);
  // static ConfigState open_bpmmixer;
  // 2.7
  singleton_accessors(float4,distance_tempoweight);
  singleton_accessors(float4,distance_echoweight);
  singleton_accessors(float4,distance_rhythmweight);
  singleton_accessors(float4,distance_compositionweight);
  singleton_accessors(int, max_songs);
  // 2.8
  singleton_accessors(QColor,color_unchecked);
  // 2.9
  static SongSlot analyzers[8];
  // from here on the options are saved again
  singleton_accessors(float4, anal_bpm_from);
  singleton_accessors(float4, anal_bpm_to);
  singleton_accessors(int, anal_bpm_technique);
  // 3.0
  static ConfigState color_songs_based_on_history;
  singleton_accessors(QColor, color_alltime);
  // 3.4
  static ConfigState play_fragments;
  // 3.5
  singleton_accessors(QString, tag_include);
  singleton_accessors(QString, tag_mustbe);
  singleton_accessors(QString, tag_exclude);
 private:
  static void calc_and_cache();
 public:
  static bool open_ui(int pane = 0);
  static void save();
  static bool load();
};
#endif // __loaded__config_h__
