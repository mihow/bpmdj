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

#include "version.h"

#ifdef COMPILE_ALSA
#include <alsa/asoundlib.h>
#include "dsp-drivers.h"

class dsp_alsa: public dsp_driver
{
 private:
  static snd_pcm_t     *dsp;
  static snd_pcm_uframes_t buffer_size;
  static snd_pcm_uframes_t period_size;
  void    wwrite(unsigned4 *value);
 public:
  static  char * arg_dev;
  
  void    start();
  void    pause();
  void    write(unsigned4 *value);
  signed8 latency();
  int     open();
  void    close();
};
#endif
