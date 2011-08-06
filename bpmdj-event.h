/****
 BpmDj v4.2-pl2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

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
#ifndef __loaded__bpmdj_event_h__
#define __loaded__bpmdj_event_h__
using namespace std;
#include "custom-event.h"

class SongSelectorLogic;

/**
 * A BpmDjEvent signals various things to the song selector.
 * The selector window will accept each bpmdjevent by executing the 
 * run method. This double dispatching makes it possible
 * to be thread safe while at the same time have the flexibility
 * to write things outside the user interface itself.
 */
class BpmDjEvent: public QEvent
{
 public:
  BpmDjEvent() : QEvent((QEvent::Type)BpmDjCustom)
  {
  }
  virtual void run() = 0;
};
#endif // __loaded__bpmdj_event_h__
