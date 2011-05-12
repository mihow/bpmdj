/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle

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

#define OSS_OPTION_HELP ""

#ifdef COMPILE_OSS

#include <linux/soundcard.h>
#include "dsp-drivers.h"

#undef OSS_OPTION_HELP
#define OSS_OPTION_HELP \
	 "  --oss-driver---------------------------------\n"\
	 "               --oss                 use OSS driver\n"\
	 "   -d arg      --dsp arg             dsp device to use (default = /dev/dsp)\n"\
	 "   -F nbr      --fragments nbr       the number of fragments used to play audio.\n"\
	 "   -X          --nolatencyaccounting does not take into account the latency when marking a cue\n"\
	 "   -v          --verbose             be verbose with respect to latency\n"\
	 "   -L nbr      --latency nbr         required latency in ms (default = 744)\n"


class dsp_oss: public dsp_driver
{
 private:
  int            dsp;
  signed8        dsp_writecount;
  audio_buf_info dsp_latency;
  char * arg_dsp;
  char * arg_fragments;
  int    opt_fragments;
  int    opt_nolatencyaccounting;
  int    opt_latency ;
  char*  arg_latency ;
  signed8 playcount();
  void flush();
 public:
  dsp_oss();
  void    start();
  void    pause();
  void    write(unsigned4 *value);
  signed8 latency();
  int     open();
  void    close();
  int     parse_option(char* arg, char* argument);
};
#endif
