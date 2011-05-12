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
int      index_changed;
int      index_bpmcount_from;
int      index_bpmcount_to;
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
   index_period=0;
   index_file=0;
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
   if (index_cue>0) fprintf(f,"cue      : %ld\n",index_cue);
   if (index_cue_z>0) fprintf(f,"cue-z    : %ld\n",index_cue_z);
   if (index_cue_x>0) fprintf(f,"cue-x    : %ld\n",index_cue_x);
   if (index_cue_c>0) fprintf(f,"cue-c    : %ld\n",index_cue_c);
   if (index_cue_v>0) fprintf(f,"cue-v    : %ld\n",index_cue_v);
   if (index_bpmcount_from>=0) fprintf(f,"bpmcount-from : %d\n",index_bpmcount_from);
   if (index_bpmcount_to>=0) fprintf(f,"bpmcount-to   : %d\n",index_bpmcount_to);
   if (index_remark) fprintf(f,"remark   : %s\n",index_remark);
   for(i=0;i<index_nextcomment;i++) fprintf(f,"%s\n",index_comments[i]);
   fclose(f);
}

char* strip(char*tostrip)
{
   char *theend;
   // strip beginning
   while(*tostrip && (*tostrip==' ' || *tostrip=='\t' || *tostrip=='\n')) tostrip++;
   // find ending
   theend=tostrip;
   while(*theend) theend++;
   theend--;
   // strip end
   while(*theend && (*theend==' ' || *theend=='\t' || *theend=='\n')) theend--;
   *(theend+1)=0;
   // return stripped
   return tostrip;
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
   index_readfrom=indexn;
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
	     continue;  
	  }
	while(*c!=':' && *c!=0) c++;
	if (!*c) 
	  {
	     // ignore non-field lines
	     index_addcomment(line);
	     continue;
	  }
	*c=0;
	field=strip(line);
	value=strip(c+1);
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
	else 
	  {
	     printf("Warning: Unknown field %s\n",field);
	     index_addcomment(line);
	     continue;
	  }
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
   if (!index_period)
     {
	printf("Error: no valid period given\n");
	exit(40);
     }
   if (!index_tempo)
     {
	char tempo[500];
	sprintf(tempo,"%g",4.0*(double)11025*60.0/(double)index_period);
	index_tempo=strdup(tempo);
	index_changed=1;
     }
}
