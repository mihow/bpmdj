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

#include <stdio.h>
#include "basic-types.h"
// meta operations on files
long   fsize(FILE * f);

// sample operations on files
long   readsamples(void* target, int count, FILE* file);
long   writesamples(void* target, int count, FILE* file);

// writing
void file_signed4  (signed4     i, FILE * f);
void file_float4   (float4      i, FILE * f);
void file_float8   (float8      i, FILE * f);
void file_unsigned1(unsigned1   i, FILE * f);
void file_sequence (unsigned1 * a, int length, FILE *f);

// reading
float4    buffer_float4();
unsigned1 buffer_unsigned1();
void      buffer_sequence(unsigned1 *a, int length);

