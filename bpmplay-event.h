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
#ifndef __BPMDJ___BPMPLAY_EVENT_H__
#define __BPMDJ___BPMPLAY_EVENT_H__
using namespace std;
#line 1 "bpmplay-event.h++"
#include "custom-event.h"

class SongPlayer;
/**
 * A player event used to signal various things to the Qt songplayer.
 * The playerwindow will accept each playevent by executing the 
 * run method. This double dispatching makes it possible
 * to be thread safe while at the same time have the flexibility
 * to wrtie things outside the user interface itself.
 */
class BpmPlayEvent: public QEvent
{
public:
  BpmPlayEvent() : QEvent((QEvent::Type)BpmPlayCustom)
  {
  }
  virtual void run(SongPlayer * player) = 0;
};

class InitAndStart: public BpmPlayEvent
{
public:
  virtual void run(SongPlayer * player);
};

class PlayingStateChanged: public BpmPlayEvent
{
public:
  virtual void run(SongPlayer * player);
};
#endif
