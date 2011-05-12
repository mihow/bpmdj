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

#ifndef _CONFIG_H
#define _CONFIG_H

// constants that should not be modified
const QString MusicDir = "./music";
const QString IndexDir = "./index";

#define MAXTAGSPERCOL 17
#define MAXTAGS (MAXTAGSPERCOL*5)

#define MAGIC_1_6 0xBDE0
#define MAGIC_1_7 0xBDE1
#define MAGIC_1_8 0xBDE2
#define MAGIC_1_9 0xBDE3
#define MAGIC_2_1 0xBDE4
#define MAGIC_2_2 0xBDE5
#define MAGIC_NOW MAGIC_2_2

class Config
{
  public:
    // version 1.6
    static int  file_count;
    static int  yellowTime;   
    static int  orangeTime;
    static int  redTime;
    static int  filterBpm;
    static bool color_range;
    static bool color_played;
    static bool color_authorplayed;
    static int  authorDecay;
    static QString playCommand1;
    static QString playCommand2;
    // version 1.7
    static QString playCommand3;
    static QString playCommand4;
    static bool color_ondisk;
    static bool color_cues;
    static bool color_dcolor;
    static bool color_spectrum;
    static bool limit_ondisk;
    static bool limit_nonplayed;
    static bool limit_uprange;
    static bool limit_downrange;
    static bool limit_indistance;
    // version 1.8
    //   removed limit_inspectrum from version 1.7
    // version 1.9
    static float distance_temposcale;
    static float distance_spectrumscale;
    static bool limit_authornonplayed;
    static bool shown_aboutbox;
    // version 2.1
    static QString tmp_directory;
    static QString mixer_command;
    static bool    open_mixer;
    // version 2.2
    static bool    ask_mix;
    static bool    auto_popqueue;
 public:
    static void openUi();
    static void save();
    static void load();
};

#endif
