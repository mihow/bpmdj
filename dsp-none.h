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

#include "version.h"
#include "common.h"

#define NONE_OPTION_HELP \
"  --none-driver---------------------------------\n"\
"               --none                use no sound driver (default)\n"

class dsp_none: public dsp_driver
{
  void    start();
  void    pause();
  void    write(unsigned4 *value);
  signed8 latency();
  int     open();
  void    close();
  int     parse_option(char* arg, char* argument);
};

