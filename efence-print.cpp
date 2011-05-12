/****
 BpmDj v4.2 beta: Free Dj Tools
 Copyright (C) 2001-2010 Werner Van Belle

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
#ifndef __loaded__efence_print_cpp__
#define __loaded__efence_print_cpp__
using namespace std;
#line 1 "efence-print.c++"
#ifdef EFENCE
/*
 * Electric Fence - Red-Zone memory allocator.
 * Bruce Perens, 1988, 1993
 * 
 * This is a special version of malloc() and company for debugging software
 * that is suspected of overrunning or underrunning the boundaries of a
 * malloc buffer, or touching free memory.
 *
 * It arranges for each malloc buffer to be followed (or preceded)
 * in the address space by an inaccessible virtual memory page,
 * and for free memory to be inaccessible. If software touches the
 * inaccessible page, it will get an immediate segmentation
 * fault. It is then trivial to uncover the offending code using a debugger.
 *
 * An advantage of this product over most malloc debuggers is that this one
 * detects reading out of bounds as well as writing, and this one stops on
 * the exact instruction that causes the error, rather than waiting until the
 * next boundary check.
 *
 * There is one product that debugs malloc buffer overruns
 * better than Electric Fence: "Purify" from Purify Systems, and that's only
 * a small part of what Purify does. I'm not affiliated with Purify, I just
 * respect a job well done.
 *
 * This version of malloc() should not be linked into production software,
 * since it tremendously increases the time and memory overhead of malloc().
 * Each malloc buffer will consume a minimum of two virtual memory pages,
 * this is 16 kilobytes on many systems. On some systems it will be necessary
 * to increase the amount of swap space in order to debug large programs that
 * perform lots of allocation, because of the per-buffer overhead.
 */

/**
 * WVB - since we do not debug the C library we can safely use existing 
 * functions
 */

#include <cstdio>
#include "efence.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>

void EF_Abort(const char * pattern, ...)
{
	va_list	args;

	va_start(args, pattern);

	EF_Print("\nElectricFence Aborting: ");
	vprintf(pattern, args);
	EF_Print("\n");
	
	va_end(args);

	/*
	 * I use kill(getpid(), SIGILL) instead of abort() because some
	 * mis-guided implementations of abort() flush stdio, which can
	 * cause malloc() or free() to be called.
	 */
	kill(getpid(), SIGILL);
	/* Just in case something handles SIGILL and returns, exit here. */
	_exit(-1);
}

void EF_Exit(const char * pattern, ...)
{
  va_list args;
  va_start(args, pattern);
  
  EF_Print("\nElectricFence Exiting: ");
  vprintf(pattern, args);
  EF_Print("\n");
  
  va_end(args);
  
  /*
   * I use _exit() because the regular exit() flushes stdio,
   * which may cause malloc() or free() to be called.
   */
  _exit(-1);
}

void
EF_Print(const char * pattern, ...)
{
	va_list	args;

	va_start(args, pattern);
	vprintf(pattern, args);
	va_end(args);
}

#endif
#endif // __loaded__efence_print_cpp__
