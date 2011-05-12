/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle
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

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include "malloc.h"
#include "dirscanner.h"
#include "common.h"
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

long fsize(FILE * f)
{
   long answer, pos;
   assert(f);
   pos = ftell(f);
   fseek(f,0,SEEK_END);
   answer=ftell(f);
   fseek(f,pos,SEEK_SET);
   return answer;
}

void common_init()
{
   assert(sizeof(int)==4);
   assert(sizeof(signed2)==2);
   assert(sizeof(signed4)==4);
   assert(sizeof(signed8)==8);
}

long readsamples(void* target, int count, FILE* file)
{
  int result;
  assert(target);
  assert(file);
  assert(count>0);
  result = fread(target,sizeof(unsigned4),count,file);
  if (result<=0)
    {
      int err = ferror(file);
      if (feof(file)) 
	return 0;
      printf("file: Could not read %d samples, errno = %d (%s)\n",count,err,strerror(err));
      assert(0);
    }
  return result;
}

long writesamples(void* target, int count, FILE* file)
{
  int result;
  assert(target);
  assert(file);
  if (!count)
    return count;
  assert(count>0);
  result = fwrite(target,sizeof(unsigned4),count,file);
  if (result<=0)
    {
      int err = ferror(file);
      printf("file: Could not write %d samples, errno = %d (%s)\n",count,err,strerror(err));
    }
  assert(result>0);
  return result;
}

bool strxeq(const char* a, const char* b)
{
  if (a==NULL || !*a)
    return (b==NULL || !*b);
  if (b==NULL || !*b)
    return false;
  return strcmp(a,b)==0;
}

char* findUniqueName(const char* filename)
{
  char indexname[500];
  char *temp;
  char halfindexname[500];
  temp=strdup(basename(strdup(filename)));
  temp[strlen(temp)-4]=0;
  sprintf(halfindexname,"%s.idx",temp);
  sprintf(indexname,"./index/%s.idx",temp);
  int nr=2;
  while(exists(indexname))
    {
      sprintf(halfindexname,"%s%d.idx",temp,nr);
      sprintf(indexname,"./index/%s%d.idx",temp,nr++);
    }
  return strdup(indexname);
}

bool exists(const char* fn)
{
  FILE * f = fopen(fn,"rb");
  if (f)
    {
      fclose(f);
      return true;
    }
  return false;
}

double minimum(double a, double b)
{
  if ( a < b ) return a;
  else return b;
}

double abs_minimum(double a, double b)
{
  if ( fabs(a) < fabs(b) ) return a;
  else return b;
}

static int atof_called=0;
double atod(const char* str)
{
  if ((++atof_called%10000)==0)
    printf("atof calls %d\n",atof_called);
  return atof(str);
}

char * tohex(long i)
{
  char r[9];
  r[8]=0;
  for(int p = 7 ; p >= 0 ; p --)
    {
      int d = i%16;
      i/=16;
      if (d<10)
	r[p]='0'+d;
      else
	r[p]='a'-10+d;
    }
  return strdup(r);
}

long toint(const char* name)
{
  long result = 0;
  for (int j = 0 ; j < 8 ; j ++)
    {
      int c = name[j];
      if (c>='0' && c <= '9')
	result = result*16+c-'0';
      else  if (c>='a' && c<='f')
	result = result*16+c-'a'+10;
      else  if (c>='A' && c<='F')
	result = result*16+c-'A'+10;
      else assert(0);
    }
  return result;
}


int clip(int val)
{
  if (val<0) return -1;
  if (val>0) return +1;
  return 0;
}
