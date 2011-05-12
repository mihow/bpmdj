/****
 Hierarchical Data Objects
 Copyright (C) 2005-2011 Werner Van Belle
 http://flow.yellowcouch.org/data/

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
#ifndef __loaded__array_test_cpp__
#define __loaded__array_test_cpp__
using namespace std;
#line 1 "array-test.c++"
#include <time.h>
#include <stdio.h>
#include "om-data.h"

int main(int, char*[])
{
  time_t st;
  printf("Picture testing...\n");
  typedef Array<3,unsigned char> Image;
  typedef Array<2,unsigned char> Plane;
  const int COLOR = 0;
  const int X = 1;
  const int Y = 2;
  Array<3,unsigned char> picture(3,1024,768);
  picture = 2;
  printf("  Normal iteration loop\n");
  st = time(NULL);
  int nr = 0;
  int r = 0;
  int g = 0;
  int b = 0;
  printf("  %d [%d,%d,%d] times\nExtra optimized loop\n",nr,r,g,b);
  st = time(NULL);
  nr = 0;
  picture(From<3>(0,0,0))(Select<2>(1,2))=1;
  picture(From<3>(1,0,0))(Select<2>(1,2))=2;
  picture(From<3>(2,0,0))(Select<2>(1,2))=3;

  while(time(NULL)-st<10)
    {
      nr++;
      b = g = r = 0;
      Image::matrix_positions color(picture,COLOR);
      Plane::values xy(picture(Select<2>(X,Y)));
      for( ; color.more() ; ++color)
	{
	  int v = 0;
	  for(xy.init(color) ; xy.more() ; ++xy)
	    v+=xy;
	  if (color[COLOR]==0) r = v;
	  else if (color[COLOR]==1) g = v;
	  else if (color[COLOR]==2) b = v;
 	  else assert(0);
	}
    }
  printf("  %d [%d,%d,%d] times\nOptimized loop\n",nr,r,g,b);
  st = time(NULL);
  nr = 0;
  while(time(NULL)-st<10)
    {
      nr++;
      b = g = r = 0;
      for(Image::matrix_positions colors(picture,COLOR) ; colors.more() ; 
	  ++colors)
	{
	  int v = 0;
	  Image::matrix &colorplane = colors;
	  for(Image::matrix::values chunks(colorplane); chunks.more() ; 
	      ++chunks)
	    v+=chunks;
	  if (colors[0]==0) r = v;
	  else if (colors[0]==1) g = v;
	  else if (colors[0]==2) b = v;
	  else assert(0);
	}
    }
  printf("  %d [%d,%d,%d] times\n",nr,r,g,b);
}
#endif // __loaded__array_test_cpp__
