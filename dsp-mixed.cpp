/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

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
int        dsp_mixed::mix_dev = 0;

dsp_mixed::dsp_mixed(const PlayerConfig & config) : dsp_driver(config)
{
  mix_dev = config.get_mixed_channel();
}

void dsp_mixed::start()
{
  shared -> status = mix_start;
}

void dsp_mixed::pause()
{
  // we stop immediatelly by placing write and read at the start of the buffer
  shared -> write = 1; // read will never skip write so we put this first
  shared -> read = 0;  // then we set the read pointer and assign the initial value
  shared -> status = mix_pause;
  wait_for_unpause();
  toskip = shared->latency + audiosize;
}

void dsp_mixed::write(stereo_sample2 value)
{
  // do skip writing when we are starting
  // but when we start reset the read and write pointers
  if (toskip) 
    {
      // printf("Skipping %d\n",toskip);
      toskip--;
      if (toskip) return;
      shared -> write = 1; // read will never skip write so we put this first
      shared -> read = 0;  // then we set the read pointer and assign the initial value
      audio[0].sync = ((signed4)((::x - ::cue) % ::normalperiod)) * sync_max / (signed4)::normalperiod;
      shared -> status = mix_sync;
    }
  
  // do not advance when buffer is full
  signed4 w = shared->write;
  while (w==shared->read && shared->status!=mix_halt) 
    {
      if (shared->status==mix_halt) return;
      usleep(1000000/50000);
    }
  audio[w].sync = ((signed4)((::x - ::cue) % ::normalperiod)) * sync_max / (signed4)::normalperiod;
  audio[w++].value = value;
  w%=audiosize;
  shared->write=w;
}

signed8 dsp_mixed::latency()
{
  signed8 latency = shared->write+audiosize-shared->read;
  if (latency >= audiosize) latency -= audiosize;
  return latency+shared->latency;
}

int dsp_mixed::open()
{
  toskip = 0;
  printf("Opening dsp-mixed %d \n",mix_dev);
  char devname[500];
  sprintf(devname,"/kbpm-mix%d",mix_dev);
  devfd = shm_open(devname,O_RDWR,S_IRUSR|S_IWUSR); 
  if (devfd == -1)
    printf("Could not open shared memory area for device %s\n",devname);
  shared = (mixarea*) mmap(NULL,getpagesize(),PROT_READ|PROT_WRITE,MAP_SHARED,devfd,0);
  if (!shared || shared == MAP_FAILED)
    {
      printf("Could not map shared area into memory\n");
      printf("Error is %s\n",strerror(errno));
      exit(50);
    }
  audiosize = shared->size;;
  int buffersize = audiosize * sizeof(struct synced_stereo_sample2) + sizeof(struct mixarea);
  mapsize = buffersize / getpagesize();
  if (mapsize * getpagesize() != buffersize) mapsize++;
  mapsize*=getpagesize();
  shared = (mixarea *) mremap(shared,getpagesize(),mapsize,MAP_SHARED);
  audio = (synced_stereo_sample2*)(void*)(shared+1);
  if (!shared || shared == MAP_FAILED)
    {
      printf("Could not remap shared area into memory\n");
      printf("Error is %s\n",strerror(errno));
      exit(51);
    }
  return err_none;
}

void dsp_mixed::close()
{
  char devname[500];
  sprintf(devname,"/kbpm-mix%d\n",mix_dev);
  shared->init(shared->size);
  munmap(audio,mapsize);
  audio = NULL;
  shared = 0;
  mapsize = 0;
  shm_unlink(devname);
}
 
