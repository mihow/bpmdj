/****
 BpmDj v4.1pl1: Free Dj Tools
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
#ifndef __loaded__fragment_deliverer_cpp__
#define __loaded__fragment_deliverer_cpp__
using namespace std;
#line 1 "fragment-deliverer.c++"
#include "fragment-player.h"
#include "bpmdj.h"

void FragmentDeliverer::markFinished()
{
  finished=true;
  fragmentPlayer.delivererFinished();
}
#endif // __loaded__fragment_deliverer_cpp__