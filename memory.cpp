#include <stdio.h>
#include <assert.h>
#include "memory.h"
#include "malloc.h"
#include "efence.h"

void myfree(void* a)
{
#ifdef EFENCE
  if (!efence_free(a))
#endif
    free(a);
}

void* myalloc(int length, char* file, int line)
{
  void * result;
  // printf("%s(%d): allocating %d bytes\n",file,line,length);
  assert(length>=0);
#ifdef EFENCE
  result = efence_malloc(length);
#else
  result = malloc(length);
#endif
#ifdef COUNT_ALLOCATIONS
  static int nrallocs = 0;
  nrallocs++;
  if (nrallocs%100==0)
    printf("%d allocs ",nrallocs);
#endif
  if (!result)
    printf("Error: %s(%d): unable to allocate %d bytes \n",file,line,length);
  assert(result);
  return result;
}

void* myrealloc(void* thing, int size)
{
  void * result;
  assert(size);
#ifdef EFENCE
  result = efence_realloc(thing,size);
#else
  result = realloc(thing,size);
#endif
  assert(result);
  return result;
}

