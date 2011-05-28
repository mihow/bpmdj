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
#ifndef __loaded__bpmplay_h__
#define __loaded__bpmplay_h__
using namespace std;
#line 1 "bpmplay.h++"
#include <Qt/qapplication.h>
#include "player-core.h"

#include "Data/data.h"
#if (DATA_VERSION_MAJOR != 0) || (DATA_VERSION_MINOR != 2)
#error "DATA library has wrong version number"
#endif

class Player;

extern QApplication  * app;
extern PlayerConfig  * config;
extern Player        * player_window;
extern bool opt_setup;
extern bool opt_check;
extern const char * arg_config;

QString get_rawpath();
bool show_error(int err, int err2, QString text);
void msg_playing_state_changed();
void msg_writing_finished();
#endif // __loaded__bpmplay_h__
