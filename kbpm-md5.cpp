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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include <dirent.h>
#include <assert.h>
#include <stdio.h>
#include "kbpm-played.h"
#include "kbpm-md5.h"
#include <ctype.h>

int Sums::size=1;
int Sums::next=1;
FILE *Sums::f=NULL;
char** Sums::names=NULL;
char** Sums::sums=NULL;

void Sums::Add(const char*line)
{
   /* put line in memory */
   char* c=strdup(line);
   names[next]=c+36;
   names[next][strlen(c+36)-1]=0;
   sums[next]=c;
   c[32]=0;
   next++;
   if (next>=size)
     {
	size*=2;
	names=(char**)realloc(names,size*sizeof(char*));
	sums=(char**)realloc(sums,size*sizeof(char*));
     }
}

Sums::Sums(const char* filename)
{
   /* initialise shit */
   size=1;
   next=0;
   names=(char**)malloc(sizeof(char*)*size);
   sums=(char**)malloc(sizeof(char*)*size);
   assert(names);
   /* read the file or create it */
   f=fopen(filename,"rb");
   /* read all lines in memory and sort them */
   if (f!=NULL)
     {
	char *line=NULL;
	size_t len;
	while(getline(&line,&len,f)!=-1)
	  Add(line);
	if (line)
	  free(line);
     }
}

char* strupperdup(const char* blah)
{
   char* result=strdup(blah);
   char* tracer=result;
   while(*tracer) 
     {
	*tracer=toupper(*tracer);
	tracer++;
     }
   return result;
}

char* Sums::Md5(const char * indexname)
{
   int i=0;
   char * kapitalname=strupperdup(indexname);
   sprintf(kapitalname+strlen(kapitalname)-4,".MP3");
   while(i<next)
     {
	//	printf("comparing %s and %s\n",names[i],kapitalname);
	if (strcmp(names[i],indexname)==0)
	  {
	     //	     printf("Hit returning %s\n",sums[i]);
	     return sums[i];
	  }
	char *temp=strupperdup(names[i]);
	if (strcmp(temp,kapitalname)==0)
	  {
	     //	     printf("Hit returning %s\n",sums[i]);
	     free(temp);
	     return sums[i];
	  }
	free(temp);
	i++;
     }
   return "";
}

