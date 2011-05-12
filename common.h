/*
** $Id: common.h,v 1.2 2002/03/10 19:32:44 steudten Exp $
** Get rid of the different length of type long
**
** written by Thomas Steudten <thomas@steudten.com>
** January 25, 2002
** 
** $Log: common.h,v $
** Revision 1.2  2002/03/10 19:32:44  steudten
** Add lines for the short int (LONG2) values.
**
** Revision 1.1  2002/02/03 17:32:45  krubbens
** - change files to conform to the config rules
** - changed cbpm-player with signed8, unsigned8 etc.
**
*/

/* LONG8 */
#if defined(SIZEOF_LONG) && defined(SIZEOF_LONG_LONG)

#if SIZEOF_LONG == 8
#define LONG8 long
#elif SIZEOF_LONG == 4 && SIZEOF_LONG_LONG == 8
#define LONG8 long long
#else
#error unknown sizeof SIZEOF_LONG or SIZEOF_LONG_LONG
#endif

#else
#error SIZEOF_LONG or SIZEOF_LONG_LONG not defined 
#endif

/* LONG4 */
#if defined(SIZEOF_INT)

#if SIZEOF_INT == 4
#define LONG4 int 
#else
#error SIZEOF_INT not 4 
#endif

#else
#error SIZEOF_INT not defined 
#endif

/* LONG2 */
#if defined(SIZEOF_SHORT)

#if SIZEOF_SHORT == 2
#define LONG2 short 
#else
#error SIZEOF_SHORT not 2 
#endif

#else
#error SIZEOF_SHORT not defined 
#endif


/* WVB -- some typedef */
typedef signed   LONG8  signed8;
typedef unsigned LONG8  unsigned8;
typedef signed   LONG4  signed4;
typedef unsigned LONG4  unsigned4;
typedef signed   LONG2  signed2;
typedef unsigned LONG2  unsigned2;

/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001 Werner Van Belle
 See 'BeatMixing.ps' for more information

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

#define LONG8 long long int
#define LONG4 long int
#define LONG2 short int
/* WVB -- some typedef */
typedef signed   LONG8  signed8;
typedef unsigned LONG8  unsigned8;
typedef signed   LONG4  signed4;
typedef unsigned LONG4  unsigned4;
typedef signed   LONG2  signed2;
typedef unsigned LONG2  unsigned2;
