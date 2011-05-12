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

int Played::size=1;
int Played::next=1;
FILE *Played::f=NULL;
char** Played::names=NULL;

void Played::Add(const char*filename)
{
   /* put line in memory */
   names[next]=strdup(filename);
   names[next][strlen(names[next])-1]=0;
   next++;
   if (next>=size)
     {
	size*=2;
	names=(char**)realloc(names,size*sizeof(char*));
     }
}

Played::Played(const char* filename)
{
   /* initialise shit */
   size=1;
   next=0;
   names=(char**)malloc(sizeof(char*)*size);
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
   /* open file for appending */
   f=fopen(filename,"ab");
}

bool Played::IsPlayed(const char * indexname)
{
   int i=0;
   while(i<next)
     {
	if (strcmp(names[i],indexname)==0) 
	  return true;
	i++;
     }
   return false;
}

void Played::Play(const char* indexname)
{
   fprintf(f,"%s\n",indexname);
   fflush(f);
}
