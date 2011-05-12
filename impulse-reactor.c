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
#include <libgen.h>
#include <linux/soundcard.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include "common.h"

/*-------------------------------------------
 *         Dsp operations
 *-------------------------------------------*/ 
static int            dsp_out;
static int            dsp_in;

#define WAVRATE (44100)
static const char* arg_dsp_out = "/dev/dsp1";
static const char* arg_dsp_in = "/dev/dsp2";

void dsp_open()
{
  int p;
  dsp_out=open(arg_dsp_out,O_WRONLY);
  if (dsp_out==-1)
    printf("dsp_out: err in opening...\n");
  
  // set dsp parameters
  p=AFMT_S16_LE;
  if (ioctl(dsp_out,SNDCTL_DSP_SETFMT,&p)==-1)
    printf("dsp: setting dsp to standard 16 bit failed\n");
  p=2;
  if (ioctl(dsp_out,SNDCTL_DSP_CHANNELS,&p)==-1)
    printf("dsp: setting dsp to 2 channels failed\n");
  p=WAVRATE;
  if (ioctl(dsp_out,SNDCTL_DSP_SPEED,&p)==-1)
    {
      printf("dsp: setting dsp speed (%d) failed\n",p++);
      if (ioctl(dsp_out,SNDCTL_DSP_SPEED,&p)==-1)
	{
	  printf("dsp: setting dsp speed (%d) failed\n",p);
	  p+=2;
	  if (ioctl(dsp_out,SNDCTL_DSP_SPEED,&p)==-1)
	    printf("dsp: setting dsp speed (%d) failed\n",p);
	}
    }
  
  dsp_in=open(arg_dsp_in,O_RDONLY);
  if (dsp_in==-1)
    printf("dsp_in: err in opening...\n");
  
  // set dsp parameters
  p=AFMT_S16_LE;
  if (ioctl(dsp_in,SNDCTL_DSP_SETFMT,&p)==-1)
    printf("dsp: setting dsp to standard 16 bit failed\n");
  p=2;
  if (ioctl(dsp_in,SNDCTL_DSP_CHANNELS,&p)==-1)
    printf("dsp: setting dsp to 2 channels failed\n");
  p=WAVRATE;
  if (ioctl(dsp_in,SNDCTL_DSP_SPEED,&p)==-1)
    {
      printf("dsp: setting dsp speed (%d) failed\n",p++);
      if (ioctl(dsp_in,SNDCTL_DSP_SPEED,&p)==-1)
	{
	  printf("dsp: setting dsp speed (%d) failed\n",p);
	  p+=2;
	  if (ioctl(dsp_in,SNDCTL_DSP_SPEED,&p)==-1)
	    printf("dsp: setting dsp speed (%d) failed\n",p);
	}
    }
  
}

#define sample_freq 44100
float g_frequency_response(float freq, double *coef, int nr)
{
  float xs=0;
  float ys=0;
  int i = 0;
  for(i=0;i<nr;i++)
    {
      xs+=coef[i]*cos(freq*M_PI*2*(float)i/sample_freq);
      ys+=coef[i]*sin(freq*M_PI*2*(float)i/sample_freq);
    }
  return sqrt(xs*xs+ys*ys);
}


int main(int argc, char* argv[])
{
  int i;
  float fre;
  double c[320];
  double h[320];
  double z[320];
  int l=160;
  unsigned4 value;
  dsp_open();
  FILE *f = fopen("impulse.doubles","rb");
  fread(c,sizeof(double),160,f);
  fclose(f);
  while(c[l]<0) l--;
  for(i = 0 ; i < 320 ; i ++)
    {
      h[i]=0;
      z[i]=0;
    }
  for(i = 0; i < 160 ; i ++)
    printf("%g .. ",c[i]);
  // for(i = 0; i < l ; i++)
  // if (!i%2) c[i]=-c[i];
  printf("filter length = %d\n",l);
  
  for(fre = 1000; fre < 22000; fre+=1000)
    printf("gain at %g = %g\n",fre,g_frequency_response(fre,c,l));
  
  while(1)
    {
      double result;
      read(dsp_in,&value,4);
      // FIR part
      // push new data in history
      for(i = l ; i > 0; i --)
	h[i]=h[i-1];
      h[0]=((signed short*)&value)[0];
      // tijd om de nieuwe outpu waarde te berekenen
      result = 0;
      for(i = 0 ; i < l ; i ++)
	result += c[i]*h[i];
      // result/=l;
      // IIR part
      for(i = 1 ; i < l ; i ++)
	result -= c[i]*z[i];
      result/=c[0];
      // push new data
      for(i = l ; i > 0; i --)
	z[i]=z[i-1];
      z[0]=result;
      // write out value
      ((signed short*)&value)[0]=(signed short)result;
      ((signed short*)&value)[1]=(signed short)result;
      write(dsp_out,&value,4);
    }
  close(dsp_out);
  close(dsp_in);
  return 0;
}
