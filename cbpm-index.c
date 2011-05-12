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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <libgen.h>
#include <linux/soundcard.h>
#include <signal.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "version.h"

/*-------------------------------------------
 *         Performance operations
 *-------------------------------------------*/
static char* buffer = NULL;
static long int buffer_size;
static long int buffer_ptr;
static FILE* buffer_file;

static void buffer_init(FILE * stream)
{
   // read new buffer
   fseek(stream,0,SEEK_END);
   buffer_size=ftell(stream);
   buffer=malloc(buffer_size+1);
   assert(buffer);
   // read file
   fseek(stream,0,SEEK_SET);
   fread(buffer,buffer_size,1,stream);
   // sentinel.. so nextchar can work faster
   buffer[buffer_size]='\n';
   buffer_ptr=0;
   buffer_file=stream;
}

static int nextchar()
{
   // no end-of-file check because 
   // this is done in the beginning in getline
   return buffer[buffer_ptr++];
}

static void buffer_done()
  {
     free(buffer);
     buffer=NULL;
     buffer_size=0;
     buffer_file=NULL;
  }

ssize_t fast_getline(char **lineptr)
  // warning !!!! -- semantics differ seriously from getline.
{
   int c;
   int l=0;
   char * result;
   // check end of line !
   if (buffer_ptr>=buffer_size)
     return -1;
   // now, we are sure we can read till the end of this line !
   result = buffer+buffer_ptr;
   while((c=nextchar())!='\n') l++;
   // give a pointer back
   *lineptr = result;
   // zero terminate the thing
   result[l]=0;
   return l;
}

char* strldup(char* str, int l)
{
   char * result = malloc(l+1);
   strncpy(result,str,l+1);
   return result;
}
	      
/*-------------------------------------------
 *         Index fields
 *-------------------------------------------*/
char   * index_readfrom = NULL;
char   * index_version;
 int     index_period;
char   * index_tempo;
char   * index_file;
char   * index_remark;
char   * index_tags;
char   * index_md5sum;
char   * index_time;
 int     index_changed;
 int     index_bpmcount_from;
 int     index_bpmcount_to;
unsigned long index_cue;
unsigned long index_cue_z;
unsigned long index_cue_x;
unsigned long index_cue_c;
unsigned long index_cue_v;

/*-------------------------------------------
 *         Index operations
 *-------------------------------------------*/
void index_init()
{
   index_version=NULL;
   index_period=-1;
   index_file=0;
   index_tags=NULL;
   index_md5sum=NULL;
   index_tempo=0;
   index_remark=0;
   index_changed=0;
   index_bpmcount_from=-1;
   index_bpmcount_to=-1;
   index_time=NULL;
   index_cue_z=0;
   index_cue_x=0;
   index_cue_c=0;
   index_cue_v=0;
   index_cue=0;
}

void index_setversion()
{
   char temp[500];
   sprintf(temp,"BpmDj v%s",VERSION);
   index_version=strdup(temp);
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
   if (index_md5sum) free(index_md5sum);
   if (index_time) free(index_time);
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
   if (index_time) fprintf(f,"time : %s\n",index_time);
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

char* strip(char*tostrip, char* theend)
{
   tostrip=strip_begin(tostrip);
   return strip_end(tostrip,theend);
}

void index_read(const char* indexn)
{
   char *line=NULL;
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
   // inititalize buffer
   buffer_init(index);
   // read fields
   while((read=fast_getline(&line))!=-1)
     {
	char * field;
	char * end = line + read;
	char * c = strip(line, end - 1), *value;
	while(*c!=':' && *c!=0)
	  {
	     *c=tolower(*c);
	     c++;
	  }
	if (!*c) continue;
	*c=0;
	field=strip_end(line,c-1);
	value=strip_begin(c+1);
	if (strcmp(field,"version")==0) index_version=strldup(value,end-value);
	else if(strcmp(field,"period")==0) index_period=atoi(value);
	else if(strcmp(field,"file")==0) index_file=strldup(value,end-value);
	else if(strcmp(field,"tempo")==0) index_tempo=strldup(value,end-value);
	else if(strcmp(field,"bpmcount-to")==0) index_bpmcount_to=atoi(value);
	else if(strcmp(field,"bpmcount-from")==0) index_bpmcount_from=atoi(value);
	else if(strcmp(field,"comment")==0) index_remark=strldup(value,end-value);
	else if(strcmp(field,"time")==0) index_time=strldup(value,end-value);
	else if(strcmp(field,"cue")==0) index_cue=atol(value);
	else if(strcmp(field,"cue-z")==0) index_cue_z=atol(value);
	else if(strcmp(field,"cue-x")==0) index_cue_x=atol(value);
	else if(strcmp(field,"cue-c")==0) index_cue_c=atol(value);
	else if(strcmp(field,"cue-v")==0) index_cue_v=atol(value);
	else if(strcmp(field,"md5sum")==0) index_md5sum=strldup(value,end-value);
	else if(strcmp(field,"tag")==0) 
	  {
	     if (!index_tags) index_tags=strldup(value,end-value);
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
	     continue;
	  }
     }
   // finish file access
   buffer_done();
   fclose(index);
   // check for old non-versioned files
   if (!index_version)
     {
	char * y;
	int idx;
	printf("Error: too old index file %s\n",basename(index_readfrom));
	exit(40);
     }
   if (index_period==-1)
     {
       printf("Error: no valid period given\n");
       exit(40);
     }
   // check for old versions running 22050 samplerate
   if ((strcmp(index_version,"BpmDj v1.5")==0)
       ||(strcmp(index_version,"BpmDj v1.4")==0)
       ||(strcmp(index_version,"BpmDj v1.3")==0)
       ||(strcmp(index_version,"BpmDj v1.2")==0)
       ||(strcmp(index_version,"BpmDj v1.1")==0)
       ||(strcmp(index_version,"BpmDj v1.0")==0)
       ||(strcmp(index_version,"BpmDj v0.9")==0)
       ||(strcmp(index_version,"BpmDj v0.8")==0)
       ||(strcmp(index_version,"BpmDj v0.7")==0)
       ||(strcmp(index_version,"BpmDj v0.6")==0)
       ||(strcmp(index_version,"BpmDj v0.5")==0))
     {
       index_cue_z *= 2;
       index_cue_x *= 2;
       index_cue_c *= 2;
       index_cue_v *= 2;
       index_cue   *= 2; 
       index_changed = 1;
       index_setversion();
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
