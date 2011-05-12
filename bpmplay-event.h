/****
 BpmDj v4.1: Free Dj Tools
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
#ifndef __loaded__bpmplay_event_h__
#define __loaded__bpmplay_event_h__
using namespace std;
#line 1 "bpmplay-event.h++"
#include "custom-event.h"

class Player;
/**
 * A player event used to signal various things to the Qt song player.
 * The player window will accept each play event by executing the 
 * run method. This double dispatching makes it possible
 * to be thread safe while at the same time have the flexibility
 * to write things outside the user interface itself.
 */
class BpmPlayEvent: public QEvent
{
public:
  BpmPlayEvent() : QEvent((QEvent::Type)BpmPlayCustom)
  {
  }
  virtual void run(Player * player) = 0;
};

class InitAndStart: public BpmPlayEvent
{
public:
  virtual void run(Player * player);
};

class PlayingStateChanged: public BpmPlayEvent
{
public:
  virtual void run(Player * player);
};
#endif // __loaded__bpmplay_event_h__
