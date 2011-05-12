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
#include <qinputdialog.h>
#include <math.h>
#include <qmessagebox.h>
#include <qlcdnumber.h>
#include <ctype.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qgroupbox.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qmultilineedit.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcursor.h>
#include <qtabwidget.h>
#include <assert.h>
#include <stdlib.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qprogressbar.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <qpainter.h>
#include <qcombobox.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>
#include "importscanner.h"
#include "renamer.logic.h"
#include "about.h"
#include "albumbox.h"
#include "qsong.h"
#include "version.h"
#include "kbpm-dj.h"
#include "choose-analyzers.h"
#include "process-manager.h"
#include "similarscanner.h"
#include "cluster.h"
#include "queuedsong.h"
#include "historysong.h"
#include "merger-dialog.h"
#include "pca.h"
#include "songtree.h"
#include "avltree.cpp"
#include "edit-distance.h"
#include "compacter.h"
#include "index-reader.h"
#include "scripts.h"
#include "spectrum.h"

SongSelectorLogic::~SongSelectorLogic()
{
  delete processManager;
  delete database;
  // timer=new QTimer(this); ???
}

SongSelectorLogic::SongSelectorLogic(QWidget * parent, const QString name) :
  SongSelector(parent,name)
{
  // link with the processmanager
  processManager = new ProcessManager(this);
  database = new DataBase();
  // initialise fields
  mainTicks=0;
  timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()), SLOT(timerTick()));
  timer->start(1000);
  albumList->installEventFilter(this);
  queue->installEventFilter(this);
  
  // create menu structure
  QMenuBar *main = new QMenuBar(this);
  QPopupMenu *help = new QPopupMenu(this);
  QPopupMenu *before = new QPopupMenu(this);
  QPopupMenu *file = new QPopupMenu(this);
  selection = new QPopupMenu(this);
  queuemenu = new QPopupMenu(this);
  view = new QPopupMenu(this);
  autom = new QPopupMenu(this);

  // file menu
  file->insertItem("&Preferences",this,SLOT(doPreferences()));
  file->insertItem("&Switch Monitor To Main",this,SLOT(switchMonitorToMain()));
  file->insertItem("Play &Mixers", this, SLOT (openMixer()));
  file->insertSeparator();
  file->insertItem("Record", this, SLOT (openRecorder()));
  file->insertItem("Replay", this, SLOT (openReplay()));
  file->insertItem("Record Mixers", this, SLOT (openRecordMixer()));
  file->insertSeparator();
  file->insertItem("&Quit",this,SLOT(quitButton()));
  
  // help menu
  help->insertItem("&About",this,SLOT(doAbout()));
  help->insertItem("Online &Manual",this,SLOT(doOnlineHelp()));
  help->insertItem("&Legende",this,SLOT(doLegende()));
  help->insertItem("L&icense",this,SLOT(doLicense()));
  
  // before menu
  before->insertItem("&Import songs ("SONG_EXT")",this,SLOT(importSongs())); // retrieves from ./music all songs
  before->insertItem("Find similar song names in directory...",this,SLOT(findallsimilarnames()));
  before->insertItem("Find songs with wrong name...",this,SLOT(findWrongSongNames()));
  before->insertSeparator();
  before->insertItem("&Check/import cdrom content",this,SLOT(checkDisc())); // checks wether the songs on a disk can be found somewhere in the indices
  before->insertItem("Mark duplicates (experimental)",this,SLOT(doMarkDups())); // searches the current list and marks all MD5sum duplicates
  before->insertItem("Find wrong index file names...",this,SLOT(findWrongIdxNames()));
  before->insertItem("Compact index directory...",this,SLOT(compactIdxDirectory()));

  // view color menu
  view->insertSeparator();
  coloralreadyplayed_item = view->insertItem("Color already played titles",this,SLOT(toggle_coloralreadyplayed()));
  colorauthorplayed_item  = view->insertItem("Color already played authors",this,SLOT(toggle_colorauthorplayed()));
  colorinrange_item       = view->insertItem("Color songs within tempo range",this,SLOT(toggle_colorinrange()));
  colorsongondisk_item    = view->insertItem("Color songs not on disk",this,SLOT(toggle_colorsongsnotondisk()));
  colorcues_item          = view ->insertItem("Color songs without cues",this,SLOT(toggle_colorsongswithoutcues()));
  colordcolor_item        = view -> insertItem("Color spectrum distance",this, SLOT(toggle_colordcolor()));
  colorspectrum_item      = view -> insertItem("Color PCA of spectrum",this,SLOT(toggle_colorspectrum()));
  
  view->setItemChecked(coloralreadyplayed_item, Config::color_played);
  view->setItemChecked(colorinrange_item      , Config::color_range);
  view->setItemChecked(colorauthorplayed_item , Config::color_authorplayed);
  view->setItemChecked(colorsongondisk_item   , Config::color_ondisk);
  view->setItemChecked(colorcues_item         , Config::color_cues);
  view->setItemChecked(colordcolor_item       , Config::color_dcolor);
  view->setItemChecked(colorspectrum_item     , Config::color_spectrum);

  // view display part
  view->insertSeparator();
  temporange_item         = view->insertItem("Limit selection to &Tempo range",this,SLOT(toggle_temporange()));
  onlyuptemporange_item   = view->insertItem("&Up Tempo Range (0 < BPM < 0.06)",this,SLOT(toggle_onlyuptemporange()));
  onlydowntemporange_item = view->insertItem("&Down Tempo Range (-0.06 < BPM < 0)",this,SLOT(toggle_onlydowntemporange()));
  notyetplayed_item       = view->insertItem("Limit selection to &Non-played songs",this,SLOT(toggle_notyetplayed()));
  onlyondisk_item         = view->insertItem("Limit selection to &Files on disk",this,SLOT(toggle_onlyondisk()));
  onlyindistance_item     = view->insertItem("Limit selection to &dColor range",this,SLOT(toggle_onlyindistance()));
  onlynonplayedauthors_item = view->insertItem("Limit selection to non played authors",this,SLOT(toggle_onlynonplayedauthors()));  
  view->setCheckable(true);
  view->setItemChecked(onlyondisk_item,         Config::limit_ondisk);
  view->setItemChecked(notyetplayed_item,       Config::limit_nonplayed);
  view->setItemChecked(onlyuptemporange_item,   Config::limit_uprange);
  view->setItemChecked(onlydowntemporange_item, Config::limit_downrange);
  view->setItemChecked(onlyindistance_item,     Config::limit_indistance);
  view->setItemChecked(onlynonplayedauthors_item, Config::limit_authornonplayed);

  // auto menu
  auto_popq_item   = autom->insertItem("Pop queue when monitor empty",this,SLOT(toggle_autopop()));
  auto_askmix_item = autom->insertItem("Ask 'how did the mix go'",this,SLOT(toggle_askmix()));
  auto_mixer_item  = autom->insertItem("Open mixer when starting program",this,SLOT(toggle_openmixer()));
  
  autom->setCheckable(true);
  autom->setItemChecked(auto_popq_item,    Config::auto_popqueue);
  autom->setItemChecked(auto_askmix_item,  Config::ask_mix);
  autom->setItemChecked(auto_mixer_item,   Config::open_mixer);
  
  // selection menu
  selection->insertItem("&Add tag...",this,SLOT(selectionAddTags()));
  selection->insertItem("&Delete tag...",this,SLOT(selectionDelTags()));
  selection->insertItem("&Edit info...",this,SLOT(selectionEditInfo()));
  selection->insertItem("&Insert in album...",this,SLOT(selectionInsertInAlbum()));
  selection->insertItem("&Analyze Bpm/Spectrum ...",this,SLOT(batchAnalyzing()));
  selection->insertItem("Pca Analysis Sound Color",this,SLOT(doSpectrumPca()));
  // selection->insertItem("Cluster Analysis Sound Color",this,SLOT(doSpectrumClustering()));
  selection->insertSeparator();
  selection->insertItem("This is the main song, but don't play it",this,SLOT(selectionSetMainSong()));
  selection->insertItem("Play in 1st Extra Player",this,SLOT(selectionPlayIn3th()));
  selection->insertItem("Play in 2nd Extra Player",this,SLOT(selectionPlayIn4th()));
  selection->insertItem("Add to Queue",this,SLOT(selectionAddQueue()));
  selection->insertSeparator();
  selection->insertItem("&Fetch selection from cdrom...",this,SLOT(fetchSelection()));
  selection->insertSeparator();
  selection->insertItem("Select all b&ut tagged",this,SLOT(selectAllButTagged()));

  // queue menu
  queuemenu->insertItem("Select...",this,SLOT(queueSelectSong()));
  queuemenu->insertItem("Insert random songs...",this,SLOT(queueInsert()));
  queuemenu->insertItem("Random",this,SLOT(queueRandom()));
  queuemenu->insertSeparator();
  queuemenu->insertItem("Toggle Anker",this,SLOT(queueAnker()));
  queuemenu->insertItem("Shift Up",this,SLOT(queueShiftUp()));
  queuemenu->insertItem("Shift Down",this,SLOT(queueShiftDown()));
  queuemenu->insertItem("Delete Song",this,SLOT(queueDelete()));
  queuemenu->insertSeparator();
  queuemenu->insertItem("Export xmms playlist",this,SLOT(exportPlayList()));
  queuemenu->insertItem("Copy songs to directory...",this,SLOT(queueCopySongs()));  
  
  // main menu
  main->insertItem("&Program",file);
  main->insertItem("&View",view);
  main->insertItem("&Auto",autom);

  main->insertItem("&Selection",selection);
  main->insertItem("&Queue",queuemenu);
  main->insertItem("&Management",before);
  main->insertItem("&Help",help);
  
  // nog een aantal items toevoegen
  //   create remote/local script pair
  //   R- copy index files to other side
  //   R- position of binaries
  //   R- start player
  //   R- position to start player
  //   R- copy files back
  //   L- give shell to start here
  // mounten van music directory
};

void SongSelectorLogic::toggle_autopop()
{
  toggleAutoItem(auto_popq_item);
}


void SongSelectorLogic::toggle_askmix()
{
  toggleAutoItem(auto_askmix_item);
}

void SongSelectorLogic::toggle_openmixer()
{
  toggleAutoItem(auto_mixer_item);
}

void SongSelectorLogic::toggle_notyetplayed()
{
  toggleItem(notyetplayed_item);
}

void SongSelectorLogic::toggle_onlynonplayedauthors()
{
  toggleItem(onlynonplayedauthors_item);
}

void SongSelectorLogic::toggle_coloralreadyplayed()
{
  toggleItem(coloralreadyplayed_item);
}

void SongSelectorLogic::toggle_colorauthorplayed()
{
  toggleItem(colorauthorplayed_item);
}

void SongSelectorLogic::toggle_colorsongsnotondisk()
{
  toggleItem(colorsongondisk_item);
}

void SongSelectorLogic::toggle_colorsongswithoutcues()
{
  toggleItem(colorcues_item);
}

void SongSelectorLogic::toggle_colordcolor()
{
  toggleItem(colordcolor_item);
}

void SongSelectorLogic::toggle_colorspectrum()
{
  toggleItem(colorspectrum_item);
}

void SongSelectorLogic::toggle_onlyuptemporange()
{
  toggleItem(onlyuptemporange_item);
}

void SongSelectorLogic::toggle_onlydowntemporange()
{
  toggleItem(onlydowntemporange_item);
}

void SongSelectorLogic::toggle_temporange()
{
  toggleItem(onlyuptemporange_item);
  toggleItem(onlydowntemporange_item);
}

void SongSelectorLogic::toggle_colorinrange()
{
  toggleItem(colorinrange_item);
}

void SongSelectorLogic::toggle_onlyondisk()
{
  toggleItem(onlyondisk_item);
}

void SongSelectorLogic::toggle_onlyindistance()
{
  toggleItem(onlyindistance_item);
}

void SongSelectorLogic::toggleItem(int which)
{
  view->setItemChecked(which,!view->isItemChecked(which));
  doFilterChanged();
}

void SongSelectorLogic::toggleAutoItem(int which)
{
  autom->setItemChecked(which,!autom->isItemChecked(which));
  doAutoFilterChanged();
}

void SongSelectorLogic::doOnlineHelp()
{
  QMessageBox::warning(this,"Online","Please browse to http://bpmdj.sourceforge.net/");
}

void SongSelectorLogic::findAllTags()
{
  // 1. The Tags...
  // ---------------
  // first put all tags in tree, without duplicates
  int count;
  Song ** songs = database->getAllSongs(count);
  AvlTree<QString> *tree = new AvlTree<QString>();
  for(int i = 0 ; i < count ; i ++)
    {
      QString tags = songs[i]->tags;
      if (tree->search(tags)) continue;
      tree->add(new QStringNode(tags));
    }
  // then parse all the tags
  QStringNode *top;
  while( (top = (QStringNode*)(tree->top())) )
    {
      parseTags(top->content);
      tree->del(top->content);
    }
  delete(tree);
  // update the item list
  updateItemList();
  // do a pca on all the data
  doSpectrumPca(true);

  // 2. The Albums
  // -------------
  albumList->clear();
  for(int i = 0 ; i < count ; i ++)
    {
      Song * song = songs[i];
      AlbumField ** albums = song -> albums;
      while(*albums)
	{
	  AlbumField * album = *albums++;
	  AlbumItem  * ai = (AlbumItem*)albumList->findItem(album->name,0);
	  if (!ai) ai = new AlbumItem(album->name,albumList);
	  new AlbumItem(album->nr,song,ai);
	}
    }
}

void SongSelectorLogic::parseTags(QString tagz)
{
  char * original = strdup(tagz);
  char * tags = original;
  if (!tags) return;
  char * runner=tags;
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
  free(original);
}

void SongSelectorLogic::acceptNewSong(Song* song)
{
  database->add(song);
  parseTags(song->tags);
}

static bool alreadygavefilterwarning = false;

void SongSelectorLogic::updateItemList()
{
  // if no song is playing...
  // backup & untoggle certain properties..
  bool backup_uprange = Config::limit_uprange;
  bool backup_downrange = Config::limit_downrange;
  bool backup_distancerange = Config::limit_indistance;
  if (!processManager->playing_songs[0] &&
      !processManager->playing_songs[1])
    {
      view->setItemChecked(onlyuptemporange_item,Config::limit_uprange = false);
      view->setItemChecked(onlydowntemporange_item,Config::limit_downrange = false);
      view->setItemChecked(onlyindistance_item,Config::limit_indistance = false);
    }
  // update selection
  Song * main = processManager->playingInMain();
  songList -> clear();
  int itemcount = database -> getSelection(this,main,songList);
  songList->repaint();
  countLcd->display(itemcount);
  // nothing selected ?
  if (itemcount==0 && !alreadygavefilterwarning)
    {
      QMessageBox::warning(this,"Empty Selection","With the current selection of filters,\n"
			   "there are no songs to be displayed.\n"
			   "If you want to modify this, go to the View menu.");
      alreadygavefilterwarning = true;
    }
  // restore
  if (!processManager->playing_songs[0] &&
      !processManager->playing_songs[1])
    {
      view->setItemChecked(onlyuptemporange_item,Config::limit_uprange = backup_uprange);
      view->setItemChecked(onlydowntemporange_item,Config::limit_downrange = backup_downrange);
      view->setItemChecked(onlyindistance_item,Config::limit_indistance = backup_distancerange);
    }
  
  // now update the history view
  historyList->clear();
  if (main)
    {
      main->realize();
      Index main_index(main->storedin);
      AvlTree<QString> *file2song = database->getFileTreeRef();
      HistoryField ** after = main_index . get_prev_songs();
      while(after && *after)
	{
	  char* file = (*after)->file;
	  QString count = "-"+QString::number((*after)->count);
	  QString comment = (*after)->comment;
	  SongSortedByFile *s = (SongSortedByFile*)file2song->search(file);
	  new HistorySong(s->song,count,comment,historyList);
	  s->song->getDistance();
	  after++;
	}
      HistoryField ** before = main_index . get_next_songs();
      while (before && *before)
	{
	  char* file = (*before)->file;
	  QString count = "+"+QString::number((*before)->count);
	  QString comment = (*before)->comment;
	  SongSortedByFile *s = (SongSortedByFile*)file2song->search(file);
	  s->song->getDistance();
	  new HistorySong(s->song,count,comment,historyList);
	  before++;
	}
    }
}

void SongSelectorLogic::timerTick()
{
  mainLCD->display(++mainTicks);
  if (mainTicks==1)
    setColor(QColor(0,255,0));
  else if (mainTicks==Config::yellowTime)
    setColor(QColor(255,255,0));
  else if (mainTicks==Config::orangeTime)
    setColor(QColor(255,127,0));
  else if (mainTicks==Config::redTime)
    setColor(QColor(255,0,0));
  processManager->checkSignals();
  
  // auto pop queue
  if (Config::auto_popqueue)
    {
      if (processManager->monitorFree())
	{
	  QListViewItemIterator it(queue);
	  QueuedSong * s = (QueuedSong*)it.current();
	  if (s)
	    {
	      if (s->getSong()) 
		processManager->startSong(s->getSong());
	      delete(s);
	      queueOrder();
	    }
	}
    }  
}

void SongSelectorLogic::setColor(QColor color)
{
  countLcd->setBackgroundColor(color);
  mainLCD->setBackgroundColor(color);
}

void SongSelectorLogic::setPlayerColor(QLabel *player ,QColor color)
{
  player->setPaletteBackgroundColor(color);
}

void SongSelectorLogic::playersChanged()
{
}

void SongSelectorLogic::resetCounter()
{
  mainTicks=-1;
}

void SongSelectorLogic::switchMonitorToMain()
{
  // forced switch
  processManager->switchMonitorToMain();
}

void SongSelectorLogic::updateProcessView()
{
  // set main song
  Song * song = processManager->playingInMain();
  QColor color;
  color.setHsv(0,0,128);
  if (song)
    {
      title1->setText(song->getDisplayTitle());
      tempo1->setText(song->tempo);
      color=song->color;
    }
  else
    {
      title1->setText(QString::null);
      tempo1->setText(QString::null);
    }
  setPlayerColor(title1,color);
  
  // set monitor song
  song = processManager->playing_songs[1];
  color.setHsv(0,0,128);
  if (song)
    {
      title2->setText(song->getDisplayTitle());
      tempo2->setText(song->tempo);
      color = song->color;
    }
  else
    {
      title2->setText(QString::null);
      tempo2->setText(QString::null);
    }
  setPlayerColor(title2,color);

  // set first extra player
  song = processManager->playing_songs[2];
  color.setHsv(0,0,128);
  if (song)
    {
      title3->setText(song->getDisplayTitle());
      tempo3->setText(song->tempo);
      color = song->color;
    }
  else
    {
      title3->setText(QString::null);
      tempo3->setText(QString::null);
    }
  setPlayerColor(title3,color);

  // set second extra player
  song = processManager->playing_songs[3];
  color.setHsv(0,0,128);
  if (song)
    {
      title4->setText(song->getDisplayTitle());
      tempo4->setText(song->tempo);
      color = song->color;
    }
  else
    {
      title4->setText(QString::null);
      tempo4->setText(QString::null);
    }
  setPlayerColor(title4,color);
  
  // update item list
  updateItemList();

  // update the frequency picture
  updateFrequencyMap();
}

void SongSelectorLogic::updateFrequencyMap()
{
  QPixmap *pm = new QPixmap(24,100);
  float mid = 25.0;
  float hh = 25.0 /* the maximum amplitude */
    / (25.0); /* the strange scale - normalisation, see file spectrum.cpp */
  QPainter p;
  p.begin(pm);
  QRect r(0,0,24,100);
  p.fillRect(r,Qt::white);
  
  Song * song_a = processManager->playing_songs[0];
  Song * song_b = processManager->playing_songs[1];
  QString a = song_a ? song_a->spectrum : QString::null;
  QString b = song_b ? song_b->spectrum : QString::null;
  
  for (int j = 0; j < 24 ; j++)
    {
      bool has_a = !a.isNull();
      bool has_b = !b.isNull();
      float a_j, b_j;
      if (has_a)
	{
	  a_j = (char)(a.at(j).latin1()) - 'a';
	  a_j*=scales[j];
	  p.setPen(Qt::red);
	  p.drawPoint(j,(int)(mid-hh*a_j));
	}
      if (has_b)
	{
	  b_j = (char)(b.at(j).latin1()) - 'a';
	  b_j*=scales[j];
	  p.setPen(Qt::blue);
	  p.drawPoint(j,(int)(mid-hh*b_j));
	}
      if (has_a && has_b)
	{
	  p.setPen(Qt::black);
	  p.drawPoint(j,(int)(mid-hh*(a_j-b_j)));
	}
    }
  p.end();
  frequency_map->setPixmap(*pm);
}

int songFileCompare(const void * a, const void* b)
{
  QSong **pa=(QSong**)a;
  QSong **pb=(QSong**)b;
  QSong *A=*pa;
  QSong *B=*pb;
  return strcmp(A->file(),B->file());
}

void SongSelectorLogic::checkDisc()
{
  // 0 - which CD is this ?
  bool ok;
  QString discname = QInputDialog::getText("Check/Import Disc"
					   ,"1. Insert a CD\n2. Enter its number (without trailing slash)"
					   ,QLineEdit::Normal,QString::null,&ok,this);
  if (!ok || discname.isEmpty()) return;
  // 1 - mount cdrom
  execute(MOUNT_CDROM);
  // 2 - Scan directory
  ImportScanner scanner(this);
  scanner.scan(CDROM,discname);
  updateItemList();
  // 3 - umount cdrom
  execute(UMOUNT_CDROM);
  execute(EJECT_CDROM);
}

void SongSelectorLogic::openMixer()
{
  if (Config::mixer_command.isEmpty())
    QMessageBox::message(NULL,"Please insert an appropriate command in the preferences dialog\n");
  else
    spawn(Config::mixer_command);
}

void SongSelectorLogic::openRecorder()
{
  if (Config::record_command.isEmpty())
    QMessageBox::message(NULL,"Please create an appropriate record command in the preferences dialog\n");
  else
    {
      bool ok;
      QString trackname = QInputDialog::getText("Record track"
						,"Please enter the trackname to record"
						,QLineEdit::Normal,QString::null,&ok,this);
      if (ok && !trackname.isEmpty())
	{
	  char command[1024];
	  sprintf(command,(const char*)Config::record_command,(const char*)trackname);
	  spawn(command);
	}
    }
}

void SongSelectorLogic::openReplay()
{
  if (Config::record_command.isEmpty())
    QMessageBox::message(NULL,"Please create an appropriate replay command in the preferences dialog\n");
  else
    {
      bool ok;
      QString trackname = QInputDialog::getText("Replay track"
						,"Please enter the trackname to play"
						,QLineEdit::Normal,QString::null,&ok,this);
      if (ok && !trackname.isEmpty())
	{
	  char command[1024];
	  sprintf(command,(const char*)Config::replay_command,(const char*)trackname);
	  spawn(command);
	}
    }
}

void SongSelectorLogic::openRecordMixer()
{
  if (Config::record_command.isNull())
    QMessageBox::message(NULL,"Please create an appropriate record_mixer command in the preferences dialog\n");
  else
    spawn((const char*)Config::record_mixer_command);
}

void SongSelectorLogic::doSpectrumPca(bool fulldatabase)
{
  int count = 0;
  float **data;
  Song ** all = NULL;
  
  if (fulldatabase)
    {
      all=database->getAllSongs(count);
      data = matrix(count,24);
      if (count == 0 ) return;
      for(int i = 0, written = 0 ; i < count ; i ++)
	{
	  Song *svi= all[i];
	  if (!svi->spectrum.isNull())
	    {
	      for (int i = 0 ; i < 24 ; i++)
		{
		  char letter = svi->spectrum.at(i).latin1()-'a';
		  data[written+1][i+1]=(float)letter;
		}
	      written++;
	    }
	}
    }
  else
    {
      // 0. first count 
      QListViewItemIterator it1(songList);
      count=0;
      for(;it1.current();++it1)
	{
	  QSong *svi=(QSong*)it1.current();
	  if (svi->isSelected())
	    if (svi->spectrum()!=QString::null)
	      count++;
	}
      
      if (count == 0 ) 
	return;

      // 1. fill matrix
      data = matrix(count,24);
      QListViewItemIterator it2(songList);
      for(int written = 0 ; it2.current();++it2)
	{
	  QSong *svi=(QSong*)it2.current();
	  if (svi->isSelected() && svi->isVisible())
	    {
	      if (svi->spectrum()!=QString::null)
		{
		  for (int i = 0 ; i < 24 ; i++)
		    {
		      char letter = svi->spectrum().at(i).latin1()-'a';
		      data[written+1][i+1]=(float)letter;
		    }
		  written++;
		}
	    }
	}
    }

  // 2. do principal component analysis
  char * error_msg = NULL;
  do_pca(count,24,data,error_msg);
  if (error_msg)
    {
      QMessageBox::warning(this,"Principal Component Analysis", "An error occured, aborting pca");
      return;
    }
  
  float minx=0,miny=0,minz=0;
  float maxx=0,maxy=0,maxz=0;
  float dx,dy,dz;
  for(int i = 1 ; i <= count; i ++)
    {
#define MIN(A,B) if (B<A) A=B;
#define MAX(A,B) if (B>A) A=B;
      MIN(minx,data[i][1]);
      MIN(miny,data[i][2]);
      MIN(minz,data[i][3]);
      MAX(maxx,data[i][1]);
      MAX(maxy,data[i][2]);
      MAX(maxz,data[i][3]);
    }
  dx=maxx-minx;
  dy=maxy-miny;
  dz=maxz-minz;
  if (dx==0) dx=1;
  if (dy==0) dy=1;
  if (dz==0) dz=1;
  dx/=128.0;
  dy/=128.0;
  dz/=128.0;

  // 3. modify colors of the selected items
  if (fulldatabase)
    {
      for(int i = 0, written = 0 ; i < count ; i ++)
	{
	  Song *svi= all[i];
	  if (!svi->spectrum.isNull())
	    {
	      written++;
	      float x = data[written][1];
	      float y = data[written][2];
	      float z = data[written][3];
	      x-=minx;
	      y-=miny;
	      z-=minz;
	      x/=dx;
	      y/=dy;
	      z/=dz;
	      QColor transfer;
	      transfer.setRgb(127+(int)x,127+(int)y,127+(int)z);
	      svi->setColor(transfer);
	    }
	}
    }
  else
    {
      QListViewItemIterator it3(songList);
      for(int written = 0 ;it3.current();++it3)
	{
	  QSong *svi=(QSong*)it3.current();
	  if (svi->isSelected() && svi->isVisible())
	    {
	      if (svi->spectrum()!=QString::null)
		{
		  written++;
		  float x = data[written][1];
		  float y = data[written][2];
		  float z = data[written][3];
		  x-=minx;
		  y-=miny;
		  z-=minz;
		  x/=dx;
		  y/=dy;
		  z/=dz;
		  QColor transfer;
		  transfer.setRgb(127+(int)x,127+(int)y,127+(int)z);
		  svi->setColor(transfer);
		}
	    }
	}
    }
  
  // 4. clean up
  free_matrix(data, count, 24);
  updateProcessView();
}

void SongSelectorLogic::doSpectrumClustering()
{
  // 0. add all information in 1 cluster
  Cluster cluster;
  Cluster::reset();
  QListViewItemIterator it1(songList);
  for(;it1.current();++it1)
    {
      QSong *svi=(QSong*)it1.current();
      if (svi->isSelected() && svi->isVisible())
	if (svi->spectrum()!=QString::null)
	  cluster.addPoint(svi->songEssence());
    }
  // 1. Do the Analysis
  Couple * result = cluster.agglomerate(&SongMetriek::SPECTRUM);
  // 2. Assign Colors
  result->determine_color((float)0,(float)360,0,5);
  // 3. Clean Up
  updateProcessView();
}

void SongSelectorLogic::exportPlayList()
{
  // count nr of files to get 
  int selectionCount= queue->childCount();
  int next = 0;
  // allocate enough space to store the songs 
  Song * * toFetch = allocate(selectionCount,Song*);
  // read all the songs
  QListViewItemIterator it2(queue);
  for(;it2.current();++it2)
    {
      QueuedSong *svi=(QueuedSong*)it2.current();
      toFetch[next++]=svi->getSong();
    }
  // write script to disk 
  char* homedir = getenv("HOME");
  char scriptname[1024];
  sprintf(scriptname,"%s/.xmms/xmms.m3u",homedir);
  char * current_directory=getcwd(NULL,0);
  FILE* script=fopen(scriptname,"wb");
  assert(script);
  int i = 0;
  while(i<next)
    {
      Song* song=toFetch[i++];
      if (song)
	{
	  const char* filename=song->file;
	  fprintf(script,"%s/./music/%s\n",current_directory,filename);
	}
    }
  fclose(script);
  free(current_directory);
}

void SongSelectorLogic::fetchSelection()
{
  /* count nr of files to get */
  int selectionCount = 0;
  QListViewItemIterator it1(songList);
  for(;it1.current();++it1)
    {
      QSong *svi=(QSong*)it1.current();
      if (svi->isSelected() && svi->isVisible()) selectionCount++;
    }
  /* allocate enough space to store the songs */
  QSong * * toFetch;
  int next=0;
  toFetch=(QSong**)calloc(selectionCount,sizeof(QSong*));
  assert(toFetch);
  next=0;
  /* read all the songs */
  QListViewItemIterator it2(songList);
  for(;it2.current();++it2)
    {
      QSong *svi=(QSong*)it2.current();
      if (svi->isSelected()  && svi->isVisible())
	toFetch[next++]=svi;
    }
  /* sort the list */
  qsort(toFetch,next,sizeof(QSong*),songFileCompare);
  /* write script to disk */
  
  FILE* script=openScriptFile(FETCHFILES);
  fprintf(script,"# writing %d files\n",next);
  fprintf(script,UMOUNT_CDROM);
  fprintf(script,EJECT_CDROM);
  int i = 0;
  char* currentcd=0;
  while(i<next)
    {
      QSong* song=toFetch[i++];
      const char* filename=song->file();
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
	      fprintf(script,UMOUNT_CDROM);
	      fprintf(script,EJECT_CDROM);
	    }
	  currentcd=newcd;
	  fprintf(script,INSERT_CD_DIALOG,newcd);
	  fprintf(script,MOUNT_CDROM);
	}
      else 
	free(newcd);
      char* dirc=strdup(filename);
      char* filenc=strdup(filename);
      char* dir=dirname(dirc);
      sprintf(temp,"\"./music/%s\"",dir);
      char* target=strdup(temp);
      sprintf(temp,"mkdir -p %s",target);
      fprintf(script,"%s\n",temp);
      sprintf(temp,"\""CDROM"%s\"",index(filename,'/'));
      char* source=strdup(temp);
      sprintf(temp,CP"%s %s\n",source,target);
      fprintf(script,temp);
      free(source);
      free(target);
      free(dirc);
      free(filenc);
    }
  fprintf(script,UMOUNT_CDROM);
  fclose(script);
  
  /* umount any left over cdrom */
  execute(UMOUNT_CDROM);
  execute(EJECT_CDROM);
  /* fetch the files one by one */
  i = 0;
  currentcd = 0;
  while(i<next)
    {
      QSong* song=toFetch[i++];
      const char* filename=song->file();
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
	      if (!execute(UMOUNT_CDROM))
		QMessageBox::warning(this,"CD","Cannot unmount "CDROM);
	      execute(EJECT_CDROM);
	    }
	  /* ask user to input new cd */
	  currentcd=newcd;
	  sprintf(temp,"Please insert %s",newcd);
	  QMessageBox::information(this,"CD",temp);
	  /* mount the cdrom */
	  if (!execute(MOUNT_CDROM))
	    {
	      sprintf(temp,"Cannout mount " CDROM " (errno=%d)",errno);
	      QMessageBox::warning(this,"CD",temp);
	    }
	}
      else 
	free(newcd);
      /* create the target directory */
      char* dirc=strdup(filename);
      char* filenc=strdup(filename);
      char* dir=dirname(dirc);
      sprintf(temp,"\"./music/%s\"",dir);
      char* target=strdup(temp);
      vexecute("mkdir -p %s",target);
      /* read the given file from the cdrom */
      sprintf(temp,"\""CDROM"%s\"",index(filename,'/'));
      char* source=strdup(temp);
      vexecute(CP"%s %s\n",source,target);
      /* mark it on disk */
      song->ondisk(true);
      /* free source & target */
      free(source);
      free(target);
      free(dirc);
      free(filenc);
    }
  /* finally umount the last cdrom */
  execute(UMOUNT_CDROM);
}

void SongSelectorLogic::batchAnalyzing()
{
  char tempoLine[2048];
  char spectrumLine[2048];
  ChooseAnalyzers *bounds = new ChooseAnalyzers(this,0,true);
  int res = bounds->exec();
  if (res==QDialog::Rejected) return;
  tempoLine[0]=spectrumLine[0]=0;
  if (bounds->tempoAnalyzer->isChecked())
    {
      char frombound[500], tobound[500];
      frombound[0]=tobound[0]=0;
      if (!bounds->From->text().isEmpty())
	sprintf(frombound,"--low %s",(const char*)(bounds->From->text()));
      if (!bounds->To->text().isEmpty()) 
	sprintf(tobound,"--high %s",(const char*)(bounds->To->text()));
      int technique;
      if (bounds->resamplingScan->isChecked()) technique = 1;
      else if (bounds->ultraLongFFT->isChecked()) technique = 2;
      else if (bounds->enveloppeSpectrum->isChecked()) technique = 3;
      else if (bounds->fullAutoCorrelation->isChecked()) technique = 4;
      sprintf(tempoLine,"--bpm %d %s %s",technique,frombound,tobound);
    }
  if (bounds->spectrumAnalyzer->isChecked())
    sprintf(spectrumLine,"--spectrum");
  // write out executable batch processing for every line
  FILE* script=openScriptFile(PROCESS_ANALYZERS);
  QListViewItemIterator it2(songList);
  int count;
  for(count = 0;it2.current();++it2)
    {
      QSong *svi=(QSong*)it2.current();
      if (svi->isSelected() && svi->isVisible()) 
	count++;
    }
  QListViewItemIterator it1(songList);
  for(int nr = 0;it1.current();++it1)
    {
      QSong *svi=(QSong*)it1.current();
      if (svi->isSelected()  && svi->isVisible()) 
	{
	  svi->songEssence()->realize();
	  fprintf(script,"echo ======= %d / %d ==============\n",nr++,count-1);
	  fprintf(script,"kbpm-play -q --batch %s %s \"%s\"\n",
		  tempoLine,spectrumLine,
		  (const char*)svi->stored_in());
	}
    }
  fclose(script);
  chmod(PROCESS_ANALYZERS, S_IRUSR | S_IWUSR | S_IXUSR);
  spawn(PROCESS_ANALYZERS);
}


void SongSelectorLogic::selectSong(QListViewItem *song)
{
  if (!song) return;
  processManager->startSong(((QSong*)song)->songEssence());
}

void SongSelectorLogic::playQueueSong(QListViewItem *song)
{
  if (!song) return;
  QueuedSong * qs = (QueuedSong*)song;
  Song * s = qs->getSong();
  if (!s) return;
  processManager->startSong(s);
}

void SongSelectorLogic::playHistorySong(QListViewItem *song)
{
  if (!song) return;
  HistorySong * hs = (HistorySong*)song;
  Song * s = hs->getSong();
  if (!s) return;
  processManager->startSong(s);
}

void SongSelectorLogic::doPreferences()
{
  Config::openUi();
  updateItemList();
}

void SongSelectorLogic::songAddTag(QListViewItem * S, const QString & tag)
{
  QSong * song = (QSong*)S;
  song->songEssence()->realize();
  QString index = song->stored_in();
  // read the index file
  Index * idx = new Index(index);
  // modify taglines
  QString newtags = song->tags()+" "+tag;
  idx->set_tags(strdup(newtags));
  // write the indfex file
  idx->write_idx();
  // free the bastard
  delete idx;
  // now update the local songdata
  ((QSong*)song)->reread();
}

void SongSelectorLogic::insertSongInAlbum(QListViewItem * S, const QString & album, int nr)
{
  QSong * song = (QSong*)S;
  song->songEssence()->realize();
  QString index = song->stored_in();
  // read the index file
  Index * idx = new Index(index);
  if (!idx -> find_album(album))
    {
      idx -> add_album(new AlbumField(nr,album));
      idx -> write_idx();
      // update album view
      AlbumItem * ai = (AlbumItem*)albumList->findItem(album,0);
      if (!ai) ai = new AlbumItem(album,albumList);
      new AlbumItem(nr,song->songEssence(),ai);
    }
  delete idx;
}

void SongSelectorLogic::songEditInfo(QListViewItem * S)
{
  QSong * song = (QSong*)S;
  song->songEssence()->realize();
  QString index = song->stored_in();
  Index * idx = new Index(index);
  idx->executeInfoDialog();
  if (idx->changed())
    {
      idx->write_idx();
      delete idx;
      ((QSong*)song)->reread();
      return;
    }
  delete idx;
}

void SongSelectorLogic::songDelTag(QListViewItem *S, const QString & tag)
{
  QSong * song = (QSong*)S;
  song->songEssence()->realize();
  char* newtags;
  // read the index file
  Index * index = new Index(song->stored_in());
  newtags=strdup(index->get_tags());
  // modify taglines
  char* pos = strstr(index->get_tags(),(const char*)tag);
  if (pos)
    {
      // simply clear the sucker
      newtags[pos-index->get_tags()]=' ';
      strcpy(newtags+(pos-index->get_tags())+1, pos+strlen(tag));
      index->set_tags(strdup(newtags));
    }
  // write the index file
  index->write_idx();
  // free the bastard
  delete index;
  // now update the local songdata
  ((QSong*)song)->reread();
}

void SongSelectorLogic::selectionAddTags()
{


  bool ok;
  QString tags = QInputDialog::getText("Tags"
				       ,"Enter tags to add (seperated by spaces)"
				       ,QLineEdit::Normal,QString::null,&ok,this);
  if (ok && !tags.isEmpty())
    {
      QListViewItemIterator it1(songList);
      for(;it1.current();++it1)
	{
	  QSong *svi=(QSong*)it1.current();
	  if (svi->isSelected() && svi->isVisible()) 
	    songAddTag(svi,tags);
	}
      parseTags(tags);
    }
}

void SongSelectorLogic::selectionInsertInAlbum()
{
  static QString last = "";
  static int lastnr = 0;
  AlbumBox albumbox(NULL,NULL,TRUE);
  albumbox.album->insertItem(last);
  albumbox.number->setValue(++lastnr);
  if (albumbox.exec()==QDialog::Accepted)
    {
      QListViewItemIterator it(songList);
      last = albumbox.album->currentText();
      lastnr = albumbox.number->value();
      for(;it.current();++it)
	{
	  QSong * s = (QSong*)it.current();
	  if (s->isSelected() && s->isVisible())
	    insertSongInAlbum(s,last,albumbox.number->value());
	}
    }
}

void SongSelectorLogic::selectionEditInfo()
{
  QListViewItemIterator it1(songList);
  for(;it1.current();++it1)
    {
      QSong *svi=(QSong*)it1.current();
      if (svi->isSelected() && svi->isVisible()) 
	songEditInfo(svi);
    }
}

void SongSelectorLogic::selectionPlayIn3th()
{
  QListViewItemIterator it1(songList);
  for(;it1.current();++it1)
    {
      QSong *svi=(QSong*)it1.current();
      if (svi->isSelected() && svi->isVisible()) 
	{
	  processManager->startExtraSong(2,svi->songEssence());
	  return;
	}
    }
}

void SongSelectorLogic::selectionPlayIn4th()
{
  QListViewItemIterator it1(songList);
  for(;it1.current();++it1)
    {
      QSong *svi=(QSong*)it1.current();
      if (svi->isSelected() && svi->isVisible()) 
	{
	  processManager->startExtraSong(3,svi->songEssence());
	  return;
	}
    }
}

void SongSelectorLogic::selectionAddQueue()
{
  QListViewItemIterator it1(songList);
  queue->selectAll(false);
  QueuedSong *last = (QueuedSong*)queue->lastItem();
  if (last) last->setSelected(true);
  for(;it1.current();++it1)
    {
      QSong *svi=(QSong*)it1.current();
      if (svi->isSelected() && svi->isVisible()) 
	(new QueuedSong(svi,queue))->setSelected(true);
    }
  queueInsert(extraSongs->value());
}

void SongSelectorLogic::doAutoMix()
{
  // first obtain some interesting parameters..
  MergerDialog mergerDialog(this);
  int result = mergerDialog.exec();
  if (result==QDialog::Rejected) 
    return;
  // now we have a number of parameters...
  // time to construct them into a set of options
  char arguments[1024];
  sprintf(arguments,"--mix %d --slide %d --temposwitch %d %s %s %s %s %s",
	  mergerDialog.mixMeasures->value(),
	  mergerDialog.slideMeasures->value(),
	  mergerDialog.tempoMeasures->value(),
	  (mergerDialog.dumpRaws->isChecked() ? "--dumpraw" : ""),
	  (mergerDialog.verbose->isChecked() ? "--verbose" : ""),
	  (mergerDialog.normalizeSongs->isChecked() ? "--normalize" : ""),
	  (mergerDialog.volumeAccounting->isChecked() ? "--accountvolume" : ""),
	  (mergerDialog.fineScan->isChecked() ? "--rescantempo" : ""));
  // creating split arguments...
  char split_arguments[1024];
  bool should_split;
  should_split = 
    mergerDialog.splitBeforeMix->isChecked() || 
    mergerDialog.splitMiddleMix->isChecked() ||
    mergerDialog.splitAfterMix->isChecked() ||
    mergerDialog.splitAfterTempo->isChecked() ||
    mergerDialog.number->isChecked();
  if (should_split)
    sprintf(split_arguments, "merger --split %s %s %s %s %s\n",
	    (mergerDialog.splitBeforeMix->isChecked() ? "--beforemix" : ""),
	    (mergerDialog.splitMiddleMix->isChecked() ? "--middlemix" : ""),
	    (mergerDialog.splitAfterMix->isChecked() ? "--aftermix" : ""),
	    (mergerDialog.splitAfterTempo->isChecked() ? "--aftertempo" : ""),
	    (mergerDialog.number->isChecked() ? "--number" : ""));
  // now create the script...
  QString old="";
  QListViewItemIterator it(queue);
  FILE * out = openScriptFile(PROCESS_MIX);
  fprintf(out,RM BPMMIXED_NAME " automix.log\n");
  for(;it.current();++it)
    {
      QueuedSong *song = (QueuedSong*)it.current();
      Song * s = song->getSong();
      s->realize();
      QString n = s->storedin;
      if (old=="")
	fprintf(out,"merger %s \"%s\"\n",arguments,(const char*)n);
      else
	{
	  fprintf(out,"merger %s \"%s\" \"%s\"\n",arguments,(const char*)old,(const char*)n);
	  if (mergerDialog.playMixes->isChecked())
	    fprintf(out,BPLAY MIXDUMP_NAME" &\n");
	}
      old=n;
    }
  if (should_split)
    fprintf(out,split_arguments);
  if (mergerDialog.wavConvert->isChecked())
    fprintf(out,"for a in *"RAW_EXT"; do "RAW2WAV" \"$a\" \"`basename $a "RAW_EXT"`\".wav; "RM" \"$a\"; done\n");
  if (mergerDialog.compress->isChecked())
    fprintf(out,ENCODE);
  fclose(out);
}

void SongSelectorLogic::selectionSetMainSong()
{
  static bool dialog_shown=false;
  if (!dialog_shown)
    {
      dialog_shown=true;
      QMessageBox::information(this,"Removing the main song...","To remove the main song go to Program|Switch monitor to main");
    }
  QListViewItemIterator it1(songList);
  for(;it1.current();++it1)
    {
      QSong *svi=(QSong*)it1.current();
      if (svi->isSelected() && svi->isVisible()) 
	{
	  processManager->setMainSong(svi->songEssence());
	  return;
	}
    }
}

void SongSelectorLogic::selectionDelTags()
{
  bool ok;
  QString tags = QInputDialog::getText("Tags"
				       ,"Enter tags to delete (seperated by spaces)"
				       ,QLineEdit::Normal,QString::null,&ok,this);
  if (ok && !tags.isEmpty())
    {
      QListViewItemIterator it1(songList);
      for(;it1.current();++it1)
	{
	  QSong *svi=(QSong*)it1.current();
	  if (svi->isSelected() && svi->isVisible()) 
	    songDelTag(svi,tags);
	}
      parseTags(tags);
    }
}

void SongSelectorLogic::doMarkDups()
{
   int nr=0;
   QListViewItemIterator it1(songList);
   for(;it1.current();++it1)
     {
	QSong *svi=(QSong*)it1.current();
	if (svi->isSelected() && svi->isVisible())
	  {
	    QListViewItemIterator it2(songList);
	    QString txt=svi->md5sum();
	    printf("%d -- ",nr++);
	    bool found=false;
	    for(;it2.current();++it2)
	      {
		QSong *svi2=(QSong*)it2.current();
		if (svi!=svi2 
		    && txt==svi2->md5sum())
		  {
		    found=true;
		    svi2->setSelected(true);
		    break;
		  }
	      }
	    if (!found)
	      svi->setSelected(false);
	  }
     }
}

void SongSelectorLogic::doLegende()
{
  doAbout(2);
}

void SongSelectorLogic::doLicense()
{
  doAbout(1);
}

void SongSelectorLogic::doAbout(int pg)
{
  char tmp[500];
  AboutDialog about(NULL,NULL,1);
  sprintf(tmp,"BpmDj v%s",VERSION);
  about.versionLabel->setText(tmp);
  about.pages->setCurrentPage(pg);
  about.exec();
}

void SongSelectorLogic::doAbout()
{
  doAbout(0);
}

void SongSelectorLogic::searchLineEntered()
{
  doFilterChanged();
  QListViewItem * item = songList->firstChild();
  if (item) 
    item->setSelected(true);
  songList->setFocus();
}

void SongSelectorLogic::doFilterChanged()
{
  Config::color_range = view->isItemChecked(colorinrange_item);
  Config::color_played = view->isItemChecked(coloralreadyplayed_item);
  Config::color_authorplayed = view->isItemChecked(colorauthorplayed_item);
  Config::color_ondisk = view->isItemChecked(colorsongondisk_item);
  Config::color_cues = view->isItemChecked(colorcues_item);
  Config::color_dcolor = view->isItemChecked(colordcolor_item);
  Config::color_spectrum = view->isItemChecked(colorspectrum_item);
  Config::limit_ondisk = view->isItemChecked(onlyondisk_item);
  Config::limit_nonplayed = view->isItemChecked(notyetplayed_item); 
  Config::limit_uprange = view->isItemChecked(onlyuptemporange_item);
  Config::limit_downrange = view->isItemChecked(onlydowntemporange_item);
  Config::limit_indistance = view->isItemChecked(onlyindistance_item);
  Config::limit_authornonplayed = view->isItemChecked(onlynonplayedauthors_item);
  searchLine -> setText(searchLine->text().upper());
  updateItemList();
}

void SongSelectorLogic::doAutoFilterChanged()
{
  Config::open_mixer = autom->isItemChecked(auto_mixer_item);
  Config::ask_mix = autom->isItemChecked(auto_askmix_item);
  Config::auto_popqueue = autom->isItemChecked(auto_popq_item);
  // WVB -- misschien is het hier nuttig van onmiddelijk de nodige acties te nemen...
}

void SongSelectorLogic::quitButton()
{
   close();
}

extern const QString TAG_TRUE("Yes");
extern const QString TAG_FALSE("___");

void SongSelectorLogic::selectAllButTagged()
{
  QListViewItemIterator it(tagList);
  while(it.current())
    (it++).current()->setText(TAGS_NOT,TAG_TRUE);
}

void SongSelectorLogic::addTag(const QString tag)
{
  if (tagList->findItem(tag,TAGS_TEXT)) return;
  new QListViewItem(tagList,tag,TAG_FALSE,TAG_FALSE,TAG_FALSE);
}

void SongSelectorLogic::findallsimilarnames()
{
  QString text = QFileDialog::getExistingDirectory(NULL,this,NULL,"Specify directory to compare with database");
  // ask the name to start with
  if (!text.isEmpty())
    {
      SimilarScanner *scanner = new SimilarScanner(this);
      scanner->setRoot(text);
      scanner->exec();
    }
}

void SongSelectorLogic::findsimilarnames(const QString & text, const QString & fullname)
{
  assert(0);
}

void SongSelectorLogic::findsimilarnames()
{
  bool ok;
  QString text = QInputDialog::getText("Input name to compare with",
				       "label",QLineEdit::Normal,
				       "insert name",&ok,0,0);
  if (ok && !text.isEmpty())
    {
      dist_init();
      findsimilarnames(text,"");
      dist_done();
    }
}

void SongSelectorLogic::importSongs()
{
  ImportScanner scanner(this);
  scanner.scan(MusicDir);
  updateItemList();
  scanner.exec();
}

void SongSelectorLogic::selectionMenu()
{
  selection->exec(QCursor::pos());
  searchLine->setFocus();
}


void SongSelectorLogic::openQueueMenu()
{
  queuemenu->exec(QCursor::pos());
}

void SongSelectorLogic::findWrongSongNames()
{
  QString text = QFileDialog::getExistingDirectory(NULL,this,NULL,"Specify directory to look for songs with wrong name");
  if (!text.isEmpty())
    {
      if (text.right(1)=="/")
	text = text.left(text.length()-1);
      RenamerLogic *renamer = new RenamerLogic(this);
      renamer->scan(text,text);
      renamer->show();
    }
}

void SongSelectorLogic::findWrongIdxNames()
{
  RenamerLogic *renamer = new RenamerLogic(this);
  renamer->scan(IndexDir,IndexDir);
  renamer->show();
}

void SongSelectorLogic::queueFindAndRename(int p_old, int p_new)
{
  QListViewItemIterator it(queue);
  for(;it.current();++it)
    {
      QueuedSong *entry = (QueuedSong*)it.current();
      int pos = entry->getPos();
      if (pos == p_old)
	{
	  entry->setPos(p_new);
	  return;
	}
    }
}

void SongSelectorLogic::queueShiftUp()
{
  bool restart = true;
  while(restart)
    {
      restart=false;
      queue->sort();
      QListViewItemIterator it(queue);
      for(;it.current();++it)
	{
	  QueuedSong *entry = (QueuedSong*)it.current();
	  if (entry->isSelected())
	    {
	      int pos = entry->getPos();
	      if (pos==1)
		{
		  QMessageBox::warning(NULL,"Cannot shift upwards",
				       "Because you have selected the topmost song, I cannot shift the others songs upward.\n");
		  return;
		}
	      queueFindAndRename(pos-1,pos);
	      entry->setPos(pos-1);
	      entry->setSelected(false);
	      restart=true;
	    }
	}
    }
  queue->repaint();
}

void SongSelectorLogic::queueShiftDown()
{
  bool restart = true;
  while(restart)
    {
      restart=false;
      queue->sort();
      QueuedSong * tomove = NULL;
      QListViewItemIterator it(queue);
      for(;it.current();++it)
	{
	  QueuedSong *entry = (QueuedSong*)it.current();
	  if (entry->isSelected())
	    tomove = entry;
	}
      if (tomove)
	{
	  int pos = tomove->getPos();
	  if (pos==queue->childCount())
	    {
	      QMessageBox::warning(NULL,"Cannot shift downwards",
				   "Because you have selected the last song, I cannot shift the others songs downward.\n");
	      return;
	    }
	  queueFindAndRename(pos+1,pos);
	  tomove->setPos(pos+1);
	  tomove->setSelected(false);
	  restart=true;
	}
    }
  queue->repaint();
}

void SongSelectorLogic::queueDelete()
{
  bool restart = true;
  while(restart)
    {
      restart=false;
      QListViewItemIterator it(queue);
      for(;it.current();++it)
	{
	  QueuedSong *entry = (QueuedSong*)it.current();
	  if (entry->isSelected())
	    {
	      delete(entry);
	      restart = true;
	      break;
	    }
	}
    }
  queueOrder();
}

void SongSelectorLogic::queueAnker()
{
  QListViewItemIterator it(queue);
  for(;it.current();++it)
    {
      QueuedSong *entry = (QueuedSong*)it.current();
      if (entry->isSelected())
	entry->setAnkered(!entry->ankered());
    }
  queue->repaint();
}

void SongSelectorLogic::queueOrder()
{
  // nu lopen we er nog eens door om ze normaal te nummeren
  int cur = 1;
  for(QListViewItemIterator it(queue);it.current();++it)
    {
      QueuedSong * entry = (QueuedSong*)it.current();
      entry->setPos(cur++);
    }
  queue->repaint();
}

void SongSelectorLogic::queueInsert(int count)
{
  int cur = 1;
  if (count==0) return;
  // first we deselect the last element
  QListViewItem * last = queue->lastItem();
  if (last && last->isSelected())
    last->setSelected(false);
  // eerst lopen we er door om ze allemaal een veelvoud van count te geven
  QListViewItemIterator it(queue);
  for(;it.current();++it)
    {
      QueuedSong *entry = (QueuedSong*)it.current();
      entry->setPos(cur);
      cur+=count+1;
    }
  // dan lopen we er door om de extra fields te inserten
  cur = 1;
  QListViewItemIterator it2(queue);
  for(;it2.current();++it2)
    {
      QueuedSong *entry = (QueuedSong*)it2.current();
      if (entry->isSelected())
	{
	  cur = entry->getPos();
 	  QueuedSong * last = entry;
	  for(int i = 0 ; i < count; i++)
	    {
	      last = new QueuedSong(queue,last);
	      last->setPos(++cur);
	      last->mark=true;
	    }
	}
    }
  // nu kunnen we dus lekker al de ontbrekende songs invullen
  queueOrder();
  for(QListViewItemIterator it(queue);it.current();++it)
    {
      QueuedSong * entry = (QueuedSong*)it.current();
      entry->setSelected(entry->mark);
      entry->mark=false;
    }
  queueRandom();
}

void SongSelectorLogic::queueInsert()
{
  bool ok;
  int count = 5;
  count = QInputDialog::getInteger("Insert Songs...","How many songs should be inserted ?",5,1,50,1,&ok);
  if (!ok) return;
  queueInsert(count);
}

void SongSelectorLogic::queueSelectSong()
{
  queueRandom(true);
}

void SongSelectorLogic::queueRandom()
{
  queueRandom(false);
}

bool SongSelectorLogic::rejectQueueProposal(Song * song, int position)
{
  QListViewItemIterator it(queue);
  for(;it.current();++it)
    {
      QueuedSong * queuedsong = (QueuedSong*)it.current();
      Song * entry = queuedsong->getSong();
      if (!entry) continue;
      if (weighDuplicateAuthors->isChecked())
	{
	  if (entry->author == song->author)
	    {
	      int d = abs(queuedsong->getPos()-position);
	      if (d<=Config::authorDecay)
		return true;
	    }
	}
      if (entry->file == song->file)
	return true;
    }
  return false;
}


void SongSelectorLogic::filterProposedList(Song ** list, int &count, int position)
{
  int i = 0;
  while(i<count)
    {
      assert(list[i]);
      if (rejectQueueProposal(list[i], position))
	{
	  for(int j = i ; j < count - 1; j ++)
	    list[j]=list[j+1];
	  count--;
	}
      else
	i++;
    }
}

void SongSelectorLogic::queueCopySongs()
{
  QString target = QFileDialog::getExistingDirectory(NULL,this,NULL,"Specify directory to dump songs to");
  if (target.isEmpty())
    return;
  for(QListViewItemIterator it(queue); it.current(); ++it)
    {
      QueuedSong * qs = (QueuedSong*)it.current();
      Song* s = qs->getSong();
      if (s)
	vexecute(CP"\"./music/"+s->file+"\" "+target);
    }
}

void SongSelectorLogic::queueRandom(bool userChoice)
{
  SongMetriek * metriek = NULL;
  while(true)
    {
      QListViewItemIterator it(queue);
      // find before, element and after
      QueuedSong *before =  NULL;
      QueuedSong *after = NULL;
      QueuedSong *element = NULL;
      for(;it.current();++it)
	{
	  QueuedSong *entry = (QueuedSong*)it.current();
	  if (entry->isSelected())
	    {
	      if (!element)
		element = entry;
	    }
	  if (entry->ankered())
	    {
	      if (element)
		{
		  after = entry;
		  break;
		}
	      else
		{
		  before = entry;
		}
	    }
	}
      // check boundaries
      if (!element)
	break;
      element->setSelected(false);
      if (element->ankered())
	continue;
      if (!before)
	{
	  QMessageBox::warning(NULL,"Randomize",
			       "Cannot ranomize an element\nwithout an ankered element before");
	  continue;
	}
      if (!after)
	{
	  QMessageBox::warning(NULL,"Randomize",
			       "Cannot ranomize an element\nwithout an ankered element after");
	  continue;
	}
      // create a target song at that position
      Song *a = before->getSong();
      Song *c = after->getSong();
      float A = before->getPos();
      float C = after->getPos();
      float B = element->getPos();
      float D = (B-A)/(C-A);
      if (weighTempo->isChecked())
	metriek=&SongMetriek::ALL;
      else
	metriek=&SongMetriek::ALL_WITHOUT_TEMPO_WEIGHT;
      Point *b = a->percentToward(c, metriek, D);
      // find a song close to the target position
      int count;
      Song * * proposed = database->closestSongs(this, (Song*)b, &SongMetriek::ALL, 20, count);
      element -> setSong(NULL,-1);
      Song * choice = NULL;
      // now run through the proposed list and remove anything duplicate depending on the users preferences
      filterProposedList(proposed,count,element->getPos());
      // let the user choose or choose self
      if (count>0)
	{
	  if (userChoice)
	    {
	      QPopupMenu * songs = new QPopupMenu(this);
	      for(int i = 0 ; i < count ; i ++)
		songs -> insertItem ( proposed[i]->title+"["+proposed[i]->author+"]", i, i);
	      int result = songs->exec();
	      if (result>=0 && result < count)
		choice = proposed[result];
	    }
	  else
	    {
	      choice = proposed[0];
	    }
	  free(proposed);
	  if (choice)
	    element->setSong(choice,choice->distance((Song*)b,metriek));
	  else
	    element->setSong(choice,0);
	}
    }
  queueOrder();
}

void SongSelectorLogic::selectAlbumItem(QListViewItem* i)
{
  AlbumItem *album = (AlbumItem*)i;
  if(!album) return;
  if (album->song) return;
  songList->clearSelection();
  // traverse through all children and find them in the songlist...
  AlbumItem *child = (AlbumItem*)album->firstChild();
  bool alreadygiven = false;
  while(child)
    {
      QString file = child->text(ALBUM_FILE);
      QSong *toadd = (QSong*)songList->findItem(file,LIST_FILE);
      if (!toadd)
	{
	  if (!alreadygiven)
	    {
	      QMessageBox::warning(this,"Selection problem","Unable to select\nbecause it is not available in the song list.");
	      alreadygiven=true;
	    }
	}
      else new QueuedSong(toadd,queue);
      child = (AlbumItem*)child->nextSibling();
    }
}

void SongSelectorLogic::renameAlbumItem(QListViewItem* i)
{
  if (!i) return;
  AlbumItem *item = (AlbumItem*)i;
  if (!item->song) return;
  item->startRename(0);
}

void SongSelectorLogic::albumItemChanged(QListViewItem* i, int col)
{
  AlbumItem *item = (AlbumItem*)i;
  if (!item) return;
  Song *song = item->song;
  if (!song) return;
  song->realize();
  QString file = song->storedin;
  if (!file) return;
  Index index(file);
  AlbumField *album = index.find_album(i->parent()->text(0));
  if (!album) 
    {
      printf("Error: album entry %s that is unknown in index file\n",(const char*)i->text(1));
      return;
    }
  album -> nr = atoi(i->text(0));
  index . write_idx();
  item -> fixNr();
}

void SongSelectorLogic::changeTagList(QListViewItem* item, const QPoint & pos, int col)
{
  if (col < TAGS_OR || col > TAGS_NOT) return;
  if (!item) return;
  QString c = item -> text(col);
  item -> setText(col,c == TAG_TRUE ? TAG_FALSE : TAG_TRUE);
  doFilterChanged();
}

void SongSelectorLogic::deleteSongFromAlbum(AlbumItem * item)
{
  if (!item) return;
  Song *song = item->song;
  if (!song) return;
  song->realize();
  QString file = song->storedin;
  if (!file) return;
  AlbumItem * album = (AlbumItem*)(item->parent());
  if (!album) return;
  QString name = album->text(0);
  Index index(file);
  index . delete_album(name);
  index . write_idx();
  delete(item);
}

#define TABS_ALBUMS 1
#define TABS_QUEUE 3

bool SongSelectorLogic::eventFilter(QObject *o, QEvent *e)
{
  if ( e->type() == QEvent::KeyPress )
    {
      QKeyEvent *k=(QKeyEvent*)e;
      switch (tabs->currentPageIndex())
	{
	case TABS_ALBUMS:
	  if (k->key() == Qt::Key_Delete) 
	    {
	      deleteSongFromAlbum((AlbumItem*)(albumList->currentItem()));
	      return TRUE;
	    }
	case TABS_QUEUE:
	  if (k->key() == Qt::Key_Delete) 
	    {
	      queueDelete();
	      return TRUE;
	    }
	  if (k->key() == Qt::Key_Insert)
	    {
	      queueInsert();
	      return TRUE;
	    }
	}
    }
  return FALSE;
}

void SongSelectorLogic::keyPressEvent(QKeyEvent* e)
{
  if (e->key() == Qt::Key_Escape) return;
  SongSelector::keyPressEvent(e);
}

#define SPLIT_ONEFILE 0
#define SPLIT_ARTIST 1
#define SPLIT_TAGS 2

void SongSelectorLogic::compactIdxDirectory()
{
  // obtain information how to split all information
  int type = -1;
  CompactDialog compacter(NULL,NULL,TRUE);
  if (compacter.exec()==compacter.Rejected) return;
  if (compacter.onebigfile->isChecked()) type = SPLIT_ONEFILE;
  else if (compacter.artist->isChecked()) type = SPLIT_ARTIST;
  else if (compacter.tags->isChecked()) type = SPLIT_TAGS;
  else assert(0);
  // check existense of new_index...
  if (exists("./new_index"))
    {
      if (QMessageBox::question(this,"New Index Directory Exists","The new index directory ./new_index/ already exists.\nRemove ?",
				QMessageBox::Yes, QMessageBox::Abort) != QMessageBox::Yes) return;
      if (!execute("rm -fr ./new_index/"))
	{
	  QMessageBox::critical(this,"Delete Failed","Could not delete the directory ./new_index/",QMessageBox::Abort,0);
	  return;
	}
    }
  execute("mkdir ./new_index/");
  // fetch all songs
  int nr;
  Song * * all = database->getAllSongs(nr);
  assert(nr>=0);
  // create a tree containing the different filenames
  AvlTree<QString> * tree = new AvlTree<QString>();
  if (type == SPLIT_ONEFILE)
    tree->add(new QStringNode(compacter.filename->text()));
  else
    for (int i = 0 ; i < nr ; i ++)
      {
	Song * song = all[i];
	switch(type)
	  {
	  case SPLIT_ARTIST:
	    if (!tree->search(song->author))
	      tree->add(new QStringNode(song->author));
	    break;
	  case SPLIT_TAGS:
	    if (!tree->search(song->tags))
	      tree->add(new QStringNode(song->tags));
	    break;
	  }
      }
  // dump everything to the ./new_index/ directory..
  compacter.ok->setEnabled(false);
  compacter.cancel->setEnabled(false);
  compacter.buttons->setEnabled(false);
  compacter.show();
  QStringNode * filetocreate;
  int progress = 0;
  while ( (filetocreate = (QStringNode*)tree->top()) )
    {
      QString key =  filetocreate->content;
      tree->del(key);
      QString qfilename = QString("new_index/")+key+".bib";
      if (key.isEmpty()) 
	qfilename = QString("new_index/_empty_.bib");
      const char* filename = qfilename;
      compacter.fileinfo->setText(qfilename);
      FILE * target = fopen(filename,"wb");
      if (!target)
	{
	  QMessageBox::critical(this,QString("Compaction failed"),
				QString("The file\n") +QString(filename)+ QString("\ncould not be written !"),
				QMessageBox::Abort,0);
	  return;
	}
      app->processEvents();
      assert(target);
      for(int i = 0 ; i < nr ; i ++)
	{
	  Song * song = all[i];
	  bool handlethisone = false;
	  switch (type)
	    {
	    case SPLIT_ARTIST:
	      handlethisone = song->author == key;
	      break;
	    case SPLIT_TAGS:
	      handlethisone = song->tags == key;
	      break;
	    case SPLIT_ONEFILE:
	      handlethisone=true;
	    }
	  if (handlethisone)
	    {
	      QString indexfilename = song->storedin;
	      Index index(indexfilename);
	      if (index.can_be_stored_in_bib())
		{
		  index.write_bib_field(target);
		}
	      else
		{
		  QString withoutindexprefix = indexfilename.remove("./index");
		  QString withnewprefix = QString("./new_index")+withoutindexprefix;
		  QString targetdirectory = withnewprefix.left(withnewprefix.findRev("/"));
		  vexecute(MKDIR"\"%s\"",(const char*)targetdirectory);
		  index.write_idx((const char*)withnewprefix);
		}
	      if (nr>0 && (++progress)%(nr/100) == 0)
		{
		  compacter.progress->setProgress(progress,nr);
		  app->processEvents();
		}
	    }
	}
      fclose(target);
    }
  if (progress!=nr)
    {
      QMessageBox::critical(this,QString("Compaction failed"),
			    QString("From the ")+QString::number(nr)+" filesn\nonly "+QString::number(progress)+" have been written !",
			    QMessageBox::Abort,0);
      return;
    }
  if (!execute("rm -fr ./index/"))
    {
      QMessageBox::critical(this,"Delete Failed",
			    "Could not delete the directory ./index\n"
			    "Make sure to delete it MANUALLY and\n"
			    "rename the ./new_index directory to ./index",QMessageBox::Abort,0);
      return;
    }
  
  if (!execute(MV" ./new_index ./index"))
    {
      QMessageBox::critical(this,"Move failed",
			    "The ./new_index directory contains all your data.\n"
			    "However, somehow I could not rename it to ./index\n"
			    "Please do this manually.",QMessageBox::Abort,0);
      return;
    }
  // rereading. The index-reader will automatically update all existing Song* pointers..
  // wo we don't care about them
  compacter.prompt->setText("Rereading");
  app->processEvents();
  IndexReader * indexReader = new IndexReader(compacter.progress,compacter.fileinfo,database);
  delete indexReader;
  compacter.prompt->setText("Finishing");
  app->processEvents();
  findAllTags(); // necessary to update the album pointers and the view itself...
}
