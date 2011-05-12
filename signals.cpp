/****
 BpmDj v4.1: Free Dj Tools
 Copyright (C) 2001-2010 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.

 See the authors.txt for a full list of people involved.
****/
#ifndef __loaded__signals_cpp__
#define __loaded__signals_cpp__
using namespace std;
#line 1 "signals.c++"
#include <cstdio>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <fftw3.h>
#include "common.h"
#include "signals.h"
#include "memory.h"

BasicConvertor::BasicConvertor() {};

float8 Index_to_frequency (unsigned NumSamples, unsigned Index)
{
  if ( Index >= NumSamples )
    return 0.0;
  else if ( Index <= NumSamples/2 )
    return (float8)Index / (float8)NumSamples;
  return -(float8)(NumSamples-Index) / (float8)NumSamples;
}

signed8 higher_power_of_two(signed8 a)
{
  signed8 c = 1;
  while(c < a) c<<=1;
  return c;
}

signed8 lower_power_of_two(signed8 a)
{
  signed8 windowsize = 1;
  for(signed8 tmp = a; tmp; tmp>>=1, windowsize<<=1) ;
  if (windowsize>a) windowsize>>=1;
  return windowsize;
}

float8 find_max(float8 * data, long l)
{
  float8 m = 0;
  for(long i = 0 ; i < l ; i ++)
    if (data[i]>m)
      m=data[i];
  return m;
}

long find_max_rpos(unsigned4 * data, long l)
{
  unsigned4 m = 0;
  if (l == 0) return -1;
  long p = 0;
  m=data[0];
  for(long i = l-1 ; i >= 0 ; i--)
    if (data[i]>m)
      {
	m=data[i];
	p=i;
      }
  return p;
}

void vector_mul_div(signed4 * data, long l, signed4 mul, signed4 div)
{
  signed8 m = mul;
  signed8 d = div;
  if (!d) d = 1;
  for(long i = 0 ; i < l ; i++)
    data[i]=(signed4)((signed8)data[i]*m/d);
}

float8 find_mean(float8 * data, long l)
{
  if (l==0) return 0;
  float8 m = 0;
  for(long i = 0 ; i < l ; i ++)
    m+=data[i];
  return m/(float8)l;
}

float4 find_mean(float4 * data, long l)
{
  if (l==0) return 0;
  float4 m = 0;
  for(long i = 0 ; i < l ; i ++)
    m+=data[i];
  return m/(float8)l;
}

void translate_mean(float4 * data, long l)
{
  float4 m = find_mean(data,l);
  for(int i = 0 ; i < l ; i ++)
    data[i]-=m;
}

void normalize_mean(float8 * data, long l)
{
  float8 m = find_mean(data,l);
  printf("normalizer: mean is %g\n",m);
  if (m>0)
    for(long i = 0 ; i < l ; i ++)
      data[i]/=m;
}

void normalize_mean(float4 * data, long l)
{
  float4 m = find_mean(data,l);
  if (m>0)
    for(long i = 0 ; i < l ; i ++)
      data[i]/=m;
}

float8 norm(float8 r, float8 e)
{
  return sqrt(r*r + e*e);
}

void dump(const char* title, float8 * data, int size)
{
  printf("%s\n",title);
  for(int i = 0 ; i < size; i++)
    printf("%d: %g\n",i,data[i]);
}

void biased_autocorrelation(float8 * in, int size)
{    
  float8 * a = (float8*)fftw_malloc(size*2*sizeof(float8));
  for(int i = 0 ; i<size ; i++)
    {
      a[i]=in[i];
      a[i+size]=0;
    }
  fftw_plan forward = fftw_plan_r2r_1d(size*2,a,a,FFTW_R2HC,FFTW_ESTIMATE);
  fftw_plan backward = fftw_plan_r2r_1d(size*2,a,a,FFTW_HC2R,FFTW_ESTIMATE);
  fftw_execute(forward);
  for(int i = 0 ; i < 2 * size; i++)
    a[i]/=2*size;
  for(int i = 1 ; i < size; i++)
    a[i]=a[i]*a[i]+a[size*2-i]*a[size*2-i];
  a[0]*=a[0];
  a[size]*=a[size];
  for(int i = 1 ; i < size; i++)
    a[size+i]=0;
  fftw_execute(backward);
  for(int i = 0 ; i < size ; i ++)
    in[i]=a[i]*2*size;
  fftw_free(a);
  fftw_destroy_plan(forward);
  fftw_destroy_plan(backward);
}

void unbiased_autocorrelation(float8 * data, int size)
{
  biased_autocorrelation(data,size);
  for(int i = 0 ; i < size; i++)
    data[i]/=(size-i);
}

float8 biased_l2_distance(float8 * a, float8 * b, int size)
{
   float8 s = 0;
   for(int i = 0 ; i < size ; i++)
     {
	float8 d = a[i]-b[i];
	s+=d*d;
     }
   return s;
}

float8 biased_abs_distance(float8 * a, float8 * b, int size)
{
   float8 s = 0;
   for(int i = 0 ; i < size ; i++)
     {
       float8 d = fabs(a[i]-b[i]);
       s+=d*d;
     }
   return s;
}

float8 unbiased_abs_distance(float8 *a, float8 *b, int size)
{
  return biased_abs_distance(a,b,size)/size;
}

float8 unbiased_l2_distance(float8 * a, float8 * b, int size)
{
   return biased_l2_distance(a,b,size)/(float8)size;
}

float8 biased_best_l2_circular_distance(float8 *a,float8 *b, int size)
{
   float8 lowest = -1;
   for(int i = 0 ; i < size; i++)
     {
	float8 d1 = biased_l2_distance(a,b+size-i,i);
	float8 d2 = biased_l2_distance(a+i,b,size-i);
	float8 d = d1+d2;
	if (d<lowest || lowest<0)
	  lowest = d;
     }
   return lowest;
}

float8 biased_best_abs_circular_distance(float8 *a,float8 *b, int size)
{
   float8 lowest = -1;
   for(int i = 0 ; i < size; i++)
     {
       float8 d1 = biased_abs_distance(a,b+size-i,i);
       float8 d2 = biased_abs_distance(a+i,b,size-i);
       float8 d = d1+d2;
       if (d<lowest || lowest<0)
	 lowest = d;
     }
   return lowest;
}

void absolute(float8 * arr, int s)
{
   for(int i = 0 ; i < s; i ++)
     arr[i]=fabs(arr[i]);
}

void differentiate(float8 * arr, int s)
{
   for(int i = 0 ; i < s - 1; i ++)
     arr[i]=arr[i+1]-arr[i];
   arr[s-1]=-arr[s-1];
}

void energize(float8*data, float8 audiosize, unsigned4 fs)
{
  assert(fs>0);
  bpmdj_array(rr,fs,float4);
  for(unsigned4 i = 0 ; i < fs ; i++)
    rr[i]=0;
  float8 M = 0;
  for(unsigned4 x = 0 ; x < audiosize ; x ++)
    {
      M-=rr[x%fs];
      rr[x%fs]=data[x];
      M+=data[x];
      float8 R = M/fs;
      data[x>=fs ? x - fs : 0 ] -= R;
    }
  for(unsigned4 x = 0 ; x < audiosize ; x ++)
    data[x]*=data[x];
  for(unsigned4 i = 0 ; i < fs ; i++)
    rr[i]=0;
  float8 S = 0;
  for(unsigned4 x = 0 ; x < audiosize ; x ++)
    {
      S-=rr[x%fs];
      rr[x%fs]=data[x];
      S+=data[x];
      float8 R = sqrt(S/fs);
      data[x>=fs ? x - fs : 0 ] = R;
    }
  bpmdj_deallocate(rr);
}
#endif // __loaded__signals_cpp__
