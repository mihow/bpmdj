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
#ifndef __loaded__bloody_moc_h__
#define __loaded__bloody_moc_h__
using namespace std;
#line 1 "bloody-moc.h++"
/**
 * Because some moron at trolltech has no idea how to parse a type
 * we can no longer use the unsigned1, unsigned2, unsigned4 and unsigned8
 * types directly. Instead we need to resort to a silly workaround in which
 * we make sure that the bloody moc compiler does not notice that we actually 
 * pass our favorite type along. Grmbl. I hate fools that think that they
 * can program and just go with 'let's just fix it like this'.
 * This said. We will be using wunsigned1 to 8 as typedefs for the other ones
 */
typedef unsigned8 wunsigned8;
typedef unsigned4 wunsigned4;
typedef unsigned2 wunsigned2;
typedef unsigned1 wunsigned1;
#endif // __loaded__bloody_moc_h__
