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
#include <qapplication.h>
#include <qmessagebox.h>
#include <qlistview.h>
#include <qlcdnumber.h>
#include <qheader.h>
#include <qgroupbox.h>
#include <stdlib.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <qradiobutton.h>
#include <libgen.h>
#include <time.h>
#include <sys/times.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "player-core.h"
#include "version.h"
#include "dsp-mixed.h"

/*-------------------------------------------
 *         Dsp operations
 *-------------------------------------------*/ 
int dsp_mixed::mix_dev = 0;

dsp_mixed::dsp_mixed(const PlayerConfig & config) : dsp_driver(config)
{
  assert(0);
}

void dsp_mixed::start()
{
  assert(0);
}

void dsp_mixed::pause()
{
  assert(0);
}

void dsp_mixed::write(stereo_sample2 value)
{
  assert(0);
}

signed8 dsp_mixed::latency()
{
  assert(0);
}

int dsp_mixed::open()
{
  assert(0);
}

void dsp_mixed::close()
{
  assert(0);
}
 
