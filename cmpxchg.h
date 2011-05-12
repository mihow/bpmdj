#ifndef __loaded__cmpxchg_h__
#define __loaded__cmpxchg_h__
using namespace std;
#line 1 "cmpxchg.h++"
/**
 * Taken from the standard glibc headers: asm-x86_64/system.h
 * Including the standard asm headers never seemed to work properly.
 * - they use the keyword new which is offending C++
 * - in a newer version (mid-january 2007), the assembly code was even wrong
 */
#ifndef __HAVE_ARCH_CMPXCHG
#ifdef CONFIG_SMP
#define LOCK_PREFIX \
                ".section .smp_locks,\"a\"\n"   \
                "  .align 8\n"                  \
                "  .quad 661f\n" /* address */  \
                ".previous\n"                   \
                "661:\n\tlock; "
#else /* ! CONFIG_SMP */
#define LOCK_PREFIX ""
#endif

#define __xg(x) ((volatile long *)(x))

static inline unsigned long __cmpxchg(volatile void *ptr, unsigned long old,
                                      unsigned long nieuw, int size)
{
        unsigned long prev;
        switch (size) {
        case 1:
                __asm__ __volatile__(LOCK_PREFIX "cmpxchg %b1,%2"
                                     : "=a"(prev)
                                     : "q"(nieuw), "m"(*__xg(ptr)), "0"(old)
                                     : "memory");
                return prev;
        case 2:
                __asm__ __volatile__(LOCK_PREFIX "cmpxchg %w1,%2"
                                     : "=a"(prev)
                                     : "r"(nieuw), "m"(*__xg(ptr)), "0"(old)
                                     : "memory");
                return prev;
        case 4:
                __asm__ __volatile__(LOCK_PREFIX "cmpxchg %k1,%2"
                                     : "=a"(prev)
                                     : "r"(nieuw), "m"(*__xg(ptr)), "0"(old)
                                     : "memory");
                return prev;
        case 8:
                __asm__ __volatile__(LOCK_PREFIX "cmpxchg %1,%2"
                                     : "=a"(prev)
                                     : "r"(nieuw), "m"(*__xg(ptr)), "0"(old)
                                     : "memory");
                return prev;
        }
        return old;
}

#define cmpxchg(ptr,o,n)\
   ((__typeof__(*(ptr)))__cmpxchg((ptr),(unsigned long)(o),\
                                     (unsigned long)(n),sizeof(*(ptr))))
#endif
#endif // __loaded__cmpxchg_h__
