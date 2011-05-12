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

#include <assert.h>
#include "dsp-oss.h"
#include "dsp-alsa.h"

#ifndef COMPILE_OSS
#ifndef COMPILE_ALSA
#error -------------------------------------------
#error Should at least compile one dsp driver !!! 
#error Try using -D COMPILE_OSS or -D COMPILE_ALSA
#error -------------------------------------------
#endif
#endif

int dsp_driver::parse_option(char* opt, char* argument)
{
  if (option(opt,"v","verbose"))
    {
      verbose = true;
      return 1;
    }
  return 0;
}
