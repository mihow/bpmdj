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

#include "version.h"
#include "dsp-drivers.h"

#define MIXED_OPTION_HELP \
	 "  --mixed-driver---------------------------------\n"\
	 "               --mixed nbr           use the single sound card mix driver (1 or 2)\n"

typedef enum {mix_start, mix_sync, mix_syncing, mix_playing, mix_pause, mix_paused, mix_stop, mix_stopped, mix_halt} mixstate;

struct mixarea
{
  mixstate status;
  signed4 latency;
  signed4 read;
  signed4 write;
  signed4 size;
  void init(int s)
  {
    status = mix_stop;
    latency = 0;
    read = s - 1;
    write = 0;
    size = s;
  }
};

class dsp_mixed: public dsp_driver
{
 private:
  mixarea * shared;
  synced_stereo_sample2 * audio;
  int devfd;
  signed4 mapsize;
  signed4 audiosize;
  unsigned4 toskip;
 public:
  static int mix_dev;
  void    start();
  void    pause();
  void    write(stereo_sample2 value);
  signed8 latency();
  int     open();
  void    close();
};
