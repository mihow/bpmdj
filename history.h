/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle
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


#include <stdlib.h>
#include <stdio.h>
#include <qstring.h>
#include "song.h"
#include "growing-array.h"

/**
 * A class to represent the song already played
 */

class Played
{
  private:
    static Song * t_2;  // T - 2
    static Song * t_1;  // T - 1
    static Song * t_0;  // T: the current main
    static GrowingArray<QString> names;
    static FILE* f;
  public:
    static int songs_played;
    Played(const QString filename);
    static bool IsPlayed(Song * which);
    static void Play(Song * main_now);
};
