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

#include "basic-types.h"

signed8 higher_power_of_two(signed8 a);
double find_max(double * data, long l);
long   find_max_rpos(unsigned4 * data, long l);
double find_mean(double * data, long l);
float  find_mean(float * data, long l);
double find_abs_max(double * data, long l);
float  find_abs_max(float * data, long l);
void   normalize_abs_max(float * data, long l);
double normalize_abs_max(double * data, long l);
void   vector_mul_div(signed4 * data, long l, signed4 mul, signed4 div);
double normalize_mean(double * data, long l);
float  normalize_mean(float * data, long l);
void   translate_mean(float * data, long l);
double norm(double r, double e);
void   differentiate(double *r, int size);
/* sets r[i]=r[i+1]-r[i] */

void   absolute(double *r, int size);
/* sets r[i]=abs(r[i]) */

void   biased_circular_autocorrelation(double *arr, int size);
/* The biased autocorrelation will return the circular autocorrelation, or
   out[i]=sum_j in[i]*in[(j+i)%size] */

void   biased_autocorrelation(double *arr, int size);
/* The biased autocorrelation will return the non-circular autocorrelation, or
   out[i]=sum_j in[i]*in[j+i] */

void   unbiased_autocorrelation(double * arr, int size);
/* The unbiased autocorrelation will return the non-circular autocorrelation 
   taking into account the amount of actual multiplications performed.
   out[i]=autocor[i]/(size-i) */

double biased_abs_distance(double * a, double * b, int size);
double unbiased_abs_distance(double * a, double * b, int size);
/* returns |a[i]-b[i]| [ /size ] */

double biased_l2_distance(double * a, double * b, int size);
/* returns (a[i]-b[i])^2 */

double unbiased_l2_distance(double * a, double * b, int size);
/* returns biased_l2_distance / size */

double biased_best_abs_circular_distance(double *a, double *b, int size);
double biased_best_l2_circular_distance(double *a, double *b, int size);
/* finds the best circular shift to give the lowest L2 distance */

void   test_signals();
