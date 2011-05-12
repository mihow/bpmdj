/****
 BpmDj v4.0: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.
****/
#ifndef __loaded__resampler_cpp__
#define __loaded__resampler_cpp__
using namespace std;
#line 1 "resampler.c++"
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <fftw3.h>
#include "memory.h"
#include "bpmplay.h"
#include "stereo-sample2.h"
#include "period-type.h"
#include "player-core.h"
#include "resampler.h"
#include "lowpass.h"

Resampler resampler;

stereo_sample2 ImpulseResponse::convolve_with(stereo_sample2 * at)
{
  // figure out the length;
  assert(length<640);
  float8 L = 0;
  float8 R = 0;
  for(int i = 0 ; i < length ; i++)
    {
      int j = index[i];
      L+=((float8)(at[j].left))*value[i];
      R+=((float8)(at[j].right))*value[i];
    }
  return stereo_sample2((signed2)L,(signed2)R);
}

float8 ImpulseResponse::gain()
{
  float8 r = 0;
  for(int i = 0 ; i < length ; i++)
    r+=value[i];
  return r;
}

ImpulseResponse& ImpulseResponse::operator /=(float8 v)
{
  for(int i = 0 ; i < length ; i++)
    value[i]/=v;
  return *this;
}

ImpulseResponse ImpulseResponse::shrink()
{
  // find minimum and maximum indices
  int mi = index[0];
  int ma = index[0];
  for(int i = 0 ; i < length ; i++)
    {
      int I = index[i];
      float8 V = value[i];
      if (V!=0)
	{
	  if (I<mi) mi=I;
	  if (I>ma) ma=I;
	}
    }
  ImpulseResponse result(ma-mi+1);
  result.set_amp(0);
  for(int i = mi ; i <= ma ; i++)
    result.idx(i-mi)=i;
  for(int i = 0 ; i < length ; i++)
    {
      int I = index[i];
      float8 V = value[i];
      if (V!=0)
	result.amp(I-mi)+=V;
    }
  return result;
}

ImpulseResponse::ImpulseResponse(int l) : length(l)
{
  index = bpmdj_allocate(length,int);
  value = bpmdj_allocate(length,float8);
  for (int i = 0 ; i < length ; i++)
    {
      index[i]=0;
      value[i]=0;
    }
}

void ImpulseResponse::print()
{
  for(int i = 0 ; i < length ; i ++)
    printf("%d\t%g\n",index[i],value[i]);
}

void ImpulseResponse::set_amp(float8 a)
{
  for (int i = 0 ; i < length ; i++)
    value[i]=a;
}

void create_lowpass(ImpulseResponse & target, float8 cutoff, int Num)
{
  printf("create_lowpass called with length = %d and cutoff %g\n",
	 target.length,cutoff);
  float8 c[target.length/2];
  LpFilter(c,target.length/2,cutoff,100,Num);
  /*  float8 sum = 0;
  for(int i = 0 ; i < target.length/2; i++)
    sum+=c[i];
  float8 ma= 0;
  
  for(int i = 0 ; i < target.length/2 ; i++)
    if (c[i]>ma) ma=c[i];
  */
  target.set_amp(0);
  for(int i = 0 ; i < target.length/2 ; i++)
    {
      target.idx(target.length/2-i) = -i;
      target.idx(target.length/2+i) =  i;
      target.amp(target.length/2+i) =  c[i];
      target.amp(target.length/2-i) =  c[i];
    }
}

stereo_sample2 Resampler::resample(stereo_sample2 * in)
{
  return get_impulse_response(::y).convolve_with(in);
}

void Resampler::generate_table()
{
  table_generated_for = currentperiod;
  int Mb=normalperiod*L/currentperiod;
  if (Mb==M) return;
  M=Mb;
  printf("M = %d\n",M);
  for(int i = 0 ; i < L ; i++)
    {
      ImpulseResponse ir = generate_table(i);
      impulse_responses_at_mod[i]=ir;
      printf("%d has ir length of %d\n",i,ir.length);
      assert(ir.length<640);
      // ir.print();
    }
}

ImpulseResponse Resampler::generate_table(int playpos)
{
  /**
   * Here we have a tricky situation. We need to determine a proper number of 
   * taps for the filter. The first zero crossing of the filter should be at L
   * (or M). The number of taps on the other hand should cross also at a zero 
   * crossing to avoid introducing new aliasing. The last taps amplitude is 
   * given as sin(2.0*Pi*length*0.5/(FS*FS))
   * = sin(Pi*length/(FS*FS)). A sinus becomes 0 at multiples of Pi, so 
   * length/(FS*FS) should be an integer. Lets try 5
   * 1 = length/(FS*FS) or FS*FS*5 = length;
   */
  int FS = -1;
  int length = -1;
  if (L>M)
    {
      FS = L;
      length = 5*FS*FS;
    }
  else
    {
      FS = M;
      length = 5*FS*FS;
    }
  float8 cutoff = 0.5/(float8)FS;
  
  // create a lowpass filter
  ImpulseResponse lowpass(length);
  create_lowpass(lowpass,cutoff,FS);
  
  // now we weed to replace all the indices with their specific address
  // if that address is a proper multiple of L
  int modl = playpos*M;
  for(int i = 0 ; i < length ; i ++)
    {
      int r = lowpass.idx(i)+modl;
      if (r%L!=0)
	lowpass.amp(i)=0;
      lowpass.idx(i)=r/L;
    }
  
  // subtract the address position we calculate based on M and L
  int readpos = playpos * M/L;
  for(int i = 0 ; i < length ; i ++)
    lowpass.idx(i)-=readpos;

  lowpass = lowpass.shrink();
  lowpass /= lowpass.gain();
  return lowpass;
  
}

/**
 **/
//#define TEST_ALONE
#ifdef TEST_ALONE

float8 minimum(float8 a, float8 b)
{
  if (a<b) return a;
  return b;
}

signed8 x,y;
quad_period_type targetperiod;
quad_period_type currentperiod;
quad_period_type normalperiod;

int main(int argc, char* argv[])
{
  //  generate_table();
  //  ImpulseResponse lowpass=generate_table(atoi(argv[1]));
  ImpulseResponse lowpass(100);
  create_lowpass(lowpass,0.25);
  lowpass.shrink().print();
  return 0;
}
#endif
#endif // __loaded__resampler_cpp__
