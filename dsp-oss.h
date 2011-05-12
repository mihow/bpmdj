/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001 Werner Van Belle

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

#ifdef COMPILE_OSS
#include <linux/soundcard.h>
#include "dsp-drivers.h"

class dsp_oss: public dsp_driver
{
 private:
  static int            dsp;
  static signed8        dsp_writecount;
  static audio_buf_info dsp_latency;
  signed8 playcount();
  void flush();

 public:
  static char * arg_dsp;
  static char * arg_fragments;
  static int    opt_fragments;
  static int    opt_nolatencyaccounting;
  
  void    start();
  void    pause();
  void    write(unsigned4 *value);
  signed8 latency();
  int     open();
  void    close();
};
#endif
