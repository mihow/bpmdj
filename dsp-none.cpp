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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "player-core.h"
#include "version.h"
#include "dsp-none.h"

/*-------------------------------------------
 *         Dsp operations
 *-------------------------------------------*/ 

void dsp_none::start()
{
}

void dsp_none::pause()
{
}

void dsp_none::write(stereo_sample2 value)
{
  usleep(1000000/44100);
}

signed8 dsp_none::latency()
{
  return 0;
}

int dsp_none::open()
{
  return err_none;
}

void dsp_none::close()
{
}

extern void options_failure(char * err);
int dsp_none::parse_option(char* arg, char* argument)
{
  if (option(arg,"dev")             )
    {
      options_failure("The --dev option is dependent on the alsa driver\n"
		      "as such it should come _after_ the --alsa option\n");
      return 1;
    }
  if (option(arg,"verbose","v")    )
    {
      options_failure("The -v option is dependent on a dsp driver,\n"
		      "as such it should come _after_ a dsp selection\n"
		      "(--alsa, --oss,...)\n");
    } 
  if (option(arg,"latency","L")) 
    {
      options_failure("The latency option is dependent on a dsp driver\n"
		      "as such it should come _after_ a dsp selection \n"
		      "(--alsa, --oss,...)\n");
    } 
  return 0;
}
