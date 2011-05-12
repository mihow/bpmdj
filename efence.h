/*
 * Electric Fence - Red-Zone memory allocator.
 * Bruce Perens, 1988, 1993
 * 
 * This is a special version of malloc() and company for debugging software
 * that is suspected of overrunning or underrunning the boundaries of a
 * malloc buffer, or touching free memory.
 *
 * It arranges for each malloc buffer to be followed (or preceded)
 * in the address space by an inaccessable virtual memory page,
 * and for free memory to be inaccessable. If software touches the
 * inaccessable page, it will get an immediate segmentation
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

#ifdef EFENCE
#include <sys/types.h>
#include <sys/param.h>

/*
 * ef_number is the largest unsigned integer we'll need. On systems that
 * support 64-bit pointers, this may be "unsigned long long".
 */
#if defined(USE_LONG_LONG)
typedef unsigned long long	ef_number;
#else
typedef unsigned long		ef_number;
#endif

/*
 * NBBY is the number of bits per byte. Some systems define it in
 * <sys/param.h> .
 */
#ifndef	NBBY
#define	NBBY	8
#endif

void			Page_AllowAccess(void * address, size_t size);
void *			Page_Create(size_t size);
void			Page_Delete(void * address, size_t size);
void			Page_DenyAccess(void * address, size_t size);
size_t			Page_Size(void);

void			EF_Abort(const char * message, ...);
void			EF_Exit(const char * message, ...);
void			EF_Print(const char * message, ...);
void			EF_Lock();
void			EF_UnLock();

void * efence_malloc(size_t);
void * efence_realloc(void*,size_t);
bool   efence_free(void*);

#endif