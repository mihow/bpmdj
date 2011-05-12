/****
 Unphaser: Free Dj Tools
 Copyright (C) 2004 Werner Van Belle
 See 'BeatMixing.ps' for more information

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
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <libgen.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include "sys/times.h"
#include "fourier.h"
#include "common.h"

int main(int argc, char* argv[])
{
  assert(argc==3);
  // read input file
  signed8 samples;
  long int bufsize = 65536;
  longtrick buffer[bufsize];
  FILE * raw = fopen(argv[1],"rb");
  samples=fsize(raw)/4;
  assert(samples);
  signed4 pos = 0;
  fseek(raw,pos,SEEK_SET);
  signed8 length = 1;
  for(signed8 shifter = samples ; shifter > 1 ; shifter/=2, length*=2);
  samples = length;
  assert(length>=samples);
  double * audio = allocate(length,double);
  printf("Unphaser: allocated data-store with size %d for %d samples\n",(int)length,(int)samples);
  while(pos<samples)
    {
      signed8 toread = samples - pos;
      if (toread>bufsize) toread = bufsize;
      long count=readsamples((unsigned4*)buffer,toread,raw);
      
      for(int i = 0 ; i < count; i++)
	audio[(pos+i)]=(buffer[i].leftright.left)+(buffer[i].leftright.right);
      pos+=count;
    }
  printf("Unphaser: Read %ld samples\n",samples);
  fclose(raw);
  
  // normalize audio
  double total = 0;
  for(int i = 0 ; i < samples ; i++)
    total+=audio[i];
  total/=samples;
  if (total!=0)
    for(int i = 0 ; i < samples ; i++)
      audio[i]-=total;
  printf("Unphaser: Removed DC offset %g\n",total);
  for(int i = samples ; i < length ; i ++)
    audio[i]=0;
  double maximum = 0;
  for(int i = 0 ; i < samples ; i++)
    {
      double sample = fabs(audio[i]);
      if (sample>maximum)
	maximum=sample;
    }
  assert(maximum);
  for(int i = 0 ; i < samples ; i++)
    audio[i]/=maximum;
  printf("Unphaser: Normalized audio %g\n",maximum);
  
  // fourier transform
  double * freq_r = allocate(length,double);
  double * freq_i = allocate(length,double);
  fft_double(length,false,audio,NULL,freq_r, freq_i);
  
  // set all phase to 0
  double last=0;
  for(int i = 0 ; i < length ; i ++)
    {
      double re = freq_r[i];
      double im = freq_i[i];
      double en = sqrt(re*re+im*im);
      double an = atan2(im,re);
      an*=1.5;
      freq_r[i]=cos(an)*en;
      freq_i[i]=sin(an)*en;
      // env_fi[i]=;
      // freq_i[i] =( last+= 2* ( freq_i[i+1]-freq_i[i]) ) ;
      // freq_i[i]=freq_i[i]*(double)i/(double)length;
      // freq_i[i]=freq_i[i];
    }
  printf("Unphaser: Removed all phase information\n");

  // do inverse transform
  fft_double(length,true,freq_r,freq_i,audio,freq_i);
  
  // normalize again
  maximum=0;
  for(int i = 0 ; i < samples ; i++)
    {
      double sample = fabs(audio[i]);
      if (sample>maximum)
	maximum=sample;
    }
  assert(maximum);
  maximum/=32768.0;
  for(int i = 0 ; i < samples ; i++)
    audio[i]/=maximum;
  printf("Unphaser: Normalized audio %g\n",maximum);

  // write out all data
  raw = fopen(argv[2],"wb");
  assert(raw);
  pos = 0;
  while(pos<samples)
    {
      signed8 towrite = samples - pos;
      if (towrite>bufsize) towrite = bufsize;
      for(int i = 0 ; i < towrite; i++)
	{
	  buffer[i].leftright.left=audio[pos+i];
	  buffer[i].leftright.right=audio[pos+i];
	}
      long count=writesamples((unsigned4*)buffer,towrite,raw);
      pos+=count;
    }
  fclose(raw);
  printf("Unphaser: Wrote %ld samples\n",samples);
}


/* this experiment only confirms what I suspect, 
 * the fourier transofmration is not suitable because
 * it goes from a "time x amplitude" domain
 * to a           "freq x strength x phase"     domain
 */
