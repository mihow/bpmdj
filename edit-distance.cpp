/****
 BpmDj v3.8: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

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
#ifndef __loaded__edit_distance_cpp__
#define __loaded__edit_distance_cpp__
using namespace std;
#line 1 "edit-distance.c++"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "common.h"
#include <assert.h>
#include "memory.h"

static int * * d;

int repl(char a, char b)
{
  if (a==b) return 0;
  return 1;
}

int min3(int a, int b, int c)
{
  if (a<=b && a<=c) return a;
  if (b<=a && b<=c) return b;
  if (c<=a && c<=b) return c;
  assert(0);
}

void dist_init()
{
  int i = 0;
  int j = 0;
  int m = 128;
  int n = 128;
  // create empty distance matrix
  d = bpmdj_allocate (  m + 1 , int * ) ;
  for ( i = 0 ; i <= m ; i ++ )
    d [ i ] = bpmdj_allocate( n + 1 , int ) ;
  // set startrows
  for ( i = 0 ; i <= m ; i ++ )
    d [ i ] [ 0 ] = i ;
  for ( j = 0 ; j <= n ; j ++ )
    d [ 0 ] [ j ] = j ;
}

void dist_done()
{
  int i = 0;
  // free everything
  for ( i = 0 ; i <= 128 ; i ++ )
    bpmdj_deallocate ( d [ i ] ) ;
  bpmdj_deallocate ( d ) ;
}

int edist(char* x, char* y)
{
  int m = strlen ( x ) ;
  int n = strlen ( y ) ;
  int i = 0;
  int j = 0;
  assert(m<128);
  assert(n<128);
  // loop matrix
  for( i = 1 ; i <= m ; i++ )
    for( j = 1 ; j <= n ; j++ )
      d[i][j]=min3(d[i-1][j-1] + repl(x[i-1],y[j-1]),
		   d[i-1][j] + 1,
		   d[i][j-1] + 1);
  // return ...
  return d [ m ] [ n ] ;
}

char* strupperdup(const char* in)
{
  int l = strlen(in);
  char* out = bpmdj_allocate(l+1,char);
  int r,w;
  for(r=0, w=0; r<l; r++)
    {
      if (isalnum(in[r]))
	out[w++]=toupper(in[r]);
    }
  out[w]=0;
  return out;
}

int ndist(const char* title, const char* author, const char* other)
{
  int tl = strlen ( title ) ;
  int al = strlen ( author ) ;
  char * x1 = bpmdj_allocate( tl + al + 1, char );
  char * x2 = bpmdj_allocate( tl + al + 1, char );
  char * x3 = bpmdj_allocate( tl + 1, char );
  char * y1, * y2, * y3;
  char * z = strupperdup(other);
  int result;
  strcpy ( x1 , title ) ;
  strcpy ( x1 + tl, author ) ;
  strcpy ( x2 , author ) ;
  strcpy ( x2 + al, title ) ;
  strcpy ( x3 , title ) ;
  y1=strupperdup(x1);
  y2=strupperdup(x2);
  y3=strupperdup(x3);
  result=min3(edist(y1,z),
	      edist(y2,z),
	      edist(y3,z));
  bpmdj_deallocate(z);
  bpmdj_deallocate(y1);
  bpmdj_deallocate(y2);
  bpmdj_deallocate(y3);
  bpmdj_deallocate(x1);
  bpmdj_deallocate(x2);
  bpmdj_deallocate(x3);
  return result;
}
#endif // __loaded__edit_distance_cpp__
