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

#ifndef CAPACITY_H
#define CAPACITY_H

#include "basic-types.h"

/**
 * Capacities specify which programs are available and which 
 * programs can be used to decode a file at a specific host ? 
 * capacities are used in a negated setting. We specify which ones
 * are disabled. This allows much esaier backward compatibility
 * in future versions
 */
#define CAPACITY_MPG123059R   0x01  // 1
#define CAPACITY_MPG321       0x02  // 2
#define CAPACITY_OGG123       0x04  // 3
#define CAPACITY_MPLAYER1PRE6 0x08  // 4
#define CAPACITY_MPLAYER1PRE7 0x10  // 5
//---
#define CAPACITY_LAST         0x10  

typedef unsigned2   Capacity;
typedef Capacity    capacity_type;
const capacity_type no_disabled_capacities = 0;

// Which capacities are not available for the given extension 
Capacity get_ext_disabled_capacities(QString full_name);

// returns the capacity bit number for the given file and host the decoder 
// number is the one which will be used in the. Zero indicates no single 
// available capacity.
int get_decoder_program(Capacity host_disabled, Capacity ext_disabled, Capacity song_disabled);

// is something an extenstion with capacity ?
bool goodExtension(QString extension);

// Manage the capacity widget
class CapacityWidget;
void init_capacity_widget(CapacityWidget * cw, Capacity c);
Capacity get_capacity(CapacityWidget * cw);
void set_used_decoder(int x);

class PlayerConfig;
class Index;
// Set the environment before calling bpmdj-raw
int set_decoder_environment(PlayerConfig * config, Index * idx);

#endif
