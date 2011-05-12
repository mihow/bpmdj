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

#include <unistd.h>
#include <dirent.h>
#include <qmessagebox.h>
#include <qlcdnumber.h>
#include <ctype.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mount.h>
#include <libgen.h>
#include "songselector.logic.h"
#include "preferences.logic.h"
#include "about.h"
#include "songedit.h"
#include "qsongviewitem.h"
#include "kbpm-played.h"
#include "askinput.h"
#include "version.h"
extern "C"
{
#include "cbpm-index.h"
}

double SongSelectorLogic::mainTempo=0;
bool SongSelectorLogic::color_played=false;
bool SongSelectorLogic::color_range=false;
bool SongSelectorLogic::color_notondisk=false;
int  SongSelectorLogic::itemcount=0;
SongSelectorLogic *mainWindow;
  
void ToSwitchOrNotToSwitchSignal(int sig,siginfo_t *info, void* hu)
{
   if (info->si_pid!=mainWindow->monitorpid)
     mainWindow->switchMonitorToMain();
   else
     mainWindow->monitorpid=0;
}

SongSelectorLogic::SongSelectorLogic(QWidget*parent=0,const char*name=0, bool modal=FALSE,WFlags f=0) :
  SongSelector(parent,name,modal,f)
{
   mainTicks=0;
   timer=new QTimer(this);
   connect(timer,SIGNAL(timeout()), SLOT(timerTick()));
   timer->start(1000);
   playCommand1=PLAYCOMMAND1;
   playCommand2=PLAYCOMMAND2;
   monitorPlayCommand=1;
   playingInMain=NULL;
   playingInMonitor=NULL;
   yellowTime=TIME_YELLOW;
   orangeTime=TIME_ORANGE;
   redTime=TIME_RED;
   filterBpm=0;
   mainTempo=0;
   nextTagLine=0;
   itemcount=0;
   targetIndex->setText("");
   mainWindow=this;
   // catch signals
   struct sigaction *act;
   act=(struct sigaction*)malloc(sizeof(struct sigaction));
   assert(act);
   act->sa_sigaction=ToSwitchOrNotToSwitchSignal;
   act->sa_flags=SA_SIGINFO;
   sigaction(SIGUSR1,act,NULL);
   monitorpid=0;
   // ignore sigchlds
   signal(SIGCHLD,SIG_IGN);
};

void SongSelectorLogic::injectDataSet(SongIndex *data)
{
   dataRoot=data;
   // add all tags
   addTag("");
   findTags(data);
   // update the item list
   updateItemList();
}

void SongSelectorLogic::findTags(Song* song)
{
   if (!song) return;
   if (song->isIndex())
     findTags(((SongIndex*)song)->list);
   findTags(song->next);
   // parsing of the tags
   char* tags=song->song_tags;
   char* runner=tags;
   if (!tags) return;
   while(*runner)
     {
	while(*runner && *runner!=' ') runner++;
	if (*runner) 
	  {
	     *runner=0;
	     addTag(tags);
	     *runner=' ';
	     tags=++runner;
	  }
	else
	  {
	     addTag(tags);
	  }
     }
}

void SongSelectorLogic::flattenFilter(Song* list, QListViewItem* parent)
{
   if (!list) return;
   if (doFilter(list))
     new QSongViewItem(parent,list);
   if (list->isIndex())
     flattenFilter(((SongIndex*)list)->list,parent);
   flattenFilter(list->next,parent);
}

void SongSelectorLogic::deepFilter(Song* list, QListViewItem* parent)
{
   if (!list) return;
   if (doFilter(list))
     {
	if (list->isIndex())
	  deepFilter(((SongIndex*)list)->list,new QSongViewItem(parent,list));
	else 
	  new QSongViewItem(parent,list);
     }
   deepFilter(list->next,parent);
}

void SongSelectorLogic::flattenFilter(Song* list, QListView* parent)
{
   if (!list) return;
   if (doFilter(list))
     new QSongViewItem(parent,list);
   if (list->isIndex())
     flattenFilter(((SongIndex*)list)->list,parent);
   flattenFilter(list->next,parent);
}

void SongSelectorLogic::deepFilter(Song* list, QListView* parent)
{
   if (!list) return;
   if (doFilter(list))
     {
	if (list->isIndex())
	  deepFilter(((SongIndex*)list)->list,new QSongViewItem(parent,list));
	else new QSongViewItem(parent,list);
     }
   deepFilter(list->next,parent);
}

bool SongSelectorLogic::doFilter(Song* song)
{
   if (filter(song))
     {
	itemcount++;
	return true;
     }
   else return false;
}

void SongSelectorLogic::updateItemList()
{
   itemcount=0;
   songList->clear();
   if (flatStructure->isChecked())
     flattenFilter(dataRoot,songList);
   else 
     deepFilter(dataRoot,songList);
   countLcd->display(itemcount);
}

char* strupper(const char* t)
{
   char tmp[500];
   int i=0;
   do 
     {
	tmp[i++]=toupper(*t);
     }
   while(*(t++));
   return strdup(tmp);
}

bool SongSelectorLogic::filter(Song *item)
{
   // structure
   if (item->isIndex())
     {
	if (flatStructure->isChecked()) return false;
	return true;
     }
   // song on disk ?
   if (onDiskFilter->isChecked() && !item->song_ondisk)
     return false;
   // song played ?
   if (strcmp(item->song_index,mainMix->text())==0)
     item->song_played=true;
   if (notYetPlayedFilter->isChecked() && item->song_played)
     return false;
   // check tag matching
   bool matched=false;
   for (int i=0; i<nextTagLine; i++)
     {
	if (tagInclude[i]->isChecked())
	  {
	     if (item->containsTag(tagLines[i]->text()))
	       matched=true;
	  }
     }
   if (!matched) return false;
   for (int i=0; i<nextTagLine; i++)
     {
	if (tagExclude[i]->isChecked())
	  {
	     if (item->containsTag(tagLines[i]->text()))
	       matched=false;
	  }
     }
   if (!matched) return false;
   // tempo 
   if (item->song_tempo && tempoFilter->isChecked())
     {
	double t=atof(item->song_tempo);
	if (t<mainTempo*0.94) return false;
	if (t>mainTempo*1.06) return false;
     }
   // search
   const char * lookingfor=searchLine->text();
   if (strcmp(lookingfor,"")!=0)
     {
	char *title=strupper(item->song_title);
	char *author=strupper(item->song_author);
	if (!(!title || !author || strstr(title,lookingfor) || strstr(author,lookingfor))) 
	  {
	     free(title);
	     free(author);
	     return false;
	  }
	free(title);
	free(author);
     }
   return true;
}

void SongSelectorLogic::timerTick()
{
   mainLCD->display(++mainTicks);
   if (mainTicks==1)
     setColor(QColor(0,255,0));
   else if (mainTicks==yellowTime)
     setColor(QColor(255,255,0));
   else if (mainTicks==orangeTime)
     setColor(QColor(255,127,0));
   else if (mainTicks==redTime)
     setColor(QColor(255,0,0));
}

void SongSelectorLogic::setColor(QColor color)
{
   countLcd->setBackgroundColor(color);
   mainLCD->setBackgroundColor(color);

/*   TextLabel1->setBackgroundColor(color);
   GroupBox2->setBackgroundColor(color);
   Line1->setBackgroundColor(color);
   TextLabel2_3->setBackgroundColor(color);
   flatStructure->setBackgroundColor(color);
   notYetPlayedFilter->setBackgroundColor(color);
   colorPlayed->setBackgroundColor(color);
   tempoFilter->setBackgroundColor(color);
   colorRange->setBackgroundColor(color);
   onDiskFilter->setBackgroundColor(color);
   colorNotOnDisk->setBackgroundColor(color);
   GroupBox3->setBackgroundColor(color);
   TextLabel2_2->setBackgroundColor(color);
   TextLabel1_3->setBackgroundColor(color);
   TextLabel3_2->setBackgroundColor(color);
   monitorTitle->setBackgroundColor(color);
   monitorAuthor->setBackgroundColor(color);
   monitorMix->setBackgroundColor(color);
   PushButton4->setBackgroundColor(color);
   PushButton2->setBackgroundColor(color);
   GroupBox1->setBackgroundColor(color);
   mainTitle->setBackgroundColor(color);
   mainAuthor->setBackgroundColor(color);
   mainMix->setBackgroundColor(color);
   TextLabel2->setBackgroundColor(color);
   TextLabel3->setBackgroundColor(color);
   TextLabel4->setBackgroundColor(color);
   TextLabel1_2->setBackgroundColor(color);
   TextLabel1_4->setBackgroundColor(color);
   mainTempoText->setBackgroundColor(color);
   targetIndex->setBackgroundColor(color);
   PushButton6->setBackgroundColor(color);
   PushButton1->setBackgroundColor(color);
   PushButton3->setBackgroundColor(color);
   PushButton5->setBackgroundColor(color);
*/
}

void SongSelectorLogic::switchMonitorToMain()
{
   char tmp[500];
   /* clear the Lcd */
   mainTicks=-1;
   timerTick();
   /* set the title, author & mix */
   mainTitle->setText(monitorTitle->text());
   mainAuthor->setText(monitorAuthor->text());
   mainMix->setText(monitorMix->text());
   if(strcmp(targetIndex->text(),"")==0)
     mainTempo=monitorTempo;
   sprintf(tmp,"%g",mainTempo);
   mainTempoText->setText(strdup(tmp));
   /* clear the monitor */
   monitorTitle->setText("");
   monitorAuthor->setText("");
   monitorMix->setText("");
   monitorTempo=0;
   monitorpid=0;
   /* ok, the song is already playing, now use the next command next time */
   monitorPlayCommand=(monitorPlayCommand == 1 ? 2 : 1);
   playingInMain=playingInMonitor;
   playingInMonitor=NULL;
   /* write playing sucker to disk */
   Played::Play(mainMix->text());
   /* update view */
   if (flatStructure->isChecked())
     updateItemList();
}

int songFileCompare(const void * a, const void* b)
{
   Song **pa=(Song**)a;
   Song **pb=(Song**)b;
   Song *A=*pa;
   Song *B=*pb;
   return strcmp(A->song_file,B->song_file);
}

void SongSelectorLogic::doLock()
{
   // if main equals target clear the sucker
   if (strcmp(targetIndex->text(),mainMix->text())==0)
     {
	targetIndex->setText("");
     }
   // otherwise, set target to current main
   else
     {
	targetIndex->setText(mainMix->text());
     }
}


bool SongSelectorLogic::lookfor(char* filename,Song* cur)
{
   if (!cur) return false;
   else if (cur->isIndex())
     {
	return lookfor(filename,((SongIndex*)cur)->list) 
	  || lookfor(filename,cur->next);
     }
   else if (cur->song_file)
     {
	if (strcmp(cur->song_file,filename)==0)
	  return true;
	return lookfor(filename,cur->next);
     }
}

void SongSelectorLogic::checkfile(char* filename)
{
   // file should end on .mp3
   // or .MP3
   if (strlen(filename)<4) return;
   if (stricmp(filename+strlen(filename)-4,".mp3")==0)
     {
	printf("Looking for %s\n",filename);
	if (!lookfor(filename,dataRoot))
	  {
	     char temp[500];
	     printf("==> NOT FOUND !!!\n",filename);
	     sprintf(temp,"%s not found",filename);
	     QMessageBox::information(this,"Not found",temp);
	  }
     }
}

void SongSelectorLogic::scandir(char* dirname, char*checkname)
{
   DIR *dir;
   struct dirent*entry;
   dir=opendir(dirname);
   if (!dir) return;
   printf("Recursing into %s\n",dirname);
   while (entry=readdir(dir))
     {
	if (strcmp(entry->d_name,".")==0 ||
	    strcmp(entry->d_name,"..")==0) continue;
	char txt[500];
	char txt2[500];
	sprintf(txt,"%s/%s",checkname,entry->d_name);
	sprintf(txt2,"%s/%s",dirname,entry->d_name);
	scandir(txt2,txt);
	checkfile(txt);
     }
   closedir(dir);
}

void SongSelectorLogic::checkDisc()
{
   // 0 - which CD is this ?
   AskWhichCd whichCd(NULL,NULL,TRUE);
   if (whichCd.exec()==whichCd.Rejected) return;
   // 1 - mount cdrom
   system("mount /cdrom");
   // 2 - Scan directory 
   scandir("/cdrom",strdup(whichCd.LineEdit->text()));
   // 3 - umount cdrom
   system("umount /cdrom");
   system("eject /cdrom");
}

void SongSelectorLogic::fetchSelection()
{
   /* count nr of files to get */
   int selectionCount=0;
   QListViewItemIterator it1(songList);
   for(;it1.current();++it1)
     {
	QSongViewItem *svi=(QSongViewItem*)it1.current();
	if (svi->isSelected()) selectionCount++;
     }
   /* allocate enough space to store the songs */
   Song * * toFetch;
   int next=0;
   toFetch=(Song**)calloc(selectionCount,sizeof(Song*));
   assert(toFetch);
   next=0;
   /* read all the songs */
   QListViewItemIterator it2(songList);
   for(;it2.current();++it2)
     {
	QSongViewItem *svi=(QSongViewItem*)it2.current();
	if (svi->isSelected())
	  toFetch[next++]=svi->getSong();
     }
   /* sort the list */
   qsort(toFetch,next,sizeof(Song*),songFileCompare);
   /* write script to disk */
   FILE* script=fopen("fetchfiles.sh","wb");
   assert(script);
   fprintf(script,"#!/bin/bash\n");
   fprintf(script,"#writing %d files\n",next);
   fprintf(script,"umount /cdrom\n");
   fprintf(script,"eject /cdrom\n");
   int i = 0;
   char* currentcd=0;
   while(i<next)
     {
	Song* song=toFetch[i++];
	char* filename=song->song_file;
	char* newcd=strdup(filename);
	char temp[500];
	char* breakat=index(newcd,'/');
	assert(breakat);
	*breakat=0;
	if (currentcd==0 || strcmp(currentcd,newcd)!=0)
	  {
	     if (currentcd)
	       {
		  free(currentcd);
		  fprintf(script,"umount /cdrom\n");
		  fprintf(script,"eject /cdrom\n");
	       }
	     currentcd=newcd;
	     fprintf(script,"dialog --msgbox \"Please insert %s\" 10 40\n",newcd);
	     fprintf(script,"mount /cdrom\n");
	  }
	else 
	  free(newcd);
	char* dirc=strdup(filename);
	char* filenc=strdup(filename);
	char* dir=dirname(dirc);
	char* filen=basename(filenc);
	sprintf(temp,"\"../cBpmDj/music/%s\"",dir);
	char* target=strdup(temp);
	sprintf(temp,"mkdir -p %s",target);
	fprintf(script,"%s\n",temp);
	sprintf(temp,"\"/cdrom%s\"",index(filename,'/'));
	char* source=strdup(temp);
	sprintf(temp,"cp %s %s\n",source,target);
	fprintf(script,temp);
	free(source);
	free(target);
	free(dirc);
	free(filenc);
     }
   fprintf(script,"umount /cdrom\n");
   fclose(script);
   
   /* umount any left over cdrom */
   system("umount /cdrom");
   system("eject /cdrom");
   /* fetch the files one by one */
   i = 0;
   currentcd = 0;
   while(i<next)
     {
	Song* song=toFetch[i++];
	char* filename=song->song_file;
	/* read the cd-entry */
	char* newcd=strdup(filename);
	char temp[500];
	char* breakat=index(newcd,'/');
	assert(breakat);
	*breakat=0;
	if (currentcd==0 || strcmp(currentcd,newcd)!=0)
	  {
	     /* umount if necessarry */
	     if (currentcd)
	       {
		  free(currentcd);
		  if (system("umount /cdrom")<0)
		    QMessageBox::warning(this,"CD","Cannot umount /cdrom");
		  system("eject /cdrom");
	       }
	     /* ask user to input new cd */
	     currentcd=newcd;
	     sprintf(temp,"Please insert %s",newcd);
	     QMessageBox::information(this,"CD",temp);
	     /* mount the cdrom */
	     if (system("mount /cdrom")==-1)
	       {
		  sprintf(temp,"Cannout mount /cdrom (errno=%d)",errno);
		  QMessageBox::warning(this,"CD",temp);
	       }
	  }
	else 
	  free(newcd);
	/* create the target directory */
	char* dirc=strdup(filename);
	char* filenc=strdup(filename);
	char* dir=dirname(dirc);
	char* filen=basename(filenc);
	sprintf(temp,"\"../cBpmDj/music/%s\"",dir);
	char* target=strdup(temp);
	sprintf(temp,"mkdir -p %s",target);
	system(temp);
	/* read the given file from the cdrom */
	sprintf(temp,"\"/cdrom%s\"",index(filename,'/'));
	char* source=strdup(temp);
	sprintf(temp,"cp %s %s\n",source,target);
	/* copy it */
	system(temp);
	/* mark it on disk */
	song->song_ondisk=true;
	/* free source & target */
	free(source);
	free(target);
	free(dirc);
	free(filenc);
     }
   /* finally umount the last cdrom */
   system("umount /cdrom");
}

void SongSelectorLogic::exportPlayList()
{
   /* count nr of files to get */
   int selectionCount=0;
   QListViewItemIterator it1(songList);
   for(;it1.current();++it1)
     {
	QSongViewItem *svi=(QSongViewItem*)it1.current();
	if (svi->isSelected()) selectionCount++;
     }
   /* allocate enough space to store the songs */
   Song * * toFetch;
   int next=0;
   toFetch=(Song**)calloc(selectionCount,sizeof(Song*));
   assert(toFetch);
   next=0;
   /* read all the songs */
   QListViewItemIterator it2(songList);
   for(;it2.current();++it2)
     {
	QSongViewItem *svi=(QSongViewItem*)it2.current();
	if (svi->isSelected())
	  toFetch[next++]=svi->getSong();
     }
   /* write script to disk */
   FILE* script=fopen("playlist.xmms","wb");
   assert(script);
   int i = 0;
   while(i<next)
     {
	Song* song=toFetch[i++];
	char* filename=song->song_file;
	fprintf(script,"../cBpmDj/music/%s\n",filename);
     }
   fclose(script);
}

void SongSelectorLogic::selectSong(QListViewItem *song)
{
   int result;
   char *player;
   char *matchWith;
   char playercommand[500];
   char tmp[500];
   /* if there is still a song playing in the monitor, don't do it */
   if (monitorpid!=0)
     {
	const QString a=QString("Error");
	const QString b=QString("Cannot start playing in monitor, other monitor song still playing !");
	QMessageBox::critical(NULL,a,b,QMessageBox::Ok,0,0);
	return;
     }
   /* if no song selected (clicked outside songable area) */
   if (!song) return;
   /* if hierarchy */
   if (!song->text(LIST_INDEX)) return;
   /* send it to the monitor */
   monitorTitle->setText(song->text(LIST_TITLE));
   monitorAuthor->setText(song->text(LIST_AUTHOR));
   monitorMix->setText(song->text(LIST_INDEX));
   monitorTempo=atof(song->text(LIST_TEMPO));
   /* create suitable command */
   playingInMonitor=strdup(song->text(LIST_INDEX));
   sprintf(tmp,"\"%s\"",playingInMonitor);
   playingInMonitor=strdup(tmp);
   /* if there is a target locked index, use it */
   if (strcmp(targetIndex->text(),"")==0)
     matchWith=playingInMain;
   else
     matchWith=strdup(targetIndex->text());
   if (!matchWith) matchWith=playingInMonitor;
   player=monitorPlayCommand == 1 ? playCommand1 : playCommand2;
   sprintf(playercommand, player, matchWith, playingInMonitor);
   /* fork and execute the command */
   if (!(monitorpid=fork()))
     {
	system(playercommand);
	kill(getppid(),SIGUSR1);
	exit(0);
     }
}

void SongSelectorLogic::doPreferences()
{
   char tmp[50];
   PreferencesDialogLogic preferences(NULL,NULL,TRUE);
   preferences.playerCommand1->setText(playCommand1);
   preferences.playerCommand2->setText(playCommand2);
   sprintf(tmp,"%d",yellowTime);
   preferences.yellowTime->setText(tmp);
   sprintf(tmp,"%d",orangeTime);
   preferences.orangeTime->setText(tmp);
   sprintf(tmp,"%d",redTime);
   preferences.redTime->setText(tmp);
   if (preferences.exec()==Accepted)
     {
	playCommand1=strdup(preferences.playerCommand1->text());
	playCommand2=strdup(preferences.playerCommand2->text());
	yellowTime=atoi(preferences.yellowTime->text());
	orangeTime=atoi(preferences.orangeTime->text());
	redTime=atoi(preferences.redTime->text());
     }
}

void SongSelectorLogic::doSongEdit(QListViewItem *song)
{
   if (!song) return;
   char* index=strdup(song->text(LIST_INDEX));
   SongData songdata(NULL,NULL,TRUE);
   songdata.index->setText(index);
   songdata.title->setText(song->text(LIST_TITLE));
   songdata.author->setText(song->text(LIST_AUTHOR));
   songdata.newTempo->setText(song->text(LIST_TEMPO));
   songdata.newTags->setText(song->text(LIST_TAGS));
   if (songdata.exec()==Accepted)
     {
	double tempo=atof(songdata.newTempo->text());
	// read the index file
	index_read(index);
	// modify tempo & period
	index_period = (int) ((double)11025 * (double)60 * (double) 4 / tempo);
	index_tempo = strdup(songdata.newTempo->text());
	// modify taglines
	index_tags = strdup(songdata.newTags->text());
	// write the indfex file
	index_write();
	// free the bastard
	index_free();
	// now update the local songdata
	Song *s=((QSongViewItem*)song)->getSong();
	s->song_tempo=strdup(songdata.newTempo->text());
	s->song_tags=strdup(songdata.newTags->text());
     }
   free(index);
}

void SongSelectorLogic::songAddTag(QListViewItem *song, const char* tag)
{
   char newtags[500];
   char* index=strdup(song->text(LIST_INDEX));
   // read the index file
   index_read(index);
   // modify taglines
   sprintf(newtags,"%s %s",(const char*)(song->text(LIST_TAGS)),tag);
   index_tags = strdup(newtags);
   // write the indfex file
   index_write();
   // free the bastard
   index_free();
   // now update the local songdata
   Song *s=((QSongViewItem*)song)->getSong();
   s->song_tags=strdup(newtags);
   free(index);
}

void SongSelectorLogic::selectionAddTags()
{
   // ask user the tag to add
   SongData songdata(NULL,NULL,TRUE);
   songdata.index->setText("*");
   songdata.title->setText("*");
   songdata.author->setText("*");
   songdata.newTempo->setText("---");
   songdata.newTags->setText("");
   if (songdata.exec()==Accepted)
     {
	QListViewItemIterator it1(songList);
	for(;it1.current();++it1)
	  {
	     QSongViewItem *svi=(QSongViewItem*)it1.current();
	     if (svi->isSelected()) 
	       songAddTag(svi,songdata.newTags->text());
	  }
     }
}

void SongSelectorLogic::doMarkDups()
{
   int nr=0;
   QListViewItemIterator it1(songList);
   for(;it1.current();++it1)
     {
	QSongViewItem *svi=(QSongViewItem*)it1.current();
	if (svi->isSelected())
	  {
	     QListViewItemIterator it2(songList);
	     char* txt=strdup(svi->text(LIST_MD5SUM));
	     printf("%d -- ",nr++);
	     bool found=false;
	     for(;it2.current();++it2)
	       {
		  QSongViewItem *svi2=(QSongViewItem*)it2.current();
		  if (svi!=svi2 
		      && strcmp(txt,
				svi2->text(LIST_MD5SUM))==0)
		    {
		       found=true;
		       svi2->setSelected(true);
		       break;
		    }
	       }
	     free(txt);
	     if (!found)
	       svi->setSelected(false);
	  }
     }
}

void SongSelectorLogic::doAbout()
{
   char tmp[500];
   AboutDialog about(NULL,NULL,1);
   sprintf(tmp,"BpmDj v%s",VERSION);
   about.versionLabel->setText(tmp);
   about.exec();
}

void SongSelectorLogic::doFilterChanged()
{
   const char * txt1;
   char * txt2;
   color_range=colorRange->isChecked();
   color_notondisk=colorNotOnDisk->isChecked();
   color_played=colorPlayed->isChecked();
   txt1=searchLine->text();
   txt2=strupper(txt1);
   searchLine->setText(txt2);
   free(txt2);
   updateItemList();
}

void SongSelectorLogic::addTag(const char* tag)
{
   int baseline=260;
   int col=0, row=0;
   int xbox=120;
   int i=0;
   /* does the tag already exist */
   for(i=0;i<nextTagLine;i++)
     if (strcmp(tagLines[i]->text(),tag)==0) return;
   /* no, add it */
   if (nextTagLine>=MAXTAGS) return;
   assert(nextTagLine<MAXTAGS);
   col=nextTagLine/MAXTAGSPERCOL;
   row=nextTagLine-col*MAXTAGSPERCOL;
   tagLines[nextTagLine]   =  new QLabel( GroupBox2, "" );
   tagLines[nextTagLine]   -> setGeometry( QRect( 50+col*xbox, baseline+30*row, 110, 30 ) );
   tagLines[nextTagLine]   -> setText( tag );
   tagInclude[nextTagLine] =  new QCheckBox( GroupBox2, "" );
   tagInclude[nextTagLine] -> setGeometry( QRect( 10+col*xbox, baseline+30*row, 20, 30 ) );
   tagInclude[nextTagLine] -> setText( tr( "" ) );
   tagExclude[nextTagLine] =  new QCheckBox( GroupBox2, "" );
   tagExclude[nextTagLine] -> setGeometry( QRect( 30+col*xbox, baseline+30*row, 20, 30 ) );
   tagExclude[nextTagLine] -> setText( tr( "" ) );
   connect( tagInclude[nextTagLine], SIGNAL( stateChanged(int) ), this, SLOT( doFilterChanged() ) );
   connect( tagExclude[nextTagLine], SIGNAL( stateChanged(int) ), this, SLOT( doFilterChanged() ) );
   nextTagLine++;
   /* and now sort it */
   i=0;
   while(i<nextTagLine-1)
     {
	if (strcmp(tagLines[i]->text(),tagLines[i+1]->text())==1)
	  {
	     QString txt = tagLines[i]->text();
	     tagLines[i]->setText(tagLines[i+1]->text());
	     tagLines[i+1]->setText(txt);
	     i=0;
	  }
	else
	  i++;
     }
}
