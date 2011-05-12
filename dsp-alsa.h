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

#define ALSA_OPTION_HELP ""

#ifdef COMPILE_ALSA
#include <alsa/asoundlib.h>
#include "dsp-drivers.h"

#undef ALSA_OPTION_HELP
#define ALSA_OPTION_HELP \
"  --alsa-driver---------------------------------\n" \
"               --alsa                use ALSA sound driver\n"\
"               --dev arg             device to use (default = hw:0,0)\n" \
"   -v          --verbose             be verbose with respect to latency\n"\
"   -L nbr      --latency nbr         required latency in ms (default = 744)\n"

class dsp_alsa: public dsp_driver
{
 private:
  snd_pcm_t *dsp;
  snd_pcm_uframes_t buffer_size;
  snd_pcm_uframes_t period_size;
  unsigned4 * buffer;
  unsigned4 filled;
  void    wwrite(unsigned4 *value);
 public:
  char *     arg_dev;
  char *     arg_latency;
  dsp_alsa();
  void    start();
  void    pause();
  void    write(stereo_sample2 value);
  signed8 latency();
  int     open();
  void    close();
  int     parse_option(char* option, char* argument);
};

#endif
