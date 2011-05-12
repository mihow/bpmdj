/****
 BpmDj v3.8: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

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
#ifndef __loaded__dsp_drivers_cpp__
#define __loaded__dsp_drivers_cpp__
using namespace std;
#line 1 "dsp-drivers.c++"
#include <assert.h>
#include <qdialog.h>
#include <qstring.h>
#include "player-config.h"
#include "dsp-oss.h"
#include "dsp-alsa.h"
#include "dsp-none.h"
#include "dsp-jack.h"
#include "scripts.h"

#ifndef COMPILE_OSS
#ifndef COMPILE_ALSA
#ifndef COMPILE_JACK
#error -------------------------------------------
#error You should at least compile one dsp driver.
#error Use -D COMPILE_OSS or -D COMPILE_ALSA or
#error or -D COMPILE_JACK
#error -------------------------------------------
#endif
#endif
#endif

dsp_driver * dsp_driver::get_driver(PlayerConfig * cfg)
{
  switch (cfg->get_player_dsp())
    {
    case 0 : return new dsp_none ( * cfg ) ;
    case 1 :
#ifdef COMPILE_OSS
      return new dsp_oss  ( * cfg ) ;
#else
      Warning("The OSS driver was not compiled in");
      return new dsp_none( * cfg );
#endif
    case 2 :
#ifdef COMPILE_ALSA
      return new dsp_alsa ( * cfg ) ;
#else
      Warning("The ALSA driver was not compiled in");
      return new dsp_none( * cfg );
#endif
    case 3 : assert(0); 
    case 4 : 
#ifdef COMPILE_JACK
      return new dsp_jack ( * cfg ) ;
#else
      Warning("The Jack driver was not compiled in");
      return new dsp_none( * cfg );
#endif
    }
  return new dsp_none( * cfg );
}
#endif // __loaded__dsp_drivers_cpp__
