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
#ifndef __loaded__song_process_h__
#define __loaded__song_process_h__
using namespace std;
#line 1 "song-process.h++"
#include <set>
#include "overseer.h"
class SongSlot;
struct DeathProcessListener
{
  virtual void process_died(SongSlot* slot) {};
};

struct SongProcess: public Process
{
  SongSlot* slot;
  DeathProcessListener* listener;
  SongProcess(SongSlot* i, DeathProcessListener* l)
  {
    slot=i;
    listener=l;
    assert(slot);
    assert(listener);
  }
  void process_died()
  {
    listener->process_died(slot);
  }
  static void checkSignals();
  void start(QString command, QString description, bool append);
};
#endif // __loaded__song_process_h__
