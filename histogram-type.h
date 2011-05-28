/****
 BpmDj v4.2: Free Dj Tools
 Copyright (C) 2001-2011 Werner Van Belle

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
#ifndef __loaded__histogram_type_h__
#define __loaded__histogram_type_h__
using namespace std;
#line 1 "histogram-type.h++"
#include "common.h"

class histogram_type
{
 public:
  float8 left;
  float8 right;
  float8 sum;
  float8 scale;
  int    total;
  int    median;
  signed4 * bins;
  int    count;
  histogram_type();
  histogram_type(float8 left, float8 right, int c);
  void init(float8 left, float8 right, int c);
  int bin(float8 val);
  void hit(float8 val);
  void normalize(int val);
  void normalize_autocorrelation_diff(int val);
  void strip();
  int valat(float8 v);
  float8 mean();
  float8 dev();
  float8 best_dist(histogram_type *a);
  float8 cor_dist(histogram_type *a);
  void halve();
};
#endif // __loaded__histogram_type_h__
