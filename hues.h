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
#ifndef __loaded__hues_h__
#define __loaded__hues_h__
using namespace std;
#line 1 "hues.h++"
#include <qcolor.h>
#include "types.h"

/**
 * This files generates a hue bar which has a linear 
 * sorting of colors instead of a banded color pattern. 
 * More information at 
 *
 * http://werner.yellowcouch.org/Papers/huebars/index.html
 */

const int huecolors=360;
extern QRgb hue2rgb[huecolors];
void init_hues();
QRgb get_hue(unsigned2 hue, unsigned1 intensity);
#endif // __loaded__hues_h__
