/****
 BpmDj v4.2-pl2: Free Dj Tools
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
#ifndef __loaded__pca_h__
#define __loaded__pca_h__
using namespace std;
#include "Data/types.h"
void do_pca(int rows, int columns, float4**data, const char* &error_msg);
float4 ** matrix(int rows, int columns);
void free_matrix(float4 **data, int rows, int columns);
#endif // __loaded__pca_h__
