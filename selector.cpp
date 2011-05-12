/****
 BpmDj v4.0: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.
****/
#ifndef __loaded__selector_cpp__
#define __loaded__selector_cpp__
using namespace std;
#line 1 "selector.c++"
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
#include <qtextedit.h>
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
#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlineedit.h>
#include <qframe.h>
#include <q3header.h>
#include <qlistview.h>
#include <qsplitter.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qfont.h>
#include <qframe.h>
#include <libgen.h>
#include "music-scanner.h"
#include "about.h"
#include "ui-album.h"
#include "qsong.h"
#include "version.h"
#include "song-metric.h"
#include "bpmdj.h"
#include "players-manager.h"
#include "cluster.h"
#include "queuedsong.h"
#include "historysong.h"
#include "ui-bpmmerge.h"
#include "edit-distance.h"
#include "index-reader.h"
#include "scripts.h"
#include "spectrum-type.h"
#include "qvectorview.h"
#include "tags.h"
#include "common.h"
#include "ui-freq-mapping.h"
#include "memory.h"
#include "statistics.h"
#include "ui-clustering.h"
#include "history.h"
#include "log-viewer.h"
#include <qaction.h>
#include "bpmdj-event.h"
#include "vector-iterator.h"
#include "song-iterator.h"
#include "listview-iterator.h"
#include "fragment-cache.h"
#include "index-reader.h"
#include "song-copier.h"

SongSelectorLogic * selector = NULL;

SongSelectorLogic::~SongSelectorLogic()
{
  QMutexLocker _ml(&lock);
  delete processManager;
  delete database;
}

SongSelectorLogic::SongSelectorLogic(QWidget * parent) :
  QDialog(parent,Qt::Window), lock(true)
{
  setupUi(this);
  QMutexLocker _ml(&lock);
  status = new QStatusBar(this);
  status -> setSizeGripEnabled(false);
  status -> setFont(QFont("Arial",10,QFont::Normal));
  status -> message("BpmDj v"VERSION" (c) Werner Van Belle 2000-2009");
  layout()->addWidget(status);
  QGridLayout* songListFrameLayout = new QGridLayout( songListFrame, 1, 1, 
					 0, 0, "songListFrameLayout"); 
  songList = new QVectorView( songListFrame, new QSong());
  songList->addColumn( tr( "Tempo" ) );
  songList->addColumn( tr( "Spectrum" ) );
  songList->addColumn( tr( "dColor" ) );
  songList->addColumn( tr( "Time" ) );
  songList->addColumn( tr( "Cues" ) );
  songList->addColumn( tr( "Title" ) );
  songList->addColumn( tr( "Author" ) );
  songList->addColumn( tr( "Version" ) );
  songList->addColumn( tr( "Tags" ) );
  songList->addColumn( tr( "OnDisk" ) );
  songList->addColumn( tr( "Index" ) );
  songList->addColumn( tr( "Md5" ) );
  songList->addColumn( tr( "Filename on disk" ) );
  songList->addColumn( tr( "Minimum" ) );
  songList->addColumn( tr( "Maximum" ) );
  songList->addColumn( tr( "Mean" ) );
  songList->addColumn( tr( "Power" ) );
  songList->addColumn( tr( "Echo" ) );
  songList->addColumn( tr( "Rhythm" ) );
  songList->addColumn( tr( "Composition" ) );
  songList->setShowSortIndicator( TRUE );
  songList->setSizePolicy( 
    QSizePolicy( (QSizePolicy::SizeType)5, 
		 (QSizePolicy::SizeType)5, 
		 0, 
		 0, 
		 songList->sizePolicy().hasHeightForWidth() ) );
  songList->setFrameShape( Q3ListView::Panel );
  songList->setResizePolicy( Q3ScrollView::Manual );
  songList->header()->moveSection(LIST_HISTOGRAM,2);
  songList->header()->moveSection(LIST_RHYTHM,2);
  songList->header()->moveSection(LIST_COMPOSITION,2);
  songListFrameLayout->addWidget( songList, 0, 0 );
  
  connect(songList, SIGNAL(doubleClicked(int)), 
	  this, SLOT( selectSong(int)));
  connect(songList, SIGNAL(ctrlReturnPressed(int)), 
	  this, SLOT( selectSong(int)));
  connect(songList, SIGNAL(returnPressed(int)), 
	  this, SLOT( selectionMenu()));
  connect(songList, SIGNAL(rightButtonPressed(int,const QPoint&,int) ), 
	  this, SLOT( selectionMenu()));
  connect(songList, SIGNAL(clicked(int)), this, SLOT( playFragment(int) ) );
  
  // link with the process manager
  processManager = new ProcessManager();
  analyzers = new AnalyzersManager();
  database = new DataBase();
  
  // initialize fields
  mainTicks=0;
  timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()), SLOT(timerTick()));
  timer->start(1000);
  albumList->installEventFilter(this);
  queue->installEventFilter(this);
  frequency_dialog = NULL;

  // create menu structure
  QMenuBar   * main = new QMenuBar(this);
  layout()->setMenuBar(main);
  Q3PopupMenu * help = new Q3PopupMenu(this);
  Q3PopupMenu * before = new Q3PopupMenu(this);
  Q3PopupMenu * file = new Q3PopupMenu(this);
  selection = new Q3PopupMenu(this);
  queuemenu = new Q3PopupMenu(this);
  Q3PopupMenu * view  = new Q3PopupMenu(this);
  Q3PopupMenu * autom = new Q3PopupMenu(this);
  
  // file menu
  file->insertItem("&Preferences",this,SLOT(doPreferences()));
  file->insertItem("&Clear Main",this,SLOT(switchMonitorToMain()));
  file->insertSeparator();
  file->insertItem("Inspect log files",this, SLOT(openLogDialog()));
  file->insertItem("Statistics",this, SLOT(openStatistics()));
  file->insertItem("&Quit",this,SLOT(quitButton()));
  
  // help menu
  help->insertItem("&About",this,SLOT(doAbout()));
  help->insertItem("Online &Manual",this,SLOT(doOnlineHelp()));
  help->insertItem("L&icense",this,SLOT(doLicense()));
  
  // before menu
  before->insertItem("&Import songs ("+extensionList()+")",
		     this, SLOT(importSongs()));
  before->insertItem("Check song availability",
		     this,SLOT(startExistenceCheck()));
  before->insertItem("Rename songs with wrong title...",
		     this,SLOT(startRenamer()));
  before->insertSeparator();
#ifdef INCOMPLETE_FEATURES
  // checks whether the songs on a disk can be found somewhere in the indices
  before->insertItem("Find similar song names in directory...",
		     this,SLOT(findallsimilarnames()));
#endif
  before->insertItem("Backup indices",this,SLOT(doBackup()));

  // view color menu
  view->insertSeparator();
  linkViewProp(Config::color_played, view,"Color already played titles");
  linkViewProp(Config::color_authorplayed, view,"Color already played authors");
  linkViewProp(Config::color_songs_based_on_history, 
	       view,"Color songs based on history");
  linkViewProp(Config::color_range, view,"Color songs within tempo range");
  linkViewProp(Config::color_ondisk, view,"Color songs not on disk");
  linkViewProp(Config::color_cues, view,"Color songs without cues");
  linkViewProp(Config::color_dcolor, view,"Color spectrum distance");
  linkViewProp(Config::color_spectrum, view,"Color PCA of spectrum");
  
  // view display part
  view->insertSeparator();
  view -> insertItem("Limit selection to &Tempo range",
		     this,SLOT(toggle_temporange()));
  
  linkViewProp(Config::limit_uprange, 
	       view,"&Up Tempo Range (0 < BPM < 0.06)");
  linkViewProp(Config::limit_downrange, 
	       view,"&Down Tempo Range (-0.06 < BPM < 0)");
  linkViewProp(Config::limit_nonplayed, 
	       view,"Limit selection to &Non-played songs");
  linkViewProp(Config::limit_ondisk, 
	       view,"Limit selection to &Files on disk");
  linkViewProp(Config::limit_indistance,
	       view,"Limit selection to &dColor range");
  linkViewProp(Config::limit_authornonplayed, 
	       view,"Limit selection to non played authors");
  
  // view dialog part
  view->insertSeparator();
  view->insertItem("Frequency shaping dialog",
		   this,SLOT(show_freq_shaping_dialog()));
  
  // auto menu
  linkAutoProp(Config::auto_popqueue ,autom,"Pop queue when monitor empty");
  linkAutoProp(Config::ask_mix       ,autom,"Ask 'how did the mix go'");
  linkAutoProp(Config::play_fragments ,autom,"Play fragments within selector");
  
  // selection menu
  selection->insertItem("&Add tag...",this,SLOT(selectionAddTags()));
  selection->insertItem("&Delete tag...",this,SLOT(selectionDelTags()));
  selection->insertItem("Set Author...",this,SLOT(selectionSetAuthor()));
  selection->insertItem("&Edit info...",this,SLOT(selectionEditInfo()));
  selection->insertItem("&Insert in album...",
			this,SLOT(selectionInsertInAlbum()));
  selection->insertItem("&Analyze",this,SLOT(queueAnalysis()));
  selection->insertItem("Pca Analysis Sound Color",this,SLOT(doSpectrumPca()));
  selection->insertItem("Cluster Analysis",this,SLOT(doClustering()));
  selection->insertSeparator();
  selection->insertItem("This is the main song, but don't play it",
			this,SLOT(selectionSetMainSong()));
#ifdef INCOMPLETE_FEATURES
  selection->insertItem("Avoid this song",this,SLOT(avoidSongs()));
  selection->insertItem("Clear Song Avoidance",this,SLOT(avoidNoSongs()));
  selection->insertItem("Play in 1st Extra Player",
			this,SLOT(selectionPlayIn3th()));
  selection->insertItem("Play in 2nd Extra Player",
			this,SLOT(selectionPlayIn4th()));
#endif
  selection->insertItem("Queue",this,SLOT(selectionAddQueue()));
  selection->insertSeparator();
  selection->insertSeparator();
  selection->insertItem("Select all songs without tag",
			this,SLOT(selectAllButTagged()));
  selection->insertItem("Select all songs",this,SLOT(selectAllTags()));

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
  queuemenu->insertItem("Copy songs to directory...",
			this,SLOT(queueCopySongs()));  
  
  // main menu
  main->insertItem("&Program",file);
  main->insertItem("&View",view);
  main->insertItem("&Auto",autom);
  main->insertItem("&Selection",selection);
  main->insertItem("&Queue",queuemenu);
  main->insertItem("&Library",before);
  main->insertItem("&Help",help);
  
  // create player views
  assert(playerLayout->layout());
  playersLeft->addWidget(new SongSelectorPlayView(playerLayout,
						  Config::players[0]));
  playersLeft->addWidget(new SongSelectorPlayView(playerLayout,
						  Config::players[1]));
  playersRight->addWidget(new SongSelectorPlayView(playerLayout,
						   Config::players[2]));
  playersRight->addWidget(new SongSelectorPlayView(playerLayout,
						   Config::players[3]));

  // create analyzer views
  assert(analLayout1->layout());
  assert(analLayout2->layout());
  for(int i = 0 ; i < 4 ; i ++)
    {
      SongSelectorAnalView * anal_view;
      anal_view = new SongSelectorAnalView(analLayout1,analyzers,
					   Config::analyzers[i]);
      analLayout1->layout()->addWidget(anal_view);
      anal_view = new SongSelectorAnalView(analLayout2,analyzers,
					   Config::analyzers[4+i]);
      analLayout2->layout()->addWidget(anal_view);
    }
  for(int i = 0 ; i < 8; i ++)
    connect(&Config::analyzers[i],SIGNAL(stateChanged()),
	    this,SLOT(toAnalyzeOrNot()));

  // check which tags were marked in the previous session
  QStringList ins = Config::get_tag_include().split(OneSpace,
						    QString::SkipEmptyParts);
  QStringList musts = Config::get_tag_mustbe().split(OneSpace,
						     QString::SkipEmptyParts);
  QStringList outs = Config::get_tag_exclude().split(OneSpace,
						     QString::SkipEmptyParts);
  Tags::add_tag(""); addTag("");
  Q3ListViewItem *empty_tag = tagList->findItem("",TAGS_TEXT);
  assert(empty_tag);
  empty_tag->setText(1,TAG_TRUE);
  {
    QStringListIterator i(ins);  
    while (i.hasNext()) 
      {
	QString tag = i.next();
	Tags::add_tag(tag); addTag(tag);
	tagList->findItem(tag,TAGS_TEXT)->setText(1,TAG_TRUE);
      }
  }

  {
    QStringListIterator i(musts);
    while (i.hasNext())
      {
	QString tag = i.next();
	Tags::add_tag(tag); addTag(tag);
	tagList->findItem(tag,TAGS_TEXT)->setText(2,TAG_TRUE);
      }
  }

  {
    QStringListIterator i(outs);
    while (i.hasNext()) 
      {
	QString tag = i.next();
	Tags::add_tag(tag); addTag(tag);
	tagList->findItem(tag,TAGS_TEXT)->setText(3,TAG_TRUE);
      }
  }
  History::init(".played.log",playedList);
  // copy the header information from the configuration file
  Q3Header *h = songList->header();
  copy_header(Config::get_header(),h);
  Config::set_header(h);
  // will fetch the tags we already know so that the database
  // can filter incoming songs immediately
  database->updateCache(this); 
};

extern IndexReader indexReader;
void SongSelectorLogic::start_reading_indices()
{
  QMutexLocker _ml(&lock);
  updateProcessView(false);
  indexReader.start(Config::get_file_count());
}

void SongSelectorLogic::stop_reading_indices(unsigned4 total_files)
{
  Config::set_file_count(total_files);
  if (!total_files)
    QMessageBox::message(NULL,
			 "You have no songs indexed in the database. You can\n"
			 "import songs from the music directory by selecting\n"
			 "'Import Songs' from the Library menu.");
  initialize_extras();
  doSpectrumPca(true);
  startExistenceCheck();
};

void SongSelectorLogic::linkViewProp(ConfigState & p, Q3PopupMenu *m, QString t)
{
  QMutexLocker _ml(&lock);
  p.setMenuText(t);
  p.addTo(m);
  connect(&p,SIGNAL(toggled()),this,SLOT(updateItemList()));
}

void SongSelectorLogic::linkAutoProp(ConfigState & p, Q3PopupMenu *m, QString t)
{
  QMutexLocker _ml(&lock);
  p.setMenuText(t);
  p.addTo(m);
}

void SongSelectorLogic::toggle_temporange()
{
  QMutexLocker _ml(&lock);
  bool newstate =  !(Config::limit_uprange.isOn() || 
		     Config::limit_downrange.isOn());
  Config::limit_uprange.setOn(newstate);
  Config::limit_downrange.setOn(newstate);
}

bool checkDocsAt(QString pos)
{
  QString p = pos+"/bpmdj.html";
  if (!exists(p)) return false;
  p = pos+"/index.html";
  return exists(p);
}

void SongSelectorLogic::doOnlineHelp()
{
  QMutexLocker _ml(&lock);
  // data location
  QString pos;
  QString pp = dirname(strdup(programname));
  if (!(checkDocsAt(pos = ".") ||
      checkDocsAt(pos = "documentation") ||
      checkDocsAt(pos = pp) ||
      checkDocsAt(pos = pp+"/documentation") ||
	checkDocsAt(pos = "/usr/share/doc/bpmdj-"VERSION)))
    pos = "http://bpmdj.yellowcouch.org/";

  // code location
  QString bin;
  if (exists(bin="/usr/bin/firefox") ||
      exists(bin="/usr/bin/mozilla") ||
      exists(bin="/usr/bin/konqueror") )
    spawn(bin+" "+pos+"/index.html","Documentation");
  else
    QMessageBox::warning(this,"Online",
			 "Please browse to http://bpmdj.yellowcouch.org/");
}

void SongSelectorLogic::doBackup()
{
  QMutexLocker _ml(&lock);
  time_t t_in=time(&t_in);
  struct tm * t = NULL;
  t = localtime(&t_in);
  vexecute("tar -cvzf bpmdj-index-backup-%d-%d-%d-%d-%d.tgz index",
	   t->tm_mday,t->tm_mon,1900+t->tm_year,t->tm_hour,t->tm_min);
}

void SongSelectorLogic::startExistenceCheck()
{ 
  QMutexLocker _ml(&lock);
  if (database)
    existenceScanner.start(new vector<Song*>(database->getAllSongs()));
}

void SongSelectorLogic::initialize_extras()
{ 
  QMutexLocker _ml(&lock);
  // first parse all the tags...
  parse_tags();
  bool ok = false;
  Q3ListViewItem * e = NULL;
  Q3ListViewItemIterator it(tagList);
  while(it.current() && !ok)
    {
      Q3ListViewItem *c = it.current();
      if (c->text(TAGS_TEXT)==EMPTY)
	e = c;
      if (c->text(TAGS_OR)==TAG_TRUE)
	ok=true;
      it++;
    }
  if (!ok && e)
    e->setText(TAGS_OR,TAG_TRUE);
  
  // update statistics
  const vector<Song*> &songs = database->getAllSongs();
  {
    constVectorIterator<Song*> song(songs);
ITERATE_OVER(song)

      statistics_first_pass(song.val());
    }
  }
  statistics_prepare_second_pass();
  {
    constVectorIterator<Song*> song(songs);
ITERATE_OVER(song)

      statistics_second_pass(song.val());
    }
  }
  statistics_stop_second_pass();
  
  // update album list
  {
    constVectorIterator<Song*> song(songs);
ITERATE_OVER(song)

      AlbumField ** albums = song.val() -> get_albums();
      while(*albums)
      {
	AlbumField * album = *albums++;
	AlbumItem  * ai = (AlbumItem*)albumList->findItem(album->name,0);
	if (!ai) ai = new AlbumItem(album->name,albumList);
	new AlbumItem(album->nr,song.val(),ai);
      }
    }
  }

  taglist2config(tagList);
  
  // mark all the played songs
  History::mark_all_played_songs(database);
  updateItemList();
}

void SongSelectorLogic::parse_tags()
{
  QMutexLocker _ml(&lock);
  if (!Tags::new_tags) return;
  vectorIterator<QString> tag(Tags::tag_names);
ITERATE_OVER(tag)

    addTag(tag.val());
  }
  Tags::new_tags=false;
}

void SongSelectorLogic::acceptNewSong(Song* song)
{
  QMutexLocker _ml(&lock);
  database->add(song);
}

static bool alreadygavefilterwarning = false;
void SongSelectorLogic::reread_and_repaint(Song* song)
{
  QMutexLocker _ml(&lock);
  assert(song);
  song->reread();
  int a,b;
  songList->get_visible_range(a,b);
  while(a<=b)
    {
      Song * s = QSong::songEssence(a);
      if (s==song)
	songList->repaintItem(a);
      a++;
    }
}

void SongSelectorLogic::updateItemList()
{
  QMutexLocker _ml(&lock);
  parse_tags();
  // if no song is playing...
  // backup & untoggle certain properties..
  bool backup_uprange = Config::limit_uprange;
  bool backup_downrange = Config::limit_downrange;
  bool backup_distancerange = Config::limit_indistance;
  if (!::main_song && !::monitor_song)
    {
      Config::limit_uprange.set(false);
      Config::limit_downrange.set(false);
      Config::limit_indistance.set(false);
    }
  // update selection
  Song * main = ::main_song;
  int itemcount = database->getSelection(this,songList,Config::get_max_songs());
  songList->repaint();
  countLcd->display(itemcount);
  // nothing selected ?
  if (itemcount==0 && !alreadygavefilterwarning && Config::get_file_count())
    {
      alreadygavefilterwarning = true;
      QMessageBox::warning(this,"Empty Selection",
			   "With the current selection of filters,\n"
			   "there are no songs to be displayed.\n"
			   "If you want to modify this, go to the View menu.");
    }
  // restore
  if (!::main_song && !::monitor_song)
    {
      Config::limit_uprange.set(backup_uprange);
      Config::limit_downrange.set(backup_downrange);
      Config::limit_indistance.set(backup_distancerange);
    }
  
  // now update the history view
  historyList->clear();
  if (main)
    {
      Index main_index(main->get_storedin());

      HistoryField ** after = main_index . get_prev_songs();
      while(after && *after)
	{
	  QString file = (*after)->file;
	  QString count = "-"+QString::number((*after)->count);
	  QString comment = (*after)->comment;
	  Song * s = database->file_to_song(file);
	  if (s)
	    {
	      new HistorySong(s,count,comment,historyList);
	      s->get_distance_to_main();
	    }
	  after++;
	}
      HistoryField ** before = main_index . get_next_songs();
      while (before && *before)
	{
	  QString file = (*before)->file;
	  QString count = "+"+QString::number((*before)->count);
	  QString comment = (*before)->comment;
	  
	  Song * s = database->file_to_song(file);
	  if (s)
	    {
	      s->get_distance_to_main();
	      new HistorySong(s,count,comment,historyList);
	    }
	  before++;
	}
    }
}

/**
 * This version of the item list updater is used from within the index scanner
 * It will not sort the data it retrieves from the database and will also
 * not report on the lack of exiting songs at the moment. The history view is
 * also not updated here.
 */
void SongSelectorLogic::step_reading_indices(vector<Song*> *new_songs)
{
  QMutexLocker _ml(&lock);
  //  parse_tags();
  // if no song is playing...
  // backup & untoggle certain properties..
  bool backup_uprange = Config::limit_uprange;
  bool backup_downrange = Config::limit_downrange;
  bool backup_distancerange = Config::limit_indistance;
  if (!::main_song && !::monitor_song)
    {
      Config::limit_uprange.set(false);
      Config::limit_downrange.set(false);
      Config::limit_indistance.set(false);
    }
  // update selection
  database->addNewSongs(this,songList,new_songs);
  songList->repaint();
  countLcd->display(QSong::get_song_count());
  // restore
  if (!::main_song && !::monitor_song)
    {
      Config::limit_uprange.set(backup_uprange);
      Config::limit_downrange.set(backup_downrange);
      Config::limit_indistance.set(backup_distancerange);
    }
}

void SongSelectorLogic::updateColors()
{
  QMutexLocker _ml(&lock);
  if (mainTicks<Config::get_yellowTime())
    setColor(Config::get_color_green_time());
  else if (mainTicks<Config::get_orangeTime())
    setColor(Config::get_color_yellow_time());
  else if (mainTicks<Config::get_redTime())
    setColor(Config::get_color_orange_time());
  else
    setColor(Config::get_color_red_time());
}

void SongSelectorLogic::timerTick()
{
  /**
   * The timer thread will periodically call this function. Since it is the 
   * same thread it will be able to continue below. We don' want that because 
   * then we might have race conditions.
   */
  if (!lock.tryLock()) return;
  QMutexLocker _ml(&lock);
  lock.unlock();
  
  mainLCD->display(++mainTicks);
  if (mainTicks==1 
      || mainTicks==Config::get_yellowTime()
      || mainTicks==Config::get_orangeTime()
      || mainTicks==Config::get_redTime())
    updateColors();
  poll_death();
  SongProcess::checkSignals();
  
  /**
   * We can now check whether any button has been check to start an analyzer. 
   * We do this after the signal check because this function will also redraw 
   * the colors for the analyzer and will ensure an immediate color change when
   * the text is updated in the signal checking round.
   */
  toAnalyzeOrNot();
  
  // auto pop queue
  // we can only pop every 10 seconds and if the main player
  // reports an error we turn of auto-popping...
  static int pop_ticks = 0;
  pop_ticks++;
  if (Config::auto_popqueue)
    if (!::monitor_song && pop_ticks > 10)
      {
	Q3ListViewItemIterator it(queue);
	QueuedSong * s = (QueuedSong*)it.current();
	if (s)
	  {
	    if (s->getSong())
	      {
		pop_ticks = 0;
		if (!processManager->startSong(s->getSong()))
		  Config::auto_popqueue.setOn(false);
	      }
	    delete(s);
	    queueOrder();
	  }
      }
}

void SongSelectorLogic::setColor(QColor color)
{
  QMutexLocker _ml(&lock);
  if (Config::get_color_main_window())
    {
      setPaletteBackgroundColor(color);
      countLcd->setBackgroundColor(color);
      mainLCD->setBackgroundColor(color);
    }
  else
    {
      countLcd->setBackgroundColor(color);
      mainLCD->setBackgroundColor(color);
    }
}

void SongSelectorLogic::setPlayerColor(QLabel *player ,QColor color)
{
  QMutexLocker _ml(&lock);
  int r,g,b;
  color.getRgb(&r,&g,&b);
  if (r<128 && g < 128 && b < 128) 
    player->setPaletteBackgroundColor(QColor(255-r,255-g,255-b));
  else 
    player->setPaletteBackgroundColor(color);
}

void SongSelectorLogic::resetCounter()
{
  QMutexLocker _ml(&lock);
  mainTicks=-1;
}

void SongSelectorLogic::switchMonitorToMain()
{
  QMutexLocker _ml(&lock);
  // forced switch
  processManager->switchMonitorToMain();
}

void SongSelectorLogic::updateProcessView(bool main_changed)
{
  QMutexLocker _ml(&lock);
  // update item list
  if (main_changed) 
    updateItemList();
  // update the frequency picture
  updateFrequencyMap();
}

void SongSelectorLogic::show_freq_shaping_dialog()
{
  QMutexLocker _ml(&lock);
  if (!frequency_dialog)
    {
      frequency_dialog=new QDialog();
      frequency_content.setupUi(frequency_dialog);
    }
  if (frequency_dialog->isVisible())
    frequency_dialog->hide();
  else
    frequency_dialog->show();
}

void SongSelectorLogic::updateFrequencyMap()
{
  QMutexLocker _ml(&lock);
  if (!frequency_dialog || !frequency_dialog->isVisible()) return;
  QPixmap *pm = new QPixmap(spectrum_size,200);
  QPainter p;
  p.begin(pm);
  QRect r(0,0,spectrum_size,200);
  p.fillRect(r,Qt::white);

  Song * song_a = ::main_song;
  Song * song_b = ::monitor_song;
  
  spectrum_type *a = song_a ? song_a->get_spectrum() : no_spectrum;
  spectrum_type *b = song_b ? song_b->get_spectrum() : no_spectrum;
  spectrum_freq min_freq = 1000;
  spectrum_freq max_freq = -1000;

  for (int j = 0; j < spectrum_size ; j++)
    {
      if (a!=no_spectrum && b!=no_spectrum)
	{
	  float4 energy_fix = b->band(j)-a->band(j);
	  if (energy_fix<min_freq) min_freq = energy_fix;
	  if (energy_fix>max_freq) max_freq = energy_fix;
	}
      else
	{
	  p.setPen(QColor(127,127,127));
	  p.drawLine(j,0,j,199);
	}
    }
  
  if (max_freq!=-1000)
    {
      float4 dbf = 199.0/(max_freq-min_freq);  //pixels per dB
      float4 y = 199.0;
      p.setPen(Qt::green);
      do
	{
	  p.drawLine(0,(int)y,spectrum_size-1,(int)y);
	  y-=3*dbf;
	}
      while(y>=0);
      p.setPen(Qt::black);
      for (int j = 0; j < spectrum_size ; j++)
	{
	  if (a!=no_spectrum && b!=no_spectrum)
	    {
	      float4 energy_fix = b->band(j)-a->band(j);
	      energy_fix -= min_freq;
	      p.drawPoint(j,(int)(199.0-dbf*energy_fix));
	    }
	}
    }
  p.end();
  frequency_content.frequency_map->setPixmap(*pm);
}

int songFileCompare(const void * a, const void* b)
{
  Song **pa=(Song**)a;
  Song **pb=(Song**)b;
  Song *A=*pa;
  Song *B=*pb;
  return strcmp(A->get_file(),B->get_file());
}

void SongSelectorLogic::doSpectrumPca(bool fulldatabase, 
				      bool update_process_view)
{
  QMutexLocker _ml(&lock);
  vector<Song*> *all;
  if (fulldatabase)
    all=new vector<Song*>(database->getAllSongs());
  else
    { 
      all = new vector<Song*>;
      selectedSongIterator svic;
ITERATE_OVER(svic)

	if (svic.val()->get_spectrum()!=no_spectrum) 
	  all->push_back(svic.val());
      };
    }
  assert(all);
  spectrumPca.pcaThis(all);
}

void SongSelectorLogic::doClustering()
{
  QMutexLocker _ml(&lock);
  // 0. Clear all existing colors
  QColor white(0,0,0);
  const vector<Song*> &all = database->getAllSongs();
  constVectorIterator<Song*> i(all);
ITERATE_OVER(i)

    i.val()->set_color(white);
    i.val()->set_spectrum_string(EMPTY);
  }
  // 1. add all information in 1 cluster
  clusterer.reset();
  selectedSongIterator svi;
ITERATE_OVER(svi)

    if (svi.val()->has_all_cluster_fields()) 
      clusterer.addPoint(svi.val()); 
  }
  // 2. Get the information from the user
  QDialog dialog(this);
  Ui::ClusterDialog dialogcontent;
  dialogcontent.setupUi(&dialog);
  if (dialog.exec()==Rejected) return;
  SongMetriek metriek=*dialogcontent.metrics;
  
  // 3. Start the analysis
  clusterer.agglomerate(metriek);
}

void SongSelectorLogic::exportPlayList()
{
  QMutexLocker _ml(&lock);
  // count nr of files to get 
  int selectionCount= queue->childCount();
  int next = 0;
  // allocate enough space to store the songs 
  Song * * toFetch = bpmdj_allocate(selectionCount,Song*);
  // read all the songs
  listViewIterator<QueuedSong> svi(queue);
ITERATE_OVER(svi)

    toFetch[next++]=svi.val()->getSong();
  }
  // write script to disk 
  char* homedir = getenv("HOME");
  char scriptname[1024];
  sprintf(scriptname,"%s/.xmms/xmms.m3u",homedir);
  char * current_directory=getcwd(NULL,0);
  FILE* script=fopen(scriptname,"wb");
  if (!script) 
    {
      QMessageBox::message("Cannot open ~/.xmms/xmms.m3u",
			   "Cannot open ~/.xmms/xmms.m3u.\n"
			   "Either the files is write protected\n"
			   "or the xmms directory doesn't exist");
      return;
    }
  int i = 0;
  while(i<next)
    {
      Song* song=toFetch[i++];
      if (song)
	{
	  const char* filename=song->get_file();
	  fprintf(script,"%s/./music/%s\n",current_directory,filename);
	}
    }
  fclose(script);
  bpmdj_deallocate(current_directory);
}

/**
 * This method queues all the selected songs in the analysis queue
 * If no analyzer is enabled we suggest the possibility to enable
 * one.
 */
void SongSelectorLogic::queueAnalysis()
{
  QMutexLocker _ml(&lock);
  selectedSongIterator svi;
ITERATE_OVER(svi)
new QueuedAnalSong(anal_queue,svi.val());};
  bool there_is_one = false;
  for(int i = 0 ; i < 8 && !there_is_one ; i ++)
    there_is_one |= Config::analyzers[i].isEnabledOk();
  if (!there_is_one)
    QMessageBox::warning(NULL,"Analyzer needed",
         "To analyze songs you need to enabled at least one analyzer. "
	 "Please check one of the checkboxes (#1 .. #8) in the analysis pane");
}

void SongSelectorLogic::toAnalyzeOrNot()
{
  QMutexLocker _ml(&lock);
  for(int i = 0 ; i < 8 ; i ++)
    if (Config::analyzers[i].is_free())
      startAnotherAnalyzer(NULL,i);
}

/**
 * This function is called by the analyzer manager when a slot comes free
 * either due to a timeout or due to a descent stop. In any case we need
 * to start a new one here.
 */
void SongSelectorLogic::startAnotherAnalyzer(Song * finished_analyzing, 
					     int on_slot)
{
  QMutexLocker _ml(&lock);
  // reread the song which has finished
  QString analTag(QString("NoAnalysis@#")+QString::number(on_slot));
  if (finished_analyzing)
    {
      reread_and_repaint(finished_analyzing);
      if (finished_analyzing->needs_analysis())
	{
	  // if there are still missing properties then we mark it
	  // as non analyzable at the given slot and queue the song
	  songAddTag(finished_analyzing,analTag);
	  new QueuedAnalSong(anal_queue,finished_analyzing);
	}
    }
  
  // update the progress bar
  float4 current_speed = 0.0;
  for(int i = 0 ; i < 8 ; i++)
    if (Config::analyzers[i].isEnabledOk())
      current_speed += Config::analyzers[i].songs_per_second();
  unsigned8 days = 0, hours = 0, minutes = 0, seconds = 0;
  if (current_speed != 0)
    {
      unsigned8 expected_time = (unsigned8)((float4)anal_queue->childCount()
					    /current_speed);
      days =  expected_time / (24L*60L*60L);
      expected_time -= days * (24L*60L*60L);
      hours = expected_time / (60L*60L);
      expected_time -= hours * (60L*60L);
      minutes = expected_time / 60L;
      seconds = expected_time - minutes * 60L;
    }
  eta_days->display((int)days);
  eta_hours->display((int)hours);
  eta_minutes->display((int)minutes);
  eta_seconds->display((int)seconds);
  // is this slot still enabled ?
  if (!Config::analyzers[on_slot].isEnabledOk()) return;
  // is there a next that can be send to this slot ?
  tag_type tag = Tags::find_tag_create_if_necessary(analTag);
  QueuedAnalSong * next = NULL;
  Q3ListViewItemIterator it(anal_queue);
  while(it.current())
    {
      QueuedAnalSong * c = (QueuedAnalSong*)it.current();
      if (!c->getSong()->contains_tag(tag))
	{
	  next = c;
	  break;
	};
      it++;
    }
  if (!next) return;
  // create the command line options
  SongSlot* slot=&Config::analyzers[on_slot];
  QString command = next->getCommand(*slot);
  QString description = next->getDescription(*slot);
  // start it
  analyzers->start(slot, next->getSong(), command, description);
  // delete it
  delete(next);
}

void SongSelectorLogic::selectSong(int i)
{
  QMutexLocker _ml(&lock);
  Song * song = QSong::songEssence(i);
  if (!song) return;
  if (Config::players[0].isEmpty() ||
      Config::players[1].isEmpty())
    {
      QMessageBox::message(NULL,
	   "You did not set the mandatory player commands.\n"
	   "Please fix these in the preferences box","Very interesting");
      doPreferences();
    }
  processManager->startSong(song);
}

void SongSelectorLogic::playQueueSong(Q3ListViewItem *song)
{
  QMutexLocker _ml(&lock);
  if (!song) return;
  QueuedSong * qs = (QueuedSong*)song;
  Song * s = qs->getSong();
  if (!s) return;
  processManager->startSong(s);
}

void SongSelectorLogic::playHistorySong(Q3ListViewItem *song)
{
  QMutexLocker _ml(&lock);
  if (!song) return;
  HistorySong * hs = (HistorySong*)song;
  Song * s = hs->getSong();
  if (!s) return;
  processManager->startSong(s);
}

void SongSelectorLogic::savePlayHistory()
{ 
  QMutexLocker _ml(&lock);
 History::save_history();
}

void SongSelectorLogic::clearPlayHistory()
{
  QMutexLocker _ml(&lock);
  History::clear_history(database);
}

void SongSelectorLogic::doPreferences()
{
  QMutexLocker _ml(&lock);
  if (Config::open_ui())
    {
      updateItemList();
      unsetPalette();
      updateColors();
    }
}

void SongSelectorLogic::songAddTag(Song *song, const QString & tag)
{
  QMutexLocker _ml(&lock);
  QString index = song->get_storedin();
  // read the index file
  Index * idx = new Index(index);
  // modify taglines
  QString newtags = Tags::full_string(song->get_tags())+" "+tag;
  idx->set_tags(strdup(newtags));
  // write the index file
  idx->write_idx();
  // free the bastard
  delete idx;
  // now update the local song data
  reread_and_repaint(song);
}

void SongSelectorLogic::songSetAuthor(Song *song, const QString & tag)
{
  QMutexLocker _ml(&lock);
  QString index = song->get_storedin();
  // read the index file
  Index * idx = new Index(index);
  idx->set_author(strdup(tag));
  idx->write_idx();
  delete idx;
  reread_and_repaint(song);
}

void SongSelectorLogic::insertSongInAlbum(Song * song, const QString & album, 
					  int nr)
{
  QMutexLocker _ml(&lock);
  QString index = song->get_storedin();
  // read the index file
  Index * idx = new Index(index);
  if (!idx -> find_album(album))
    {
      idx -> add_album(new AlbumField(nr,album));
      idx -> write_idx();
      // update album view
      AlbumItem * ai = (AlbumItem*)albumList->findItem(album,0);
      if (!ai) ai = new AlbumItem(album,albumList);
      new AlbumItem(nr,song,ai);
    }
  delete idx;
}

void SongSelectorLogic::songEditInfo(Song *song)
{
  QMutexLocker _ml(&lock);
  QString index = song->get_storedin();
  Index * idx = new Index(index);
  idx->executeInfoDialog();
  if (idx->changed())
    {
      idx->write_idx();
      delete idx;
      reread_and_repaint(song);
      return;
    }
  delete idx;
}

void SongSelectorLogic::songDelTag(Song * song, const QString & tag)
{
  QMutexLocker _ml(&lock);
  char* newtags;
  // read the index file
  Index * index = new Index(song->get_storedin());
  newtags=strdup(index->get_tags());
  // modify tag lines
  QString new_tags = index->get_tags();
  new_tags.replace(tag,OneSpace);
  index->set_tags(new_tags);
  // write the index file
  index->write_idx();
  // free the bastard
  delete index;
  // now update the local song data
  reread_and_repaint(song);
}

void SongSelectorLogic::selectionAddTags()
{
  QMutexLocker _ml(&lock);
  bool ok;
  QString tags = QInputDialog::getText("Tags",
		       "Enter tags to add (separated by spaces)"
		       ,QLineEdit::Normal,QString::null,&ok,this);
  if (ok && !tags.isEmpty())
    {
      selectedSongIterator svi;
ITERATE_OVER(svi)
songAddTag(svi.val(),tags);};
    }
}

void SongSelectorLogic::selectionSetAuthor()
{
  QMutexLocker _ml(&lock);
  bool ok;
  QString text = QInputDialog::getText("Set Author...", 
				       "Please enter the author name to be", 
				       QLineEdit::Normal,
				       QString::null, &ok, this );
  if ( ok ) 
    {
      selectedSongIterator svi;
ITERATE_OVER(svi)
songSetAuthor(svi.val(),text);};
    }
}

void SongSelectorLogic::selectionInsertInAlbum()
{
  QMutexLocker _ml(&lock);
  static QString last = "";
  static int lastnr = 0;
  QDialog dialog;
  Ui::AlbumBox albumbox;
  albumbox.setupUi(&dialog);
  albumbox.album->insertItem(last);
  albumbox.number->setValue(++lastnr);
  if (dialog.exec()==QDialog::Accepted)
    {
      last = albumbox.album->currentText();
      lastnr = albumbox.number->value();
      selectedSongIterator s;
ITERATE_OVER(s)

	insertSongInAlbum(s.val(),last,albumbox.number->value());};
    }
}

void SongSelectorLogic::selectionEditInfo()
{
  QMutexLocker _ml(&lock);
  selectedSongIterator svi;
ITERATE_OVER(svi)
songEditInfo(svi.val());};
}

void SongSelectorLogic::selectionPlayIn3th()
{
#ifdef INCOMPLETE_FEATURES
  QMutexLocker _ml(&lock);
  selectedSongIterator svi;
ITERATE_OVER(svi)

    processManager->startExtraSong(2,svi.val());
    return;
  }
#endif
}

void SongSelectorLogic::selectionPlayIn4th()
{
#ifdef INCOMPLETE_FEATURES
  QMutexLocker _ml(&lock);
  selectedSongIterator svi;
ITERATE_OVER(svi)

    processManager->startExtraSong(3,svi.val());
    return;}
#endif
}

void SongSelectorLogic::selectionAddQueue()
{
  QMutexLocker _ml(&lock);
  queue->selectAll(false);
  QueuedSong *last = (QueuedSong*)queue->lastItem();
  if (last) last->setSelected(true);
  selectedSongIterator svi;
ITERATE_OVER(svi)

    (new QueuedSong(svi.val(),queue))->setSelected(true);
  }
  queueInsert(extraSongs->value());
}

void SongSelectorLogic::doAutoMix()
{
  QMutexLocker _ml(&lock);
  // first obtain some interesting parameters..
  QDialog container(this);
  Ui::MergerDialog mergerDialog;
  mergerDialog.setupUi(&container);
  int result = container.exec();
  if (result==QDialog::Rejected) return;
  // now we have a number of parameters...
  // time to construct them into a set of options
  char arguments[1024];
  sprintf(arguments,"--mix %d --slide %d --temposwitch "
	  "%d %s %s %s %s %s %s --startpct %d --stoppct %d",
	  mergerDialog.mixMeasures->value(),
	  mergerDialog.slideMeasures->value(),
	  mergerDialog.tempoMeasures->value(),
	  (mergerDialog.verbose->isChecked() ? "--verbose" : ""),
	  (mergerDialog.normalizeSongs->isChecked() ? "--normalize" : ""),
	  (mergerDialog.volumeAccounting->isChecked() ? "--accountvolume" : ""),
	  (mergerDialog.fineScan->isChecked() ? "--rescantempo" : ""),
	  ((mergerDialog.startAtCue->isChecked() 
	    || mergerDialog.beforeCue->isChecked()) ? "--startcue" : ""),
	  (mergerDialog.beforeCue->isChecked() ? "--beforecue" : ""),
	  mergerDialog.headPercent->value(),
	  mergerDialog.tailPercent->value());
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
    sprintf(split_arguments, "bpmmerge --split %s %s %s %s %s\n",
	    (mergerDialog.splitBeforeMix->isChecked() ? "--beforemix" : ""),
	    (mergerDialog.splitMiddleMix->isChecked() ? "--middlemix" : ""),
	    (mergerDialog.splitAfterMix->isChecked() ? "--aftermix" : ""),
	    (mergerDialog.splitAfterTempo->isChecked() ? "--aftertempo" : ""),
	    (mergerDialog.number->isChecked() ? "--number" : ""));
  // now create the script...
  QString old="";
  FILE * out = openScriptFile(PROCESS_MIX);
  fprintf(out,"rm -- " BPMMIXED_NAME " automix.log\n");
  
  listViewIterator<QueuedSong> song(queue);
ITERATE_OVER(song)

    Song * s = song.val()->getSong();
    QString n = s->get_storedin();
    if (old=="")
    {
      char * a = escape(n);
      fprintf(out,"bpmmerge %s %s\n",arguments,a);
      bpmdj_deallocate(a);
    }
    else
    {
      char * a = escape(old);
      char * b = escape(n);
      fprintf(out,"bpmmerge %s %s %s\n",arguments,a,b);
      bpmdj_deallocate(a);
      bpmdj_deallocate(b);
    }
    old=n;
  }
  if (should_split)
    fprintf(out,split_arguments);
  if (mergerDialog.wavConvert->isChecked())
    fprintf(out,"for a in *"RAW_EXT"; do "RAW2WAV" \"$a\" \"`basename $a "
	    RAW_EXT"`\".wav; rm -- \"$a\"; done\n");
  fclose(out);
}

void SongSelectorLogic::selectionSetMainSong()
{
  QMutexLocker _ml(&lock);
  static bool dialog_shown=false;
  if (!dialog_shown)
    {
      dialog_shown=true;
      QMessageBox::information(this,"Removing the main song...",
			       "To remove the main song go to\n"
			       "Program|Clear Main");
    }
  selectedSongIterator svi;
ITERATE_OVER(svi)

    processManager->setMainSong(svi.val());
    return;
  }
}

void SongSelectorLogic::avoidSongs()
{
  QMutexLocker _ml(&lock);
  selectedSongIterator svi;
ITERATE_OVER(svi)

    songs_to_avoid.push_back(svi.val());
  }
  updateItemList();
}

void SongSelectorLogic::avoidNoSongs()
{
  QMutexLocker _ml(&lock);
  songs_to_avoid.clear();
  updateItemList();
}

void SongSelectorLogic::selectionDelTags()
{
  QMutexLocker _ml(&lock);
  bool ok;
  QString tags = QInputDialog::getText("Tags",
	         "Enter tags to delete (separated by spaces)",
		 QLineEdit::Normal,QString::null,&ok,this);
  if (ok && !tags.isEmpty())
    {
      selectedSongIterator svi;
ITERATE_OVER(svi)
songDelTag(svi.val(),tags);};
    }
}

void SongSelectorLogic::doLicense()
{
  QMutexLocker _ml(&lock);
  About box(2);
}

void SongSelectorLogic::doAbout()
{
  QMutexLocker _ml(&lock);
  About box(1);
}

void SongSelectorLogic::searchLineEntered()
{
  QMutexLocker _ml(&lock);
  searchLine -> setText(searchLine->text().upper());
  updateItemList();
  songList->setFocus();
}

void SongSelectorLogic::quitButton()
{
  QMutexLocker _ml(&lock);
  close();
}

void SongSelectorLogic::selectAllButTagged()
{ 
  QMutexLocker _ml(&lock);
  Q3ListViewItemIterator it(tagList);
  while(it.current())
    {
      Q3ListViewItem * c = (it++).current();
      if (c->text(TAGS_TEXT)!=EMPTY)
	{
	  c->setText(TAGS_AND,TAG_FALSE);
	  c->setText(TAGS_NOT,TAG_TRUE);
	}
    }
  updateItemList();
}

void SongSelectorLogic::selectAllTags()
{
  QMutexLocker _ml(&lock);
  
  Q3ListViewItemIterator it(tagList);
  while(it.current())
    {
       Q3ListViewItem * c = (it++).current();
      if (c->text(TAGS_TEXT)!=EMPTY)
	{
	  c->setText(TAGS_NOT,TAG_FALSE);
	  c->setText(TAGS_AND,TAG_FALSE);
	  c->setText(TAGS_OR,TAG_FALSE);
	}
      else
	{
	  c->setText(TAGS_NOT,TAG_FALSE);
	  c->setText(TAGS_AND,TAG_FALSE);
	  c->setText(TAGS_OR,TAG_TRUE);
	}
    }
  updateItemList();
}

void SongSelectorLogic::addTag(const QString tag)
{
  QMutexLocker _ml(&lock);
  if (tagList->findItem(tag,TAGS_TEXT)) return;
  new Q3ListViewItem(tagList,tag,TAG_FALSE,TAG_FALSE,TAG_FALSE);
}

#ifdef INCOMPLETE_FEATURES
void SongSelectorLogic::findallsimilarnames()
{
  QMutexLocker _ml(&lock);
  
  QString text = QFileDialog::getExistingDirectory(NULL,this,NULL,
		   "Specify directory to compare with database");
  // ask the name to start with
  if (!text.isEmpty())
    {
      SimilarScanner *scanner = new SimilarScanner(this);
      scanner->setRoot(text);
      scanner->exec();
    }
}

void SongSelectorLogic::findsimilarnames()
{
  QMutexLocker _ml(&lock);
  
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
#endif

extern bool reading_index;
void SongSelectorLogic::importSongs()
{
  QMutexLocker _ml(&lock);
  bool explain_analysis = !Config::get_file_count();
  if (explain_analysis)
    Config::set_file_count(1);
  if (::reading_index)
    {
      QMessageBox::message(NULL,
			   "The importer cannot be started while "
			   "scanning the existing index files\n");
      return;
    }
  MusicScanner scanner(this);
  scanner.show();
  scanner.scan();
  database->flush_cache();
  updateItemList();
  scanner.exec();
  if (explain_analysis)
    QMessageBox::message("What next ?",
      "The importer has created a number of empty index files. Before they "
      "are useful they should be filled with appropriate meta-data. This can "
      "be done by analyzing the songs. To do so. Select them in the Songs-tab, "
      "press the right mouse button and choose 'Analyzers' from the "
      "popup-menu.");
}

void SongSelectorLogic::selectionMenu()
{
  QMutexLocker _ml(&lock);
  
  selection->exec(QCursor::pos());
  searchLine->setFocus();
}

void SongSelectorLogic::openQueueMenu()
{
  QMutexLocker _ml(&lock);
  
  queuemenu->exec(QCursor::pos());
}

void SongSelectorLogic::queueFindAndRename(int p_old, int p_new)
{
  QMutexLocker _ml(&lock);
  
  listViewIterator<QueuedSong> entry(queue);
ITERATE_OVER(entry)

    int pos = entry.val()->getPos();
    if (pos == p_old)
    {
      entry.val()->setPos(p_new);
      return;
    }
  }
}

void SongSelectorLogic::queueShiftUp()
{
  QMutexLocker _ml(&lock);
  
  bool restart = true;
  while(restart)
    {
      restart=false;
      queue->sort();
      listViewIterator<QueuedSong> entry(queue);
ITERATE_OVER(entry)

	if (entry.val()->isSelected())
	{
	  int pos = entry.val()->getPos();
	  if (pos==1)
	  {
	    QMessageBox::warning(NULL,"Cannot shift upwards",
				 "Because you have selected the topmost song,"
				 "I cannot shift the others songs upward.\n");
	    return;
	  }
	  queueFindAndRename(pos-1,pos);
	  entry.val()->setPos(pos-1);
	  entry.val()->setSelected(false);
	  restart=true;
	}
      };
    }
  queue->repaint();
}

void SongSelectorLogic::queueShiftDown()
{
  QMutexLocker _ml(&lock);
  
  bool restart = true;
  while(restart)
    {
      restart=false;
      queue->sort();
      QueuedSong * tomove = NULL;
      listViewIterator<QueuedSong> entry(queue);
ITERATE_OVER(entry)

	if (entry.val()->isSelected()) tomove = entry.val();
      }
      if (tomove)
	{
	  int pos = tomove->getPos();
	  if (pos==queue->childCount())
	    {
	      QMessageBox::warning(NULL,"Cannot shift downwards",
		   "Because you have selected the last song, "
		   "I cannot shift the others songs downward.\n");
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
  QMutexLocker _ml(&lock);
  
  bool restart;
  do
    {
      restart=false;
      listViewIterator<QueuedSong> entry(queue);
ITERATE_OVER(entry)

	if (entry.val()->isSelected())
	{
	  delete(entry.val());
	  restart = true;
	  break;
	};
      }
    }
  while(restart);
  queueOrder();
}

void SongSelectorLogic::queueAnker()
{
  QMutexLocker _ml(&lock);
  listViewIterator<QueuedSong> entry(queue);
ITERATE_OVER(entry)

    if (entry.val()->isSelected())
      entry.val()->setAnkered(!entry.val()->ankered());
  }
  queue->repaint();
}

void SongSelectorLogic::queueOrder()
{
  QMutexLocker _ml(&lock);
  // nu lopen we er nog eens door om ze normaal te nummeren
  int cur = 1;
  listViewIterator<QueuedSong> entry(queue);
ITERATE_OVER(entry)

    entry.val()->setPos(cur++);
  }
  queue->repaint();
}

void SongSelectorLogic::queueInsert(int count)
{
  QMutexLocker _ml(&lock);
  
  int cur = count+1;
  if (count==0) return;
  /**
   * First we run through the complete list to asisgn to each element a rank
   * which is a multiple of the count. We start at count+1. This is useful
   * if we want to insert songs directly after the main or monitor song
   */
  listViewIterator<QueuedSong> entry(queue);
ITERATE_OVER(entry)

    entry.val()->setPos(cur);
    cur+=count+1;}
  /**
   * if there is a main or monitor then the randomization will use that one as
   * anker. Otherwise we refrain from using any anker
   */
  int last_pos = (::main_song || monitor_song)  ? 1 : count + 2;
  {
    listViewIterator<QueuedSong> entry(queue);
ITERATE_OVER(entry)

      cur = entry.val()->getPos();
      if (cur>last_pos)
      {
	if (entry.val()->isSelected())
	  for(int i = last_pos ; i < cur; i++)
	    {
	      QueuedSong * last = new QueuedSong(queue,NULL);
	      last->setPos(i);
	      last->mark=true;
	    }
	last_pos = cur + 1;
      }
    };
  }
  // nu kunnen we al de ontbrekende songs invullen
  queueOrder();
  {
    listViewIterator<QueuedSong> entry(queue);
ITERATE_OVER(entry)

      entry.val()->setSelected(entry.val()->mark);
      entry.val()->mark=false;
    };
  }
  queueRandom();
}

void SongSelectorLogic::queueInsert()
{
  QMutexLocker _ml(&lock);
  
  bool ok;
  int count = 5;
  count = QInputDialog::getInteger("Insert Songs...",
	  "How many songs should be inserted ?",5,1,50,1,&ok);
  if (!ok) return;
  queueInsert(count);
}

void SongSelectorLogic::queueSelectSong()
{
  QMutexLocker _ml(&lock);
  queueRandom(true);
}

void SongSelectorLogic::queueRandom()
{
  QMutexLocker _ml(&lock);
  queueRandom(false);
}

bool SongSelectorLogic::rejectQueueProposal(Song * song, int position)
{
  QMutexLocker _ml(&lock);
  
  // check the two things that are playing
  if (song==::main_song) return true;
  if (song==::monitor_song) return true;
  // check the data in the queue so far
  listViewIterator<QueuedSong> queuedsong(queue);
ITERATE_OVER(queuedsong)

    Song * entry = queuedsong.val()->getSong();
    if (!entry) continue;
    if (weighDuplicateAuthors->isChecked())
    if (entry->get_author() == song->get_author())
    if (abs(queuedsong.val()->getPos()-position)<=Config::get_authorDecay()) 
      return true;
    if (entry->get_file() == song->get_file()) return true;
  };
  return false;
}


void SongSelectorLogic::filterProposedList(Song ** list, int &count, 
					   int position)
{
  QMutexLocker _ml(&lock);
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

extern SongCopier songCopier;
void SongSelectorLogic::queueCopySongs()
{
  QMutexLocker _ml(&lock);
  QString target = QFileDialog::getExistingDirectory(NULL,this,NULL,
		     "Specify directory to dump songs to");
  if (target.isEmpty()) return;
  songCopier.setTarget(target);
  listViewIterator<QueuedSong> qs(queue);
ITERATE_OVER(qs)

    Song* s = qs.val()->getSong();
    if (s) songCopier.fetch(s);
  }
}

void SongSelectorLogic::queueRandom(bool userChoice)
{
  QMutexLocker _ml(&lock);
  
  SongMetriek * metriek = NULL;
  /**
   * This loop will in every iteration try to find an undefined element and
   * then assign a value to it if there are no longer undefined elements, the
   * loop breaks.
   */
  Song * firstiflacking = ::monitor_song;
  if (!firstiflacking) firstiflacking = ::main_song;
  while(true)
    {
      // find before, element and after
      QueuedSong *before =  NULL;
      QueuedSong *after = NULL;
      QueuedSong *element = NULL;
      listViewIterator<QueuedSong> entry(queue);
ITERATE_OVER(entry)

	if (!element && entry.val()->isSelected()) element = entry.val();
	if (entry.val()->ankered())
	{
	  if (element)
	    {
	      after = entry.val();
	      break;
	    }
	  else 
	    before = entry.val();
	};
      }
      // check boundaries
      if (!element) break;
      // deselecting the element will make sure that this loop will always end,
      // even if no filler song can be found.
      element->setSelected(false);
      if (element->ankered())
	continue;
      
      // we now see which songs are the two endpoints;
      Song *a = before ? before->getSong() : firstiflacking;
      float4 A = before ? before->getPos()  : 0;
      if (!a)
	{
	  QMessageBox::warning(NULL,"Randomize",
	     "Cannot randomize an element without an ankered element before");
	  continue;
	}

      if (!after)
	{
	  QMessageBox::warning(NULL,"Randomize",
            "Cannot randomize an element without an ankered element after");
	  continue;
	}
      Song *c = after->getSong();
      float4 C = after->getPos();
      
      // find a small set of closest friends
      float4 B = element->getPos();
      // we take the standard song metric
      
      metriek = &SongMetriek::std;

      // find a song close to the target position
      int count;
      Song ** proposed = database->closestSongs(this, a, C-B, c, B-A,
						metriek, 100, count);
      element -> setSong(NULL,-1);
      Song * choice = NULL;
      // now run through the proposed list and remove anything duplicate 
      // depending on the users preferences
      filterProposedList(proposed,count,element->getPos());
      // let the user choose or choose self
      if (count>0)
	{
	  if (userChoice)
	    {
	      Q3PopupMenu * songs = new Q3PopupMenu(this);
	      count = 20;
	      for(int i = 0 ; i < count ; i ++)
		songs->insertItem( proposed[i]->get_title()+"["+
				   proposed[i]->get_author()+"]", i, i);
	      int result = songs->exec();
	      if (result>=0 && result < count)
		choice = proposed[result];
	    }
	  else
	    choice = proposed[0];
	  bpmdj_deallocate(proposed);
	  if (choice)
	    element->setSong(choice,choice->distance(a,B-A,c,C-B,metriek));
	  else
	    element->setSong(choice,0);
	}
    }
  queueOrder();
}

void SongSelectorLogic::selectAlbumItem(Q3ListViewItem* i)
{
  QMutexLocker _ml(&lock);
  
  AlbumItem *album = (AlbumItem*)i;
  if (!album) return;
  if (album->get_song()) return;
  songList->clearSelection();
  // traverse through all children and find them in the songlist...
  AlbumItem *child = (AlbumItem*)album->firstChild();
  const vector<Song*> & all_songs = database->getAllSongs();
  while(child)
    {
      QString file = child->text(ALBUM_FILE);
      constVectorIterator<Song*> ss(all_songs);
ITERATE_OVER(ss)

	if (file == ss.val()->get_file())
	new QueuedSong(ss.val(),queue);
      }
      child = (AlbumItem*)child->nextSibling();
    }
}

void SongSelectorLogic::renameAlbumItem(Q3ListViewItem* i)
{
  QMutexLocker _ml(&lock);
  
  if (!i) return;
  AlbumItem *item = (AlbumItem*)i;
  if (!item->get_song()) return;
  item->startRename(0);
}

void SongSelectorLogic::albumItemChanged(Q3ListViewItem* i, int col)
{
  QMutexLocker _ml(&lock);
  
  AlbumItem *item = (AlbumItem*)i;
  if (!item) return;
  Song *song = item->get_song();
  if (!song) return;
  QString file = song->get_storedin();
  if (file.isEmpty()) return;
  Index index(file);
  AlbumField *album = index.find_album(i->parent()->text(0));
  if (!album) 
    {
      Error("album entry %s that is unknown in index file",
	    (const char*)i->text(1));
      return;
    }
  album -> nr = atoi(i->text(0));
  index . write_idx();
  item -> fixNr();
}

void SongSelectorLogic::changeTagList(Q3ListViewItem* item, const QPoint & pos,
				      int col)
{
  QMutexLocker _ml(&lock);
  if (col < TAGS_OR || col > TAGS_NOT) return;
  if (!item) return;
  QString c = item -> text(col);
  item -> setText(col,c == TAG_TRUE ? TAG_FALSE : TAG_TRUE);
  updateItemList();
}

void SongSelectorLogic::deleteSongFromAlbum(AlbumItem * item)
{
  QMutexLocker _ml(&lock);
  if (!item) return;
  Song *song = item->get_song();
  if (!song) return;
  QString file = song->get_storedin();
  if (file.isEmpty()) return;
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
  QMutexLocker _ml(&lock);
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
  QMutexLocker _ml(&lock);
  if (e->key() == Qt::Key_Escape) return;
  QDialog::keyPressEvent(e);
}

void SongSelectorLogic::customEvent(QEvent * e)
{
  if ((int)e->type()==(int)BpmDjCustom)
    {
      QMutexLocker _ml(&lock);
      BpmDjEvent * E = (BpmDjEvent*)e;
      E->run();
    }
}

void SongSelectorLogic::openLogDialog()
{  
  QMutexLocker _ml(&lock);
  LogViewer(this).exec();
}

static Song* last_fragment_play_request = NULL;
void SongSelectorLogic::playFragment(int i)
{
  if (!Config::play_fragments) return;
  QMutexLocker _ml(&lock);
  Song * song = QSong::songEssence(i);
  if (!song) return;
  fragmentCache.get(song);
  last_fragment_play_request = song;
}

void SongSelectorLogic::fragmentCreated(FragmentCreated* fc)
{
  if (!Config::play_fragments) return;
  if (last_fragment_play_request != fc->get_song()) return;
  fragmentPlayer.playWave(fc->getFragment());
  last_fragment_play_request = NULL;
}
#endif // __loaded__selector_cpp__
