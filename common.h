/*
** $Id: common.h,v 1.1 2002/02/03 17:32:45 krubbens Exp $
** Get rid of the different length of type long
**
** written by Thomas Steudten <thomas@steudten.com>
** January 25, 2002
** 
** $Log: common.h,v $
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
#error SIZEOF_LONG or SIZEOF_LONG_LONG not defined (__FILE__) 
#endif

/* LONG4 */
#if defined(SIZEOF_INT)

#if SIZEOF_INT == 4
#define LONG4 int 
#endif

#else
#error SIZEOF_INT not defined (__FILE__) 
#endif

/* WVB -- some typedef */
typedef signed   LONG8  signed8;
typedef unsigned LONG8  unsigned8;
typedef signed   LONG4  signed4;
typedef unsigned LONG4  unsigned4;
/* WVB -- probably highly incorrect on some machines !! */
typedef signed   short  signed2;
typedef unsigned short  unsigned2;
