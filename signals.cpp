/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

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
using namespace std;
#line 1 "signals.c++"
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <fftw3.h>
#include "common.h"
#include "signals.h"
#include "memory.h"

BasicConvertor::BasicConvertor() {};

double Index_to_frequency (unsigned NumSamples, unsigned Index)
{
  if ( Index >= NumSamples )
    return 0.0;
  else if ( Index <= NumSamples/2 )
    return (double)Index / (double)NumSamples;
  return -(double)(NumSamples-Index) / (double)NumSamples;
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

double find_max(double * data, long l)
{
  double m = 0;
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

double find_mean(double * data, long l)
{
  if (l==0) return 0;
  double m = 0;
  for(long i = 0 ; i < l ; i ++)
    m+=data[i];
  return m/(double)l;
}

float find_mean(float * data, long l)
{
  if (l==0) return 0;
  float m = 0;
  for(long i = 0 ; i < l ; i ++)
    m+=data[i];
  return m/(double)l;
}

void translate_mean(float * data, long l)
{
  float m = find_mean(data,l);
  for(int i = 0 ; i < l ; i ++)
    data[i]-=m;
}

void normalize_mean(double * data, long l)
{
  double m = find_mean(data,l);
  printf("normalizer: mean is %g\n",m);
  if (m>0)
    for(long i = 0 ; i < l ; i ++)
      data[i]/=m;
}

void normalize_mean(float * data, long l)
{
  float m = find_mean(data,l);
  if (m>0)
    for(long i = 0 ; i < l ; i ++)
      data[i]/=m;
}

double norm(double r, double e)
{
  return sqrt(r*r + e*e);
}

void dump(const char* title, double * data, int size)
{
  printf("%s\n",title);
  for(int i = 0 ; i < size; i++)
    printf("%d: %g\n",i,data[i]);
}

void biased_autocorrelation(double * in, int size)
{    
  double * a = (double*)fftw_malloc(size*2*sizeof(double));
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

void unbiased_autocorrelation(double * data, int size)
{
  biased_autocorrelation(data,size);
  for(int i = 0 ; i < size; i++)
    data[i]/=(size-i);
}

double biased_l2_distance(double * a, double * b, int size)
{
   double s = 0;
   for(int i = 0 ; i < size ; i++)
     {
	double d = a[i]-b[i];
	s+=d*d;
     }
   return s;
}

double biased_abs_distance(double * a, double * b, int size)
{
   double s = 0;
   for(int i = 0 ; i < size ; i++)
     {
       double d = fabs(a[i]-b[i]);
       s+=d*d;
     }
   return s;
}

double unbiased_abs_distance(double *a, double *b, int size)
{
  return biased_abs_distance(a,b,size)/size;
}

double unbiased_l2_distance(double * a, double * b, int size)
{
   return biased_l2_distance(a,b,size)/(double)size;
}

double biased_best_l2_circular_distance(double *a,double *b, int size)
{
   double lowest = -1;
   for(int i = 0 ; i < size; i++)
     {
	double d1 = biased_l2_distance(a,b+size-i,i);
	double d2 = biased_l2_distance(a+i,b,size-i);
	double d = d1+d2;
	if (d<lowest || lowest<0)
	  lowest = d;
     }
   return lowest;
}

double biased_best_abs_circular_distance(double *a,double *b, int size)
{
   double lowest = -1;
   for(int i = 0 ; i < size; i++)
     {
       double d1 = biased_abs_distance(a,b+size-i,i);
       double d2 = biased_abs_distance(a+i,b,size-i);
       double d = d1+d2;
       if (d<lowest || lowest<0)
	 lowest = d;
     }
   return lowest;
}

void absolute(double * arr, int s)
{
   for(int i = 0 ; i < s; i ++)
     arr[i]=fabs(arr[i]);
}

void differentiate(double * arr, int s)
{
   for(int i = 0 ; i < s - 1; i ++)
     arr[i]=arr[i+1]-arr[i];
   arr[s-1]=-arr[s-1];
}

void energize(double*data, double audiosize, unsigned4 fs)
{
  assert(fs>0);
  bpmdj_array(rr,fs,float);
  for(unsigned4 i = 0 ; i < fs ; i++)
    rr[i]=0;
  double M = 0;
  for(unsigned4 x = 0 ; x < audiosize ; x ++)
    {
      M-=rr[x%fs];
      rr[x%fs]=data[x];
      M+=data[x];
      double R = M/fs;
      data[x>=fs ? x - fs : 0 ] -= R;
    }
  for(unsigned4 x = 0 ; x < audiosize ; x ++)
    data[x]*=data[x];
  for(unsigned4 i = 0 ; i < fs ; i++)
    rr[i]=0;
  double S = 0;
  for(unsigned4 x = 0 ; x < audiosize ; x ++)
    {
      S-=rr[x%fs];
      rr[x%fs]=data[x];
      S+=data[x];
      double R = sqrt(S/fs);
      data[x>=fs ? x - fs : 0 ] = R;
    }
  bpmdj_deallocate(rr);
}

/*
int main(int argc, char*argv[])
{
  double data[8];
  data[0]=1;
  data[1]=-2;
  data[2]=3;
  data[3]=-4;
  data[4]=10;
  data[5]=-20;
  data[6]=30;
  data[7]=-44;
  biased_autocorrelation(data,8);
  dump("autocorrelated",data,8);
  // in this test we verify whether the subtraction of the ubiased autocorrelation relates
  // to the best L2 match

  
  double *a = bpmdj_allocate(512,double);
  double *b = bpmdj_allocate(512,double);
  for(int test = 0 ; test < 1000 ; test++)
    {
      for(int i = 0 ; i < 512; i ++)
	{
	  double r = (double)random()/(double)RAND_MAX;
	     a[i]=sin(r+2*3.14*(double)(i-256)/512.0);
	     a[i]*=1+0.5*(double)random()/(double)RAND_MAX;
	     r = (double)random()/(double)RAND_MAX;
	     b[i]=sin(r+2*3.14*(double)(i-256)*(1.0+(double)test/50.0)/512.0);
	     
	     b[i]*=1+0.5*(double)random()/(double)RAND_MAX;
	     
	     // change total energy content
	     r = (double)random()/(double)RAND_MAX;
	     a[i]*=r;
	     r = (double)random()/(double)RAND_MAX;
	     b[i]*=r;
	     
	     // now the function will have a clear start and stop
	     a[i]*=sin(3.14*(double)i/512);
	     b[i]*=sin(3.14*(double)i/512);
	  }
	absolute(a,512);
	absolute(b,512);
	double l2d = biased_best_l2_circular_distance(a,b,512);
	unbiased_autocorrelation(a,512);
	unbiased_autocorrelation(b,512);
	normalize_abs_max(a,512);
	normalize_abs_max(b,512);
	double a2d = unbiased_l2_distance(a,b,512);
	printf("%d %g %g\n",test,l2d,a2d);
     }
}
*/
