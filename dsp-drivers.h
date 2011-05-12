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

#ifndef DSP_DRIVERS
#define DSP_DRIVERS

#include "common.h"
#include "dsp-oss.h"
#include "dsp-alsa.h"

extern int dsp_oss;
extern int dsp_alsa;
extern int dsp_driver;
/*-------------------------------------------
 *         Dsp operations
 *-------------------------------------------*/
void dsp_start();
void dsp_pause();
void dsp_write(unsigned4 *value);
signed8 dsp_latency();
int dsp_open();
void dsp_close();

/*-------------------------------------------
 *         Mixer operations
 *-------------------------------------------*/
/*int mixer_open();
void mixer_close();
int mixer_get_main();
int mixer_get_pcm();
void mixer_set_main(int v);
void mixer_set_pcm(int v);
*/


#endif
