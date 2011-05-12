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
#ifndef __BPMDJ___CONFIG_H__
#define __BPMDJ___CONFIG_H__
using namespace std;
#line 1 "config.h++"
#include <qlistview.h>
#include <qcolor.h>
#include <qheader.h>
#include <qpopupmenu.h>
#include "data.h"
#include "song-process.h"
#include "accessors.h"

// constants that should not be modified
const QString MusicDir = "./music";
const QString IndexDir = "./index";

void realize_mapping(QHeader * h, int column, int location, int size);
void copy_header(QHeader * in, QHeader * out);

class QPopupMenu;

class ConfigState: public QObject
{
    Q_OBJECT
  protected:
    bool state;
    friend class Config;
    QString menu_text;
    QPopupMenu * menu;
    int item;
    void update();
  public:
    ConfigState(bool init = false);
    virtual void setMenuText ( const QString & );
    bool isOn() const;
    virtual void addTo ( QPopupMenu * w );
    operator bool() const {return state;};  
    // the set function bypasses the signal emit phase and does not update the UI
    void set(bool);
  public slots:
    void toggle ();
    // setOn will emit a modification signal to the relevant partners.
    virtual void setOn ( bool );
  signals:
    void toggled();
};

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
  static SongProcess players[4];
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
  singleton_accessors(float,distance_temposcale);
 public:
  singleton_accessors(float,distance_spectrumweight);
  static ConfigState limit_authornonplayed;
  singleton_accessors(bool,shown_aboutbox);
  // version 2.1
  // removed tmp_directory from version 2.9
  singleton_accessors(QString,mixer_command);
  // static ConfigState open_mixer;
  // version 2.2
  static ConfigState ask_mix;
  static ConfigState auto_popqueue;
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
  static ConfigState open_bpmmixer;
  // 2.7
  singleton_accessors(float,distance_tempoweight);
  singleton_accessors(float,distance_echoweight);
  singleton_accessors(float,distance_rythmweight);
  singleton_accessors(float,distance_compositionweight);
  singleton_accessors(int,  max_songs);
  // 2.8
  singleton_accessors(QColor,color_unchecked);
  // 2.9
  static SongProcess analyzers[8];
  // from here on the options are saved again
  singleton_accessors(float, anal_bpm_from);
  singleton_accessors(float, anal_bpm_to);
  singleton_accessors(int, anal_bpm_technique);
  // 3.0
  static ConfigState color_songs_based_on_history;
  singleton_accessors(QColor, color_alltime);
 private:
  static void calc_and_cache();
  static void set_playCommand1(QString s) 
    {
      players[0].setOldCommand(s);
    };
  static void set_playCommand2(QString s)
    {
      players[1].setOldCommand(s);
    };
  static void set_playCommand3(QString s)
    {
      players[2].setOldCommand(s);
    };
  static void set_playCommand4(QString s)
    {
      players[3].setOldCommand(s);
    };
  static void set_analCommand1(QString s)
    {
      analyzers[0].setOldCommand(s);
    };
  static void set_analCommand2(QString s)
    {
      analyzers[1].setOldCommand(s);
    };
  static void set_analCommand3(QString s)
    {
      analyzers[2].setOldCommand(s);
    };
  static void set_analCommand4(QString s)
    {
      analyzers[3].setOldCommand(s);
    };
  static void set_analCommand5(QString s)
    {
      analyzers[4].setOldCommand(s);
    };
  static void set_analCommand6(QString s)
    {
      analyzers[5].setOldCommand(s);
    };
  static void set_analCommand7(QString s)
    {
      analyzers[6].setOldCommand(s);
    };
  static void set_analCommand8(QString s) 
    {
      analyzers[7].setOldCommand(s);
    };
 public:
  static bool open_ui(int pane = 0);
  static void save();
  static bool load();
};
#endif
