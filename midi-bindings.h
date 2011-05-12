/****
 BpmDj v4.2 beta: Free Dj Tools
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
#ifndef __loaded__midi_bindings_h__
#define __loaded__midi_bindings_h__
using namespace std;
#line 1 "midi-bindings.h++"
#ifdef MIDI
#include "player.h"

typedef void(*slot_function)(Player *, int);

class midi_bindings 
{
protected:
  Player *p;
  slot_function action[256][256];
public:
  midi_bindings(Player *player);
  Player *player() { return p; }
  slot_function binding(int i, int j) { return action[i][j]; }
};

void * process_midi(void* bindings);

#endif
#endif // __loaded__midi_bindings_h__
