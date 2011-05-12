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

#include <qapplication.h>
#include "player-core.h"

extern QApplication    * app;
extern PlayerConfig    * config;
extern SongPlayerLogic * player_window;
extern char * arg_config;

enum {InitAndStart = QEvent::User, PlayingStateChanged};
void get_rawpath();
bool show_error(int err, int err2, const char*text);
void msg_playing_state_changed();
