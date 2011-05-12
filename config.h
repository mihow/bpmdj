/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle

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

#ifndef _CONFIG_H
#define _CONFIG_H

#include <qlistview.h>
#include <qcolor.h>
#include <qheader.h>
#include "data.h"

// constants that should not be modified
const QString MusicDir = "./music";
const QString IndexDir = "./index";

void realize_mapping(QHeader * h, int column, int location, int size);
void copy_header(QHeader * in, QHeader * out);

class Config
{
 public:
  // version 1.6
  singleton_accessors(int,file_count);
  singleton_accessors(int,yellowTime);   
  singleton_accessors(int,orangeTime);
  singleton_accessors(int,redTime);
  singleton_accessors(int,filterBpm);
  singleton_accessors(bool,color_range);
  singleton_accessors(bool,color_played);
  singleton_accessors(bool,color_authorplayed);
  singleton_accessors(int,authorDecay);
  singleton_accessors(QString,playCommand1);
  singleton_accessors(QString,playCommand2);
  // version 1.7
  singleton_accessors(QString,playCommand3);
  singleton_accessors(QString,playCommand4);
  singleton_accessors(bool,color_ondisk);
  singleton_accessors(bool,color_cues);
  singleton_accessors(bool,color_dcolor);
  singleton_accessors(bool,color_spectrum);
  singleton_accessors(bool,limit_ondisk);
  singleton_accessors(bool,limit_nonplayed);
  singleton_accessors(bool,limit_uprange);
  singleton_accessors(bool,limit_downrange);
  singleton_accessors(bool,limit_indistance);
  // version 1.8
  //   removed limit_inspectrum from version 1.7
  // version 1.9
 private:
  singleton_accessors(float,distance_temposcale);
 public:
  singleton_accessors(float,distance_spectrumweight);
  singleton_accessors(bool,limit_authornonplayed);
  singleton_accessors(bool,shown_aboutbox);
  // version 2.1
  // removed tmp_directory from version 2.9
  singleton_accessors(QString,mixer_command);
  singleton_accessors(bool,open_mixer);
  // version 2.2
  singleton_accessors(bool,ask_mix);
  singleton_accessors(bool,auto_popqueue);
  // version 2.4
  singleton_accessors(QString,record_command);
  singleton_accessors(QString,replay_command);
  singleton_accessors(QString,record_mixer_command);
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
  singleton_accessors(QListView*,taglist);
  singleton_accessors(QHeader*,header);
  // 2.6
  singleton_accessors(QString,bpm_mixer_command);
  singleton_accessors(bool,open_bpmmixer);
  // 2.7
  singleton_accessors(float,distance_tempoweight);
  singleton_accessors(float,distance_echoweight);
  singleton_accessors(float,distance_rythmweight);
  singleton_accessors(float,distance_compositionweight);
  singleton_accessors(int,  max_songs);
  // 2.8
  singleton_accessors(QColor,color_unchecked);
  // 2.9
  singleton_accessors(QString,analCommand1);
  singleton_accessors(QString,analCommand2);
  singleton_accessors(QString,analCommand3);
  singleton_accessors(QString,analCommand4);
  singleton_accessors(QString,analCommand5);
  singleton_accessors(QString,analCommand6);
  singleton_accessors(QString,analCommand7);
  singleton_accessors(QString,analCommand8);
  // from here on the options are saved again
  singleton_accessors(float, anal_bpm_from);
  singleton_accessors(float, anal_bpm_to);
  singleton_accessors(int, anal_bpm_technique);
 private:
  static void calc_and_cache();
 public:
  static bool open_ui(int pane = 0);
  static void save();
  static void load();
};

#endif
