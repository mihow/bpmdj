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

#ifndef HISTOGRAM_TYPE_H
#define HISTOGRAM_TYPE_H

#include "basic-types.h"
#include "common.h"

class histogram_type
{
 public:
  double left;
  double right;
  double sum;
  double scale;
  int    total;
  int    median;
  signed4 * bins;
  int    count;
  histogram_type();
  histogram_type(double left, double right, int c);
  void init(double left, double right, int c);
  int bin(double val);
  void hit(double val);
  void normalize(int val);
  void normalize_autocorrelation_diff(int val);
  void strip();
  int valat(double v);
  double mean();
  double dev();
  double best_dist(histogram_type *a);
  double cor_dist(histogram_type *a);
  void halve();
};

#endif
