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

#include <qapplication.h>
#include <qlineedit.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qlcdnumber.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qprogressbar.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <qspinbox.h>
#include <unistd.h>
#include <libgen.h>
#include <linux/soundcard.h>
#include <time.h>
#include <sys/times.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include "kbpm-counter.h"
#include "sys/times.h"
#include "about.h"

extern "C"
{
#include "cbpm-index.h"
#include "version.h"
#include "common.h"
#include "player-core.h"
}

/*
static   int  arg_from=0;
static   int  arg_to=100;
static   int  opt_write=0;
static   int  opt_start=0;
static   int  opt_stop=0;
static char * arg_write;
*/

BpmCountDialog::BpmCountDialog(SongPlayer*parent=0,const char*name=0, bool modal=FALSE, WFlags f =0) :
CountDialog(0,name,modal,f)
{
   player=parent;
   // owkee.. inbitialisation of everything.
   tapcount=0;
   audio=NULL;
   audiorate = 11025;
   startbpm = 120;
   stopbpm = 160;
   startshift = 0;
   stopshift = 0;
   index_bpmcount_from = 0;
   index_bpmcount_to = 100;
   bufsiz = 32 * 1024;
   // internal communicative variables
   reading_progress = 0;
   processing_progress = 0;
   stop_signal=false;
   working=false;
   // read the index file
   char d[1024];
   sprintf(d,"BpmDj v%s",VERSION);
   index_version=strdup(d);
   // clear the image
   QPixmap *pm = new QPixmap(10,10);
   QPainter p;
   p.begin(pm);
   QRect r(QRect(0,0,pm->width(),pm->height()));
   p.fillRect(r,Qt::white);
   p.end();
   BpmPix->setPixmap(*pm);
   // set index filen filename
   IdxEdit->setText(index_readfrom);
   // set tag line
   if (index_tags)
     TagEdit->setText(index_tags);
   else
     TagEdit->setText("");   
   // init md5 sum
   if (index_md5sum)
     Md5Label->setText(index_md5sum);
   // set song 
   if (index_file)
     Mp3Label->setText(index_file);
}


unsigned long BpmCountDialog::phasefit(long i)
{
   long c,d;
   unsigned long mismatch=0;
   unsigned long prev=mismatch;
   assert(i<audiosize);
   for(c=i;c<audiosize;c++)
     {
	d=abs((long)audio[c]-(long)audio[c-i]);
	prev=mismatch;
	mismatch+=d;
	assert(mismatch>=prev);
     }
   return mismatch;
}

long fsize(FILE * f)
{
   long answer;
   fseek(f,0,SEEK_END);
   answer=ftell(f);
   fseek(f,0,SEEK_SET);
   return answer;
}

/*void copyright()
{
   printf("BpmDj BPM Counter v%s, Copyright (c) 2001 Werner Van Belle\n",VERSION);
   printf("This software is distributed under the GPL2 license. See copyright.txt for details\n");
   printf("See beatmixing.ps for details how to use the program.\n");
   printf("--------------------------------------------------------------------\n");
}

void options_failure(char* err)
{
   // print options
   printf("Usage:  kbpm-count <options> argument\n\n"
	  "   -t nbr   --to nbr         read to % (default = 100)\n"
	  "   -f nbr   --from nbr       read from % (default = 0)\n"
	  "   -w arg   --write arg      write .idx file to ...\n"
	  "   argument                  the .mp3 file to find tempo for\n\n%s\n\n",err);
   exit(1);
}

/*void process_options(int argc, char* argv[])
{
   // run trough all the arguments
   int i = 1 ;
   for(i = 1 ; i < argc ; i ++)
     {
	if (argv[i][0]=='-')
	  {
	     char* arg;
	     // check long opt ?
	     if (argv[i][1]=='-')
	       arg=argv[i]+2;
	     else if (argv[i][1]==0 || argv[i][2]!=0)
	       options_failure("option neither short or long");
	     else arg=argv[i]+1;
	     // check value
	     if (strcmp(arg,"create")==0 ||
		 strcmp(arg,"c")==0)
	       opt_create=1;
	     else if (strcmp(arg,"from")==0 ||
		      strcmp(arg,"f")==0)
	       {
		  if (++i>=argc) 
		    options_failure("from argument scanning error");
		  arg_from=atoi(argv[i]);
	       }
	     else if (strcmp(arg,"to")==0 ||
		      strcmp(arg,"t")==0)
	       {
		  if (++i>=argc) 
		    options_failure("to argument scanning error");
		  arg_to=atoi(argv[i]);
	       }
	     else if (strcmp(arg,"low")==0 ||
		      strcmp(arg,"l")==0)
	       {
		  if (++i>=argc) 
		    options_failure("low argument scanning error");
		  arg_low=atoi(argv[i]);
	       }
	     else if (strcmp(arg,"high")==0 ||
		      strcmp(arg,"h")==0)
	       {
		  if (++i>=argc)
		    options_failure("high argument scanning error");
		  arg_high=atoi(argv[i]);
	       }
	     else if (strcmp(arg,"write")==0 ||
		      strcmp(arg,"w")==0)
	       {
		  opt_write=1;
		  if (++i>=argc)
		    options_failure("write argument scanning error");
		  arg_write=argv[i];
	       }
	     else if (strcmp(arg,"s")==0 ||
		      strcmp(arg,"start")==0)
	       opt_start=1;
	     else if (strcmp(arg,"S")==0 ||
		      strcmp(arg,"stop")==0)
	       opt_stop=1;
	  }
	else
	  argument = argv[i];
     }
   if (argument==NULL)
     {
	options_failure("requires at least one argument");
     }
   
}
*/

void BpmCountDialog::setBpmBounds(long start, long stop)
{
   char tmp[500];
   char tmp2[500];
   startbpm=start;
   stopbpm=stop;
   sprintf(tmp,"%d",startbpm);
   FromBpmEdit->setText(tmp);
   sprintf(tmp2,"%d",stopbpm);
   ToBpmEdit->setText(tmp2);
}

void BpmCountDialog::setPercentBounds(long startper, long stopper)
{
   char tmp[500];
   char tmp2[500];
   // de bpmcount_from en bpm_count_to fields moeten 
   // ingevuld worden
   index_bpmcount_from=startper;
   index_bpmcount_to=stopper;
   sprintf(tmp,"%d",index_bpmcount_from);
   FromPercentEdit->setText(tmp);
   sprintf(tmp2,"%d",index_bpmcount_to);
   ToPercentEdit->setText(tmp2);
}

void BpmCountDialog::getMd5()
{
   char d[500];
   // first we look up the md5sum !
   sprintf(d,"Obtaining md5sum for %s",argument);
   StatusLabel->setText(d);
   sprintf(d,"md5sum \"%s\" | awk '{printf $1}' >sum.tmp\n",argument);
   if (system(d)>256)
     {
	printf("Error: md5sum failed");
	exit(101);
     }
   else
     {
	FILE * kloink=fopen("sum.tmp","r");
	char s[40];
	int i=0;
	while(i<32)
	  {
	     int c = getc(kloink);
	     s[i]=c;
	     i++;
	  }
	s[32]=0;
	index_md5sum=strdup(s);
	fclose(kloink);
     }
   sprintf(d,"%s",index_md5sum);
   StatusLabel->setText("");
   Md5Label->setText(d);
}

void BpmCountDialog::timerTick()
{
   ReadingBar->setProgress(reading_progress);
   CountingBar->setProgress(processing_progress);
}

void BpmCountDialog::readAudio()
{
   FILE * raw;
   char d[500];
   // the filename of the file to read is the basename 
   // suffixed with .raw
   sprintf(d,"%s.raw",basename(index_file));
   raw=fopen(d,"rb");
   if (!raw)
     {
 	printf("Error: Unable to open %s\n",argument);
	exit(3);
     }
   // read complete file shrunken down into memory
   audiosize=fsize(raw);
   long startpercent=(long)((long long)audiosize*(long long)index_bpmcount_from/(long long)100);
   startpercent-=startpercent%4;
   long stoppercent=(long)((long long)audiosize*(long long)index_bpmcount_to/(long long)100);
   stoppercent-=stoppercent%4;
   audiosize=stoppercent-startpercent;
   audiosize/=(4*(WAVRATE/audiorate));
   audio=(unsigned char*)malloc(audiosize+1);
   if (!audio)
     {
	printf("Error: unable to allocate audio buffer\n");
	exit(4);
     }
   
   // reading in memory
   StatusLabel->setText("Reading");
   long pos=0, count, redux, i;
   signed short buffer[bufsiz];
   fseek(raw,startpercent,SEEK_SET);
   while(pos<audiosize && !stop_signal)
     {
	count=fread(buffer,sizeof(signed short),bufsiz,raw);
	reading_progress = pos*100/audiosize;
	for (i=0;i<count;i+=2*(WAVRATE/audiorate))
	  {
	     signed long int left, right,mean;
	     left=abs(buffer[i]);
	     right=abs(buffer[i+1]);
	     mean=(left+right)/2;
	     redux=abs(mean)/128;
	     if (pos+i/(2*(WAVRATE/audiorate))>=audiosize) break;
	     assert(pos+i/(2*(WAVRATE/audiorate))<audiosize);
	     audio[pos+i/(2*(WAVRATE/audiorate))]=(unsigned char)redux;
	  }
	pos+=count/(2*(WAVRATE/audiorate));
     }
   fclose(raw);
   if (stop_signal)
     {
	free(audio);
	audio=NULL;
	audiosize=0;
	StatusLabel->setText("Canceled while reading");
	return;
     }
   reading_progress = 100;
}
   
void BpmCountDialog::rangeCheck()
  {
     unsigned long int val;
     bool changed = false;
     val = atoi(FromPercentEdit->text());
     if (changed |= index_bpmcount_from != val) 
       index_bpmcount_from=val;
     val = atoi(ToPercentEdit->text());
     if (changed |= index_bpmcount_to != val) 
       index_bpmcount_to=val;
     val = atoi(FromBpmEdit->text());
     if (startbpm != val)
       startbpm=val;
     val = atoi(ToBpmEdit->text());
     if (stopbpm != val)
       stopbpm=val;
     
     // in this case we need to remove the current audio buffer
     if (changed)
       {
	  if (audio)
	    {
	       free(audio);
	       audio=NULL;
	       audiosize=0;
	    }
       }
  }

void BpmCountDialog::doit()
{
   char statustext[1024];
   char * tmp,d[500];
   int res2;
   long count,pos,i,redux;
   
   getMd5();
   
   // check wether ranges are changed
   rangeCheck();
   
   if (stop_signal)
     return;
   if (!audio)
     readAudio();
   if (stop_signal)
     return;
   
   // now process the sucker 
   StatusLabel->setText("Finding best autocorrelation");
   sprintf(d," ");
   stopshift=audiorate*60*4/startbpm;
   startshift=audiorate*60*4/stopbpm;
     {
	unsigned long foutat[stopshift-startshift];
	unsigned long fout, minimumfout=0, maximumfout,minimumfoutat,left,right;
	memset(&foutat,0,sizeof(foutat));
	
	// how many times wil we execute phasefit ???
	// stopshift-startshift/50 + 200
	int O = ((stopshift-startshift) / 50)+200;
	int o = 0;
	for(i=startshift;i<stopshift && !stop_signal;i+=50)
	  {
	     fout=phasefit(i);
	     o++;
	     processing_progress = o*100/O;
	     foutat[i-startshift]=fout;
	     //	     printf(d,"# %d: %ld (%g BPM)\n",i,fout,
	     //		    4.0*(double)audiorate*60.0/(double)i);
	     if (minimumfout==0) maximumfout=minimumfout=fout;
	     if (fout<minimumfout) 
	       {
		  minimumfout=fout;
		  minimumfoutat=i;
	       }
	     if (fout>maximumfout) maximumfout=fout;
	  }
	if (stop_signal)
	  {
	     StatusLabel->setText("Canceled while autocorrelating");
	     return;
	  }
	if (minimumfoutat>=100) left=minimumfoutat-100;
	else left=0;
	if (left<startshift) left=startshift;
	right=left+200;
	if (right>stopshift) right=stopshift;
	for(i=left;i<right && !stop_signal;i++)
	  {
	     fout=phasefit(i);
	     o++;
	     processing_progress = o*100/O;
	     foutat[i-startshift]=fout;
	     //	     printf("# %d: %ld (%g BPM)\n",i,fout,
	     //		    4.0*(double)audiorate*60.0/(double)i);
	     if (minimumfout==0) maximumfout=minimumfout=fout;
	     if (fout<minimumfout) 
	       {
		  minimumfout=fout;
		  minimumfoutat=i;
	       }
	     if (fout>maximumfout) maximumfout=fout;
	  }
	if (stop_signal)
	  {
	     StatusLabel->setText("Canceled while autocorrelating");
	     return;
	  }
	processing_progress = 100;
	StatusLabel->setText("Ready");
	index_period=minimumfoutat;
	index_changed=1;
	double tempo = 4.0*(double)audiorate*60.0/(double)minimumfoutat;
	sprintf(d,"%g",tempo);
	index_tempo=strdup(d);

	normalperiod=index_period*4;
	currentperiod=normalperiod;
	if (WAVRATE==22050)
	  {
	     currentperiod/=2;
	     normalperiod/=2;
	  }

	// if possible we should draw this !
	QPixmap *pm = new QPixmap(640,640);
	QPainter p;
	p.begin(pm);
	QRect r(QRect(0,0,pm->width(),pm->height()));
	p.fillRect(r,Qt::white);
	// now we should draw the vertical lines at 1/4; 2/4; 3/4
	int X,Y;
	p.setPen(Qt::green);
	p.drawLine(320,0,320,639);
	p.drawLine(160,0,160,639);
	p.drawLine(480,0,480,639);
	p.setPen(Qt::red);
	X = (minimumfoutat - startshift) * r.width() / (stopshift - startshift);
	X = r.width() - X;
	p.drawLine(X,0,X,639);
	
	// draw points
	p.setPen(Qt::black);
	for(i=startshift;i<stopshift;i++)
	  {
	     fout=foutat[i-startshift];
	     if (fout)
	       {
		  fout-=minimumfout;
		  X = (i - startshift) * r.width() / (stopshift - startshift);
		  X = r.width() - X;
		  Y = (int)((long long)fout * (long long)r.height() / (long long) (maximumfout-minimumfout));
		  Y = r.height() - Y;
		  p.drawPoint(X,Y);
		  p.drawPoint(X+1,Y);
		  p.drawPoint(X,Y+1);
		  p.drawPoint(X+1,Y+1);
	       }
	  }
	// set labels...
	sprintf(d,"%2g",4.0*(double)audiorate*60.0/stopshift);
	X0->setText(d);
	sprintf(d,"%2g",4.0*(double)audiorate*60.0/(startshift+3*(double)(stopshift-startshift)/4.0));
	X1->setText(d);
	sprintf(d,"%2g",4.0*(double)audiorate*60.0/(startshift+(double)(stopshift-startshift)/2.0));
	X2->setText(d);
	sprintf(d,"%2g",4.0*(double)audiorate*60.0/(startshift+(double)(stopshift-startshift)/4.0));
	X3->setText(d);
	sprintf(d,"%2g",4.0*(double)audiorate*60.0/startshift);
	X4->setText(d);

	p.end();
	BpmPix->setPixmap(*pm);
     }
}

void BpmCountDialog::doitwrapper()
{
   doit();
   working=false;
   StartButton->setText("Start");
   BusyToggle->toggle();
}

void* doit(void* dialog)
{
   ((BpmCountDialog*)dialog)->doitwrapper();
}

void BpmCountDialog::startAutomaticCounter()
{
   if (working)
     {
	stopWorking();
     }
   else
     {
	StartButton->setText("Stop");
	BusyToggle->toggle();
	working=true;
	stop_signal=false;
	pthread_t y;
	pthread_create(&y,NULL,::doit,(void*)this);
     }
}

void BpmCountDialog::removeRaw()
  {
     char d[500];
     // remove the old file
     sprintf(d,"%s.raw",basename(index_file));
     remove(d);
  }

void BpmCountDialog::stopWorking()
  {
     if (working)
       {
	  StatusLabel->setText("Canceling...");
	  stop_signal=true;
	  while(working) ;
       }
  }

void BpmCountDialog::finish()
{
   stopWorking();
   if (strcmp(index_readfrom,IdxEdit->text())!=0)
     {
	index_changed=1;
	index_readfrom=strdup(IdxEdit->text());
     }
   if (strcmp(index_tags,TagEdit->text())!=0)
     {
	index_tags=strdup(TagEdit->text());
	index_changed=1;
     }
   accept();
}

/*int main(int argc, char *argv[])
{
   // input all information into the dialog
 * // het zetten van deze bounds gaat nog plezant worden. Nadat de applicatie gestart is is daar niet veel meer
 * aan te doen in essentie. Buiten de values vanuit de index file aan te passen
 * hetzelfde voor de bpm bounds
 
   dialog->setPercentBounds(arg_from,arg_to);
   dialog->setBpmBounds(arg_low,arg_high);
   // de index_file is de naam van de file met het volledige pad binnen 
   // de music directory. Dit wil zeggen dat we de music er af moeten strippen
   index_file=strdup(argument);
   if (strstr(index_file,"music/")==index_file) index_file+=strlen("music/");
   else if (strstr(index_file,"./music/")==index_file) index_file+=strlen("./music/");
   else printf("Warning: song not in music/ or ./music/ directory\n");
   dialog->setMp3File(index_file);
   // de file die we wegschrijven is de songname met .idx ipv .mp3
   // deze data wordt in de huidige direcotry geschreven, tenzij anders 
   // opgegeven natuurlijk
   char * tmp;
   if (opt_write)
     index_readfrom=strdup(arg_write);
   else
     {
	index_readfrom=strdup(basename(index_file));
	tmp=strstr(index_readfrom,".mp3");
	if (!tmp) tmp=strstr(index_readfrom,".MP3");
	strcpy(tmp,".idx");
     }
   // this one will also read the given data
   dialog->setIndexFile(index_readfrom);
   // now insert widget and execute the thing
   app->setMainWidget(dialog);
   dialog->show();
}
*/

void BpmCountDialog::reset()
{
   tapcount = 0;
   TapLcd->display(tapcount);
}

void BpmCountDialog::tap()
{
   char d[500];
   // increase tapcount
   tapcount++;
   if (tapcount==1)
     {
	starttime=times(NULL);
     }
   else
     {
	// we have to decrease tapcount with one because at tick 2 the 
	// time passed counts for only one beat.
	index_period=(((signed8)times(NULL)-(signed8)starttime)/(tapcount-1));
	index_period/=SkipBox->value();
	index_period*=11025*4;
	index_period/=CLOCK_FREQ;
	normalperiod=index_period*4;
	currentperiod=normalperiod;
	if (WAVRATE==22050)
	  {
	     currentperiod/=2;
	     normalperiod/=2;
	  }
	double tempo = 4.0*(double)11025*60.0/(double)index_period;
	sprintf(d,"%g",tempo);
	if (index_tempo)
	  free(index_tempo);
	index_tempo=strdup(d);
	// printf("tempo = %s\n",index_tempo);
	index_changed=1;
	// update bounds
	setBpmBounds((long)(tempo-10.0),(long)(tempo+10.0));
     }
   TapLcd->display(tapcount);
}


void BpmCountDialog::incBpm()
{
   index_period-=10;
   if(index_period<100) 
     index_period=100;
   normalperiod=index_period*4;
   currentperiod=normalperiod;
   if (WAVRATE==22050)
     {
	currentperiod/=2;
	normalperiod/=2;
     }
}

void BpmCountDialog::decBpm()
{
   index_period+=10;
   normalperiod=index_period*4;
   currentperiod=normalperiod;
   if (WAVRATE==22050)
     {
	currentperiod/=2;
	normalperiod/=2;
     }
}

void BpmCountDialog::about()
{
   char tmp[500];
   AboutDialog about(NULL,NULL,1);
   sprintf(tmp,"BpmDj v%s",VERSION);
   about.versionLabel->setText(tmp);
   about.exec();
}
