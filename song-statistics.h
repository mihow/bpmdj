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

#ifndef SONG_STATISTICS_H
#define SONG_STATISTICS_H

#include <math.h>
#include "common.h"
#include "signals.h"
#include "files.h"

float4 normalize_freq(float4 in, int band);
//float4 normalize_echo(float4 val, int band, int delay);
float4 stats_get_freq(int band);

void statistics_first_pass(Song * song);
void statistics_prepare_second_pass();
void statistics_second_pass(Song * song);
void statistics_stop_second_pass();

#endif
