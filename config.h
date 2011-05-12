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

class Config
{
  public:
    static int  file_count;   // the number of .idx files present 
    static int  yellowTime;   
    static int  orangeTime;
    static int  authorDecay;
    static int  redTime;
    static int  filterBpm;
    static bool color_range;
    static bool color_played;
    static bool color_authorplayed;
    static bool color_ondisk;
    static bool color_cues;
    static bool color_dcolor;
    static bool color_spectrum;
    static bool limit_ondisk;
    static bool limit_nonplayed;
    static bool limit_uprange;
    static bool limit_downrange;
    static bool limit_indistance;
    static QString playCommand1;
    static QString playCommand2;
    static QString playCommand3;
    static QString playCommand4;
 public:
    static void openUi();
    static void save();
    static void load();
};

#endif
