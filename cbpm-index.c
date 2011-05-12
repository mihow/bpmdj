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

/*-------------------------------------------
 *         Headers
 *-------------------------------------------*/
#include "config.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */

#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif /* HAVE_STDIO_H */

#ifdef HAVE_TERMIOS_H
#include <termios.h>
#endif /* HAVE_TERMIOS_H */

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif /* HAVE_FCNTL_H */

#ifdef HAVE_LIBGEN_H
#include <libgen.h>
#endif /* HAVE_LIBGEN_H */

#ifdef HAVE_LINUX_SOUNDCARD_H
#include <linux/soundcard.h>
#endif /* HAVE_LINUX_SOUNDCARD_H */

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif /* HAVE_SIGNAL_H */

#ifdef HAVE_TIME_H
#include <time.h>
#endif /* HAVE_TIME_H */

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif /* HAVE_ASSERT_H */

#include "common.h"

/*-------------------------------------------
 *         Index fields
 *-------------------------------------------*/
char   * index_readfrom = NULL;
char * * index_comments;
 int     index_nextcomment;
char   * index_version;
 int     index_period;
char   * index_tempo;
char   * index_file;
char   * index_remark;
char   * index_tags;
char   * index_md5sum;
 int     index_changed;
 int     index_bpmcount_from;
 int     index_bpmcount_to;
unsigned long index_cue;
unsigned long index_cue_z;
unsigned long index_cue_x;
unsigned long index_cue_c;
unsigned long index_cue_v;
#define  maxcomments (5000)
/*-------------------------------------------
 *         Index operations
 *-------------------------------------------*/
void index_init()
{
   index_version=NULL;
   index_comments=calloc(maxcomments,sizeof(char*));
   index_nextcomment=0;
   index_period=-1;
   index_file=0;
   index_tags=NULL;
   index_md5sum=NULL;
   index_tempo=0;
   index_remark=0;
   index_changed=0;
   index_bpmcount_from=-1;
   index_bpmcount_to=-1;
   index_cue_z=0;
   index_cue_x=0;
   index_cue_c=0;
   index_cue_v=0;
}

void index_free()
{
   int i;
   if (index_readfrom) free(index_readfrom);
   index_readfrom=NULL;
   if (index_version) free(index_version);
   if (index_remark) free(index_remark);
   if (index_file) free(index_file);
   if (index_tempo) free(index_tempo);
   if (index_tags) free(index_tags);
   for(i=0;i<index_nextcomment;i++)
     if (index_comments[i]) free(index_comments[i]);
   if (index_comments) free(index_comments);
   if (index_md5sum) free(index_md5sum);
}

void index_write()
{
   int i;
   FILE*f;
   if (!index_readfrom)
     {
	printf("Error: no name given to write index file to\n");
	return;
     }
   f=fopen(index_readfrom,"wb");
   if (!f)
     {
	printf("Warning: cannot open %s for writing\n",index_readfrom);
	return;
     }
   fprintf(f,"version  : %s\n",index_version);
   fprintf(f,"file     : %s\n",index_file);
   fprintf(f,"period   : %d\n",index_period);
   fprintf(f,"tempo    : %s\n",index_tempo);
   if (index_tags) fprintf(f,"tag      : %s\n",index_tags);
   if (index_cue>0) fprintf(f,"cue      : %ld\n",index_cue);
   if (index_cue_z>0) fprintf(f,"cue-z    : %ld\n",index_cue_z);
   if (index_cue_x>0) fprintf(f,"cue-x    : %ld\n",index_cue_x);
   if (index_cue_c>0) fprintf(f,"cue-c    : %ld\n",index_cue_c);
   if (index_cue_v>0) fprintf(f,"cue-v    : %ld\n",index_cue_v);
   if (index_bpmcount_from>=0) fprintf(f,"bpmcount-from : %d\n",index_bpmcount_from);
   if (index_bpmcount_to>=0) fprintf(f,"bpmcount-to   : %d\n",index_bpmcount_to);
   if (index_md5sum) fprintf(f,"md5sum : %s\n",index_md5sum);
   if (index_remark) fprintf(f,"remark   : %s\n",index_remark);
   for(i=0;i<index_nextcomment;i++) fprintf(f,"%s\n",index_comments[i]);
   fclose(f);
}

char* strip_begin(char*tostrip)
{
   while(*tostrip && (*tostrip==' ' || *tostrip=='\t' || *tostrip=='\n')) tostrip++;
   return tostrip;
}

char* strip_end(char*tostrip,char*theend)
{
   int c;
   while(theend>=tostrip && (c=*theend) && (c==' ' || c=='\t' || c=='\n')) theend--;
   *(theend+1)=0;
   return tostrip;
}

char* strip(char*tostrip)
{
   tostrip=strip_begin(tostrip);
   return strip_end(tostrip,tostrip+strlen(tostrip)-1);
}

void index_addcomment(char* line)
{
   index_comments[index_nextcomment++]=strdup(line);
   assert(index_nextcomment<maxcomments);
}

void index_delete(int idx)
{
   int t;
   --index_nextcomment;
   for(t=idx;t<index_nextcomment;t++)
     index_comments[t]=index_comments[t+1];
}

void index_read(char* indexn)
{
   char *line=NULL;
   int linesiz=0;
   int read;
   // open file
   FILE* index;
   index_readfrom=strdup(indexn);
   index=fopen(index_readfrom,"rb");
   if (!index) 
     {
	printf("Couldn't read %s\n",indexn);
	exit(5);
     }
   // clear all fields
   index_init();
   // read fields
   while((read=getline(&line,&linesiz,index))!=-1)
     {
	char *field, *c=strip(line), *value;
	if (*c=='#') 
	  {  // ignore and store comments
	     index_addcomment(line);
	     free(line);
	     line=NULL;
	     continue;  
	  }
	while(*c!=':' && *c!=0) c++;
	if (!*c) 
	  {
	     // ignore non-field lines
	     index_addcomment(line);
	     free(line);
	     line=NULL;
	     continue;
	  }
	*c=0;
	field=strip_end(line,c-1);
	value=strip_begin(c+1);
	if (strcasecmp(field,"version")==0) index_version=strdup(value);
	else if(strcasecmp(field,"period")==0) index_period=atoi(value);
	else if(strcasecmp(field,"file")==0) index_file=strdup(value);
	else if(strcasecmp(field,"tempo")==0) index_tempo=strdup(value);
	else if(strcasecmp(field,"bpmcount-to")==0) index_bpmcount_to=atoi(value);
	else if(strcasecmp(field,"bpmcount-from")==0) index_bpmcount_from=atoi(value);
	else if(strcasecmp(field,"comment")==0) index_remark=strdup(value);
	else if(strcasecmp(field,"cue")==0) index_cue=atol(value);
	else if(strcasecmp(field,"cue-z")==0) index_cue_z=atol(value);
	else if(strcasecmp(field,"cue-x")==0) index_cue_x=atol(value);
	else if(strcasecmp(field,"cue-c")==0) index_cue_c=atol(value);
	else if(strcasecmp(field,"cue-v")==0) index_cue_v=atol(value);
	else if(strcasecmp(field,"md5sum")==0) index_md5sum=strdup(value);
	else if(strcasecmp(field,"tag")==0) 
	  {
	     if (!index_tags) index_tags=strdup(value);
	     else 
	       {
		  char tmp[1000];
		  sprintf(tmp,"%s %s",index_tags,value);
		  free(index_tags);
		  index_tags=strdup(tmp);
	       }
	  }
	else 
	  {
	     printf("Warning: Unknown field %s\n",field);
	     index_addcomment(line);
	     continue;
	  }
	free(line);
	line=NULL;
     }
   if (line) 
     free(line);
   fclose(index);
   // check for old non-versioned files
   if (!index_version)
     {
	char * y;
	int idx;
	printf("Warning: old index file %s\n",basename(index_readfrom));
	// search for "# .* - ";
	for(idx=0;idx<index_nextcomment;idx++)
	  {
	     line=index_comments[idx];
	     if (strstr(line," - "))
	       {
		  index_period=atoi(line+2);
		  index_comments[idx]=strdup("");
		  continue;
	       }
	     if (line[0]=='#')
	       index_delete(idx--);
	  }
	// fix some other fields
	index_file=strdup(basename(index_readfrom));
	y=strstr(index_file,".idx");
	assert(y);
	strcpy(y,".mp3");
	index_changed=1;
     }
   // fix fields
   if (!index_version)
     {
	char version[500];
	sprintf(version,"BpmDj v%d.%d",MAJOR_VERSION,MINOR_VERSION);
	index_changed=1;
	index_version=strdup(version);
     }
   if (index_period==-1)
     {
	printf("Error: no valid period given\n");
	exit(40);
     }
   // update tempo
     {
	char tempo[500];
	double T=4.0*(double)11025*60.0/(double)index_period;
	if (T>=100.0)
	  sprintf(tempo,"%g",T);
	else 
	  sprintf(tempo,"0%g",T);
	if (!index_tempo || strcmp(index_tempo,tempo)!=0)
	  {
	     index_tempo=strdup(tempo);
	     index_changed=1;
	  }
     }
}
