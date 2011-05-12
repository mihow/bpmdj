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
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qlcdnumber.h>
#include <ctype.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qgroupbox.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qmultilinedit.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>
#include "songselector.logic.h"
#include "preferences.h"
#include "about.h"
#include "tagbox.h"
#include "qsongviewitem.h"
#include "kbpm-played.h"
#include "askinput.h"
#include "version.h"
#include "kbpm-dj.h"
#include "bpmbounds.h"

extern "C"
{
#include "cbpm-index.h"
#include "edit-distance.h"
}

double SongSelectorLogic::mainTempo=0;
bool SongSelectorLogic::color_played=false;
bool SongSelectorLogic::color_range=false;
bool SongSelectorLogic::color_notondisk=false;
int  SongSelectorLogic::itemcount=0;
SongSelectorLogic *mainWindow;

void ToSwitchOrNotToSwitchSignal(int sig,siginfo_t *info, void* hu)
{
   // uiteindelijk moeten we de monitor switchen
   if (info->si_pid!=mainWindow->monitorpid)
     mainWindow->switchMonitorToMain();
   else
     mainWindow->clearMonitor();
}

SongSelectorLogic::SongSelectorLogic(QWidget*parent=0,const char*name=0) :
  SongSelector(parent,name)
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
   
   // create menu structure
   QMenuBar *main = new QMenuBar(this);
   QPopupMenu *help = new QPopupMenu(this);
   QPopupMenu *tools = new QPopupMenu(this);
   QPopupMenu *file = new QPopupMenu(this);
   QPopupMenu *selection = new QPopupMenu(this);
               view = new QPopupMenu(this);
   file->insertItem("&Quit",this,SLOT(quitButton()));
   help->insertItem("&About",this,SLOT(doAbout()));
   help->insertItem("&Read Manual",this,SLOT(doHelp()));
   // retrieves from ./music all mp3's and creates index files for them
   tools->insertItem("&Import mp3's",this,SLOT(importMp3s()));
   // checks wether the songs on a disk can be found somewhere in the indices
   tools->insertItem("&Check cdrom content",this,SLOT(checkDisc()));
   // searches the current list and marks all MD5sum duplicates
   tools->insertItem("Mark duplicates (experimental)",this,SLOT(doMarkDups()));
   tools->insertItem("Find similar song names... (experimental)",this,SLOT(findsimilarnames()));
   tools->insertItem("Find all similar song names... (experimental)",this,SLOT(findallsimilarnames()));
   tools->insertSeparator();
   tools->insertItem("&Preferences",this,SLOT(doPreferences()));
   neglectdirstruct_item = view->insertItem("&Neglect index directory structure",this,SLOT(toggle_neglectdirstruct()));
   view->insertSeparator();
   notyetplayed_item = view->insertItem("&Show only not yet played songs",this,SLOT(toggle_notyetplayed()));
   coloralreadyplayed_item = view->insertItem("&Color already played songs",this,SLOT(toggle_coloralreadyplayed()));
   view->insertSeparator();
   onlytemporange_item = view->insertItem("&Only show songs within tempo range (-0.06 < BPM < 0.06)",this,SLOT(toggle_onlytemporange()));
   colorinrange_item = view->insertItem("Color songs in range",this,SLOT(toggle_colorinrange()));
   view->insertSeparator();
   onlyondisk_item = view->insertItem("Show only files on disk",this,SLOT(toggle_onlyondisk()));
   colornotondisk_item = view->insertItem("Color files not on disk",this,SLOT(toggle_colornotondisk()));
   view->setCheckable(true);
   view->setItemChecked(neglectdirstruct_item,true);
   view->setItemChecked(coloralreadyplayed_item,true);
   view->setItemChecked(colorinrange_item,true);
   view->setItemChecked(onlyondisk_item,true);
   selection->insertItem("&Add tag...",this,SLOT(selectionAddTags()));
   selection->insertItem("&Measure Bpm ...",this,SLOT(measureBpms()));
   selection->insertSeparator();
   selection->insertItem("&Fetch selection from cdrom...",this,SLOT(fetchSelection()));
   selection->insertItem("E&xport xmms playlist",this,SLOT(exportPlayList()));
   main->insertItem("&File",file);
   main->insertItem("&View",view);
   main->insertItem("&Selection",selection);
   main->insertItem("&Tools",tools);
   main->insertItem("&Help",help);
};

void SongSelectorLogic::toggle_neglectdirstruct()
{
   toggleItem(neglectdirstruct_item);
}

void SongSelectorLogic::toggle_notyetplayed()
{
   toggleItem(notyetplayed_item);
}

void SongSelectorLogic::toggle_coloralreadyplayed()
{
   toggleItem(coloralreadyplayed_item);
}

void SongSelectorLogic::toggle_onlytemporange()
{
   toggleItem(onlytemporange_item);
}

void SongSelectorLogic::toggle_colorinrange()
{
   toggleItem(colorinrange_item);
}

void SongSelectorLogic::toggle_onlyondisk()
{
   toggleItem(onlyondisk_item);
}

void SongSelectorLogic::toggle_colornotondisk()
{
   toggleItem(colornotondisk_item);
}

void SongSelectorLogic::toggleItem(int which)
{
   view->setItemChecked(which,!view->isItemChecked(which));
   doFilterChanged();
}

void SongSelectorLogic::doHelp()
{
   if (!fork())
     {
	system("gv /usr/share/doc/bpmdj/beatmixing.ps");
	exit(0);
     }
}

void SongSelectorLogic::injectDataSet(SongIndex *data)
{
   dataRoot=data;
   // add all tags
   addTag("");
   findTags(data);
   // enable the first everything tag
   tagInclude[0] -> setChecked(true);
   // update the item list
   updateItemList();
}

void SongSelectorLogic::clearMonitor()
{
   if (playingInMonitor)
     {
	// printf("Clearing monitor %s\n",playingInMonitor->song_index);
	playingInMonitor->reread();
	parseTags(playingInMonitor->song_tags);
	mainWindow->monitorpid=0;
	playingInMonitor=NULL;
     }
}

void SongSelectorLogic::findTags(Song* song)     
{
   if (!song) return;
   if (song->isIndex())
     findTags(((SongIndex*)song)->list);
   findTags(song->next);
   // parsing of the tags
   parseTags(song->song_tags);
}

void SongSelectorLogic::parseTags(char* tags)
{
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
   if (view->isItemChecked(neglectdirstruct_item))
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
	if (view->isItemChecked(neglectdirstruct_item)) return false;
	return true;
     }
   // song on disk ?
   if (view->isItemChecked(onlyondisk_item) && !item->song_ondisk)
     return false;
   // song played ?
   if (strcmp(item->song_index,mainMix->text())==0)
     item->song_played=true;
   if (view->isItemChecked(notyetplayed_item) && item->song_played)
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
   if (item->song_tempo && view->isItemChecked(onlytemporange_item))
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
   mainTempo=monitorTempo;
   sprintf(tmp,"%g",mainTempo);
   mainTempoText->setText(strdup(tmp));
   /* clear the monitor */
   monitorTitle->setText("");
   monitorAuthor->setText("");
   monitorMix->setText("");
   monitorTempo=0;
   monitorpid=0;
   /* clear the main */
   if (playingInMain)
     {
	playingInMain->reread();
	parseTags(playingInMain->song_tags);
     }
   //printf("Clearing main %s\n",playingInMain->song_index);
   /* ok, the song is already playing, now use the next command next time */
   monitorPlayCommand=(monitorPlayCommand == 1 ? 2 : 1);
   playingInMain=playingInMonitor;
   playingInMonitor=NULL;
   /* write playing sucker to disk */
   Played::Play(mainMix->text());
   /* update view */
   if (view->isItemChecked(neglectdirstruct_item))
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

bool SongSelectorLogic::lookfor(const char* filename,Song* cur)
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

bool exists(char* fn)
{
   FILE * f = fopen(fn,"rb");
   if (f)
     {
	fclose(f);
	return true;
     }
   return false;
}

void SongSelectorLogic::checkfile(const char* filename, ScanningProgress * progress)
{
   // file should end on .mp3 or .MP3
   if (strlen(filename)<4) return;
   if (stricmp(filename+strlen(filename)-4,".mp3")==0)
     {
	// printf("Looking for %s\n",filename);
	if (!lookfor(filename,dataRoot))
	  {	     
	     char *temp;
	     // replace last 3 chars with .idx
	     char indexname[500];
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
	     // good create necessary data
	     index_init();
	     index_readfrom=strdup(indexname);
	     
	     char log[500];
	     sprintf(log,"Writing %s",index_readfrom);
	     progress->Created->insertLine(log);
	     progress->Created->setCursorPosition(progress->Created->numLines(),1);
	     app->processEvents();
	     
	     index_setversion();
	     index_file=strdup(filename);
	     index_changed=true;
	     index_tags=strdup("New");
	     index_period=20000;
	     index_write();
	     index_free();
	     // inform dataroot of new file
	     Song * ns = new Song(halfindexname,INDEX_DIR);
	     assert(ns);
	     assert(dataRoot);
	     dataRoot->add(ns);
	     parseTags(ns->song_tags);
	     updateItemList();
	  }
     }
}

void SongSelectorLogic::scandir(const char* dirname, const char* checkname, ScanningProgress * progress)
{
   DIR *dir;
   struct dirent*entry;
   dir=opendir(dirname);
   if (!dir) return;
   
   char tmp[500];
   sprintf(tmp,"Recursing into %s",dirname);
   progress->Log->insertLine(tmp);
   progress->Log->setCursorPosition(progress->Log->numLines(),1);
   app->processEvents();
			     
   while (entry=readdir(dir))
     {
	if (strcmp(entry->d_name,".")==0 ||
	    strcmp(entry->d_name,"..")==0) continue;
	char txt[500];
	char txt2[500];
	if (checkname)
	  sprintf(txt,"%s/%s",checkname,entry->d_name);
	else
	  sprintf(txt,"%s",entry->d_name);
	sprintf(txt2,"%s/%s",dirname,entry->d_name);
	scandir(txt2,txt,progress);
	checkfile(txt,progress);
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
   ScanningProgress * progress = new ScanningProgress();
   progress->show();
   scandir("/cdrom",strdup(whichCd.LineEdit->text()),progress);
   progress->Okay->setEnabled(true);
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
	fprintf(script,"./music/%s\n",filename);
     }
   fclose(script);
}

void SongSelectorLogic::measureBpms()
{
   /* ask user the given bpm's */
   BpmBounds *bounds = new BpmBounds(this,0,true);
   int res = bounds->exec();
   if (res==QDialog::Rejected)
     return;
   char frombound[500];
   char tobound[500];
   if (bounds->From->text().isEmpty())
     sprintf(frombound,"");
   else 
     sprintf(frombound,"--low %s",(const char*)(bounds->From->text()));
   if (bounds->To->text().isEmpty())
     sprintf(tobound,"");
   else
     sprintf(tobound,"--high %s",(const char*)(bounds->To->text()));
   /* write out executable batch processing for every line */
   FILE* script=fopen("process_bpm.sh","wb");
   fprintf(script,"#!/bin/sh\n");
   assert(script);
   QListViewItemIterator it1(songList);
   for(;it1.current();++it1)
     {
	QSongViewItem *svi=(QSongViewItem*)it1.current();
	if (svi->isSelected()) 
	  fprintf(script,"kbpm-play --batch %s %s \"%s\"\n",frombound,tobound,svi->getSong()->song_index);
     }
   fclose(script);
   chmod("process_bpm.sh",S_IRUSR | S_IWUSR | S_IXUSR);
   /* now start the sucker */
   if (!fork())
     {
	if (system("./process_bpm.sh")<=256) exit(100);
	printf("Error: couldn't execute process_bpm.sh\n");
	exit(100);
     }
}

void SongSelectorLogic::selectSong(QListViewItem *song)
{
   int result;
   char *player;
   Song *matchWith;
   char playercommand[500];
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
   playingInMonitor=((QSongViewItem*)song)->getSong();
   matchWith=playingInMain;
   if (!matchWith) matchWith=playingInMonitor;
   player = monitorPlayCommand == 1 ? playCommand1 : playCommand2;
   sprintf(playercommand, player, matchWith->song_index, playingInMonitor->song_index);
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
   PreferencesDialog preferences(NULL,NULL,TRUE);
   preferences.playerCommand1->setText(playCommand1);
   preferences.playerCommand2->setText(playCommand2);
   sprintf(tmp,"%d",yellowTime);
   preferences.yellowTime->setText(tmp);
   sprintf(tmp,"%d",orangeTime);
   preferences.orangeTime->setText(tmp);
   sprintf(tmp,"%d",redTime);
   preferences.redTime->setText(tmp);
   if (preferences.exec()==QDialog::Accepted)
     {
	playCommand1=strdup(preferences.playerCommand1->text());
	playCommand2=strdup(preferences.playerCommand2->text());
	yellowTime=atoi(preferences.yellowTime->text());
	orangeTime=atoi(preferences.orangeTime->text());
	redTime=atoi(preferences.redTime->text());
     }
}

void SongSelectorLogic::measureBpm(QListViewItem *song)
{
   // time to fork and wait for return ....
   if (!fork())
     {
	char  toexecute[1024];
	// -s makes sure the bpm-counter starts immediatelly
	// -S makes sure the bpm-counter stops afterwards
	// -w specifies the index file to use
	// and the argument is the .mp3 in question
	// WVB -- this should move to the preference dialog
	sprintf(toexecute,"./kbpm-count -s -S -w \"%s\" \"music/%s\"",( const char*)(song->text(LIST_INDEX)),(const char*)(song->text(LIST_FILE)));
	if (system(toexecute)==-1)
	  {
	     sprintf(toexecute,"kbpm-count -s -S -w \"%s\" \"music/%s\"",( const char*)(song->text(LIST_INDEX)),(const char*)(song->text(LIST_FILE)));
	     if (system(toexecute)==-1)
	       {
		  printf("Failed executing kbpm-count\n");
	       }
	  }
	exit(0);
     }
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
   TagBox songdata(NULL,NULL,TRUE);
   songdata.newTags->setText("");
   if (songdata.exec()==QDialog::Accepted)
     {
	QListViewItemIterator it1(songList);
	for(;it1.current();++it1)
	  {
	     QSongViewItem *svi=(QSongViewItem*)it1.current();
	     if (svi->isSelected()) 
	       songAddTag(svi,songdata.newTags->text());
	  }
	parseTags(strdup(songdata.newTags->text()));
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
   color_range=view->isItemChecked(colorinrange_item);
   color_notondisk=view->isItemChecked(colornotondisk_item);
   color_played=view->isItemChecked(coloralreadyplayed_item);
   txt1=searchLine->text();
   txt2=strupper(txt1);
   searchLine->setText(txt2);
   free(txt2);
   updateItemList();
}

void SongSelectorLogic::quitButton()
{
   close();
}
				    
void SongSelectorLogic::addTag(const char* tag)
{
   int baseline=40;
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
   tagLines[nextTagLine]   -> show();
   tagInclude[nextTagLine] =  new QCheckBox( GroupBox2, "" );
   tagInclude[nextTagLine] -> setGeometry( QRect( 10+col*xbox, baseline+30*row, 20, 30 ) );
   tagInclude[nextTagLine] -> setText( tr( "" ) );
   tagInclude[nextTagLine] -> show();
   tagExclude[nextTagLine] =  new QCheckBox( GroupBox2, "" );
   tagExclude[nextTagLine] -> setGeometry( QRect( 30+col*xbox, baseline+30*row, 20, 30 ) );
   tagExclude[nextTagLine] -> setText( tr( "" ) );
   tagExclude[nextTagLine] -> show();
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

void SongSelectorLogic::findallsimilarnames()
{
   bool ok;
   // ask the name to start with
   QString text = QInputDialog::getText("Input name to compare with","label","insert name",&ok);
   if (ok && !text.isEmpty())
     {
	dist_init();
	findallsimilarnamesindir(text);
	dist_done();
     }
}

void SongSelectorLogic::findallsimilarnamesindir(const char* dirname)
{
   DIR *dir;
   struct dirent*entry;
   printf("scanning directory %s\n",dirname);
   dir=opendir(dirname);
   assert(dir);
   while (entry=readdir(dir))
     {
	char txt[500];
	sprintf(txt,"%s/%s",dirname,entry->d_name);
	if (entry->d_type==DT_DIR)
	  {
	     if (strcmp(entry->d_name,".")==0 ||
		 strcmp(entry->d_name,"..")==0) continue;
	     findallsimilarnamesindir(txt);
	  }
	else if (entry->d_type==DT_REG)
	  {
	     int l;
	     char txt2[500];
	     strcpy(txt2,entry->d_name);
	     l=strlen(txt2);
	     if (l>4)
	       {
		  if (strcasecmp(txt2+l-4,".mp3")==0)
		    {
		       txt2[l-4]=0;
		       findsimilarnames(txt2,txt);
		    }
	       }
	  }
     }
   closedir(dir);
}

void SongSelectorLogic::findsimilarnames(const char* text, const char* fullname)
{
//   printf("going in with %s...\n",text);
   QListViewItemIterator it1(songList);
   for(;it1.current();++it1)
     {
	QSongViewItem *svi=(QSongViewItem*)it1.current();
	if (svi)
	  {
	     Song *song = svi->getSong();
	     if (song)
	       {
		  if (!song->isIndex())
		    {
		       const char* t = song->song_title;
		       const char* a = song->song_author;
		       assert(t && a);
		       int val=ndist(t,a,text);
		       if (val<(strlen(text)/10)+3)
			 {
			    printf("%s <-> %s[%s] %d\n",text,t,a,val);
			    if (fullname)
			      {
				 char d[1024];
				 sprintf(d,"mpg123 -0 -4 -t \"%s\" 2>/tmp/p; grep kbit /tmp/p; tail -n 1 /tmp/p",fullname);
				 system(d);
				 sprintf(d,"mpg123 -0 -4 -t \"./music/%s\" 2>/tmp/p; grep kbit /tmp/p; tail -n 1 /tmp/p",song->song_file);
				 system(d);
				 printf("\n");
			      }
			 }
		    }
	       }
	  }
     }
//   printf("coming out...\n");
}

void SongSelectorLogic::findsimilarnames()
{
   bool ok;
   // ask the name to start with
   QString text = QInputDialog::getText("Input name to compare with","label","insert name",&ok);
   if (ok && !text.isEmpty())
     {
	dist_init();
	findsimilarnames(text,NULL);
	dist_done();
     }
}

const char* SongSelectorLogic::askDir()
{
   //   QFileDialog *filedialog = new QFileDialog(0,0,true);
   //   filedialog->exec()
   QString path = QFileDialog::getExistingDirectory(NULL,this,NULL,"Specify directory");
   if (path.isEmpty()) return NULL;
   return strdup((const char*)path);
}

void SongSelectorLogic::importMp3s()
{
//   const char * dir = askDir();
//   if (dir==NULL) return;
   // scandir requires two arguments. The first is the absolute position to scan
   // the second is how the entries should be named relatively to the music dir
   // therefore dirname should point to a location relatively to music
//   printf("Direcotry to scan = %s\n",dir);
   
   // gdvdmme..
   // het probleem hier is onmiddelijk dat we niet weten hoe de gegeven directory zich verhoud tot de
   // music directory.
   ScanningProgress * progress = new ScanningProgress();
   progress->show();
   scandir(MUSIC_DIR,NULL,progress);
   progress->Okay->setEnabled(true);
}
