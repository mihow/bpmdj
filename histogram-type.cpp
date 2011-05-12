/****
 BpmDj v3.6: Free Dj Tools
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
#ifndef __loaded__histogram_type_cpp__
#define __loaded__histogram_type_cpp__
using namespace std;
#line 1 "histogram-type.c++"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "histogram-type.h"
#include "common.h"
#include "memory.h"
#include "signals.h"
#include "files.h"

histogram_type::histogram_type()
{
  left = 0;
  right = 0;
  sum = 0;
  scale = 0;
  total = 0;
  median = 0;
  bins = NULL;
  count = 0;
}

histogram_type::histogram_type(double l, double r, int c)
{
  init(l,r,c);
}

void histogram_type::init(double l, double r, int c)
{
  left = l;
  right = r;
  count = c;
  assert(right>left);
  bins = bpmdj_allocate(c,signed4);
  for(int i = 0 ; i < count; i++)
    bins[i]=0;
  sum = 0;
  total = 0;
}

int histogram_type::bin(double val)
{
  double x = (val - left) / (right - left);
  x *= count;
  int b = (int)x;
  if (b>=0 && b <count)
    return b;
  return -1;
}

void histogram_type::hit(double val)
{
  int b = bin(val);
  if (b>0)
    {
      sum+=val;
      total++;
      bins[b]++;
    }
}

void histogram_type::normalize(int val)
{
  int p = 0;
  int m = bins[0];
  for(int i = count-1 ; i >=0 ; i --)
    {
      if (bins[i]>m)
	{
	  p = i;
	  m = bins[i];
	}
    }
  median = p;
  vector_mul_div(bins,count,val,m);
}

void histogram_type::strip()
{
  int a = 0;
  int b = count - 1;
  while(bins[a]==0) a++;
  while(bins[b]==0) b--;
  double new_left = left + a * (right-left) / count;
  double new_right = left + (b+1) * (right-left) / count;
  int    new_count = b-a+1;
  if (new_count<=0) new_count=1;
  signed4 * new_bins = bpmdj_allocate(new_count,signed4);
  for(int i = a ; i <= b ; i++)
    new_bins[i-a]=bins[i];
  // printf("Stripped histogram from %d to %d\n",count,new_count);
  bpmdj_deallocate(bins);
  bins = new_bins;
  left = new_left;
  right = new_right;
  count = new_count;
}

double histogram_type::dev()
{
  double m = mean();
  double dev = 0;
  unsigned8 hits = 0;
  for(int i = 0; i < count ; i++)
    {
      hits+=bins[i];
      double d = (i*(right-left)/count) - m;
      dev+=d*d*bins[i];
    }
  dev/=hits;
  dev=sqrt(dev);
  return dev;
}

int histogram_type::valat(double v)
{
  int b = bin(v);
  if (b>=0)
    return bins[b];
  return 0;
}

/*double histogram_type::median_scaled(double mean)
{
  double answer = left + (right-left)*((double)median+0)/(double)count;
  return mean;
}*/

double histogram_type::mean()
{
  return sum/total;
}

void histogram_type::normalize_autocorrelation_diff(int val)
{
  // convert it to doubles
  int ns = higher_power_of_two(count);
  double *in = bpmdj_allocate(ns,double);
  for(int i = 0; i < count ;i++)
    in[i]=bins[i];
  for(int i = count ; i < ns; i++)
    in[i]=0;
  unbiased_autocorrelation(in,ns);
  ::normalize_abs_max<double>(in,ns);
  for(int i = 0 ; i < ns ; i++)
    in[i]=sqrt(fabs(in[i]));
  differentiate(in,ns);
  scale = find_abs_max(in,ns);
  normalize_abs_max(in,ns);
  for(int i = 0 ; i < count ; i++)
    bins[i]=(int)(in[i]*(double)val);
  bpmdj_deallocate(in);
}

double histogram_type::best_dist(histogram_type *a)
{
  int m = count > a->count ? count : a->count;
  double *A = bpmdj_allocate(m,double);
  double *B = bpmdj_allocate(m,double);
  for(int i = 0 ; i < m; i++)
    B[i]=A[i]=0;
  for(int i = 0; i < m ;i++)
    {
      if (i < count)
	A[i]=bins[i];
      if (i < a->count)
	B[i]=a->bins[i];
    }
  return biased_best_abs_circular_distance(A,B,m);
}

double histogram_type::cor_dist(histogram_type *a)
{

  // convert it to doubles
  int m = count > a->count ? count : a->count;
  int ns = higher_power_of_two(m);
  double *A = bpmdj_allocate(ns,double);
  double *B = bpmdj_allocate(ns,double);
  for(int i = 0 ; i < ns; i++)
    B[i]=A[i]=0;
  for(int i = 0; i < m ;i++)
    {
      if (i < count)
	A[i]=bins[i];
      if (i < a->count)
	B[i]=a->bins[i];
    }
  unbiased_autocorrelation(A,ns);
  unbiased_autocorrelation(B,ns);
  
  normalize_abs_max(A,ns);
  normalize_abs_max(B,ns);
  for(int i = 0 ; i < ns ; i ++)
    {
      A[i]=sqrt(fabs(A[i]));
      B[i]=sqrt(fabs(B[i]));
    }
  
  differentiate(A,ns);
  differentiate(B,ns);

  double d = unbiased_abs_distance(A,B,ns);
  return d/2.0;
}

void histogram_type::halve()
{
  count/=2;
  for(int i = 1 ; i < count ; i++)
    bins[i]=(bins[2*i]+bins[2*i+1])/2;
}

#endif // __loaded__histogram_type_cpp__
