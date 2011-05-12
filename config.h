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

#ifndef _CONFIG_H
#define _CONFIG_H

#include <qlistview.h>
#include <qcolor.h>
#include <qheader.h>

// constants that should not be modified
const QString MusicDir = "./music";
const QString IndexDir = "./index";

#define MAGIC_1_6 0xBDE0
#define MAGIC_1_7 0xBDE1
#define MAGIC_1_8 0xBDE2
#define MAGIC_1_9 0xBDE3
#define MAGIC_2_1 0xBDE4
#define MAGIC_2_2 0xBDE5
#define MAGIC_2_4 0xBDE6
#define MAGIC_2_5 0xBDE7
#define MAGIC_NOW MAGIC_2_5

class Config
{
  public:
    // version 1.6
    static int     file_count;
    static int     yellowTime;   
    static int     orangeTime;
    static int     redTime;
    static int     filterBpm;
    static bool    color_range;
    static bool    color_played;
    static bool    color_authorplayed;
    static int     authorDecay;
    static QString playCommand1;
    static QString playCommand2;
    // version 1.7
    static QString playCommand3;
    static QString playCommand4;
    static bool    color_ondisk;
    static bool    color_cues;
    static bool    color_dcolor;
    static bool    color_spectrum;
    static bool    limit_ondisk;
    static bool    limit_nonplayed;
    static bool    limit_uprange;
    static bool    limit_downrange;
    static bool    limit_indistance;
    // version 1.8
    //   removed limit_inspectrum from version 1.7
    // version 1.9
    static float   distance_temposcale;
    static float   distance_spectrumscale;
    static bool    limit_authornonplayed;
    static bool    shown_aboutbox;
    // version 2.1
    static QString tmp_directory;
    static QString mixer_command;
    static bool    open_mixer;
    // version 2.2
    static bool    ask_mix;
    static bool    auto_popqueue;
    // version 2.4
    static QString record_command;
    static QString replay_command;
    static QString record_mixer_command;
    // version 2.5
    static QColor  color_tempo44;
    static QColor  color_tempo54;
    static QColor  color_tempo64;
    static QColor  color_tempo74;
    static QColor  color_tempo84;
    static bool    show_tempo54;
    static bool    show_tempo64;
    static bool    show_tempo74;
    static bool    show_tempo84;
    static bool    show_unknown_tempo;
    static QColor  color_green_time;
    static QColor  color_yellow_time;
    static QColor  color_orange_time;
    static QColor  color_red_time;
    static bool    color_main_window;
    static bool    log_spectrum_distance;
    static QColor  color_played_song;
    static QColor  color_played_author;
    static QColor  color_unavailable;
    static QColor  color_dcolor_col;
    static int     color_cluster_depth;
    static QListView *taglist;
    static QHeader   *header;
 public:
    static void openUi();
    static void save();
    static void load();
};

#endif
