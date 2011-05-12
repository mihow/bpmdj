/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2006 Werner Van Belle

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
#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlineedit.h>
#include <qframe.h>
#include <qheader.h>
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
#include "importscanner.h"
#include "aboutbox.h"
#include "albumbox.h"
#include "qsong.h"
#include "version.h"
#include "song-metric.h"
#include "kbpm-dj.h"
#include "process-manager.h"
#include "similarscanner.h"
#include "cluster.h"
#include "queuedsong.h"
#include "historysong.h"
#include "merger-dialog.h"
#include "pca.h"
#include "songtree.h"
#include "avltree.h"
#include "edit-distance.h"
#include "index-reader.h"
#include "scripts.h"
#include "spectrum-type.h"
#include "foreach.h"
#include "qvectorview.h"
#include "tags.h"
#include "common.h"
#include "freq-mapping.h"
#include "memory.h"
#include "song-statistics.h"
#include "cluster-dialog.h"
#include "history.h"
#include "log-viewer.logic.h"
#include <qaction.h>
#include "bpmdj-event.h"

#define foreach_qsong(V,B)  for(int V##idx = 0 ; V##idx < QSong::get_song_count() ; V##idx++) { Song * V = QSong::get_songs(V##idx); B ; } 
#define foreach_selected_qsong(V,B) foreach_qsong ( V, if ( QSong::get_selected(V##idx) ) { B ; } ; )

SongSelectorLogic * song_selector_window = NULL;

SongSelectorLogic::~SongSelectorLogic()
{
  QMutexLocker _ml(&lock);
  delete processManager;
  delete database;
}

SongSelectorLogic::SongSelectorLogic(QWidget * parent, const QString name) :
  SongSelector(parent,name), lock(true)
{
  QMutexLocker _ml(&lock);
  QGridLayout* statuslayout = new QGridLayout(statusframe, 1, 1, 0, 0); 
  status = new QStatusBar(statusframe);
  status -> setSizeGripEnabled(false);
  status -> setFont(QFont("Arial",10,QFont::Normal));
  status -> message("BpmDj v"VERSION" (c) Werner Van Belle 2000-2006");
  statuslayout->addWidget(status,0,0);
  // fix frame thing
  splitter->setResizeMode(header_frame,QSplitter::KeepSize);
  // insert songList
  QGridLayout* songListFrameLayout = new QGridLayout( songListFrame, 1, 1, 0, 0, "songListFrameLayout"); 
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
  songList->addColumn( tr( "Rythm" ) );
  songList->addColumn( tr( "Composition" ) );
  songList->setShowSortIndicator( TRUE );
  songList->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, songList->sizePolicy().hasHeightForWidth() ) );
  songList->setFrameShape( QListView::Panel );
  songList->setResizePolicy( QScrollView::Manual );
  songList->header()->moveSection(LIST_HISTOGRAM,2);
  songList->header()->moveSection(LIST_RYTHM,2);
  songList->header()->moveSection(LIST_COMPOSITION,2);
  songListFrameLayout->addWidget( songList, 0, 0 );
  
  connect( songList, SIGNAL( doubleClicked(int) ), this, SLOT( selectSong(int) ) );
  connect( songList, SIGNAL( ctrlReturnPressed(int) ), this, SLOT( selectSong(int) ) );
  connect( songList, SIGNAL( returnPressed(int) ), this, SLOT( selectionMenu() ) );
  connect( songList, SIGNAL( rightButtonPressed(int,const QPoint&,int) ), this, SLOT( selectionMenu() ) );
  
  // link with the processmanager
  processManager = new ProcessManager(this);
  analyzers = new AnalyzersManager(8,this,480);
  database = new DataBase();
  
  // initialise fields
  mainTicks=0;
  timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()), SLOT(timerTick()));
  timer->start(1000);
  albumList->installEventFilter(this);
  queue->installEventFilter(this);
  frequency_dialog = NULL;

  // create menu structure
  QMenuBar   * main = new QMenuBar(this);
  QPopupMenu * help = new QPopupMenu(this);
  QPopupMenu * before = new QPopupMenu(this);
  QPopupMenu * file = new QPopupMenu(this);
  selection = new QPopupMenu(this);
  queuemenu = new QPopupMenu(this);
  QPopupMenu * view  = new QPopupMenu(this);
  QPopupMenu * autom = new QPopupMenu(this);
  
  // file menu
  file->insertItem("&Preferences",this,SLOT(doPreferences()));
  file->insertItem("&Clear Main",this,SLOT(switchMonitorToMain()));
  file->insertItem("Bpm Mixing Desk", this, SLOT (openBpmMixer()));
  file->insertItem("Play &Mixers", this, SLOT (openMixer()));
  file->insertSeparator();
  file->insertItem("Record", this, SLOT (openRecorder()));
  file->insertItem("Replay", this, SLOT (openReplay()));
  file->insertItem("Record Mixers", this, SLOT (openRecordMixer()));
  file->insertSeparator();
  file->insertItem("Inspect log files",this, SLOT(openLogDialog()));
  file->insertItem("Statistics",this, SLOT(openStatistics()));
  file->insertItem("&Quit",this,SLOT(quitButton()));
  
  // help menu
  help->insertItem("&About",this,SLOT(doAbout()));
  help->insertItem("Online &Manual",this,SLOT(doOnlineHelp()));
  help->insertItem("L&icense",this,SLOT(doLicense()));
  
  // before menu
  before->insertItem("&Import songs ("SONG_EXT")",this,SLOT(importSongs())); // retrieves from ./music all songs

  before->insertItem("Rename songs with wrong title...",this,SLOT(startRenamer()));
  before->insertSeparator();
  before->insertItem("&Check/import cdrom content",this,SLOT(checkDisc())); // checks wether the songs on a disk can be found somewhere in the indices
#ifdef INCOMPLETE_FEATURES
  before->insertItem("Find similar song names in directory...",this,SLOT(findallsimilarnames()));
  before->insertItem("Mark duplicates (experimental)",this,SLOT(doMarkDups())); // searches the current list and marks all MD5sum duplicates
#endif
  before->insertItem("Backup indices",this,SLOT(doBackup()));

  // view color menu
  view->insertSeparator();
  linkViewProp(Config::color_played,       view,"Color already played titles");
  linkViewProp(Config::color_authorplayed, view,"Color already played authors");
  linkViewProp(Config::color_songs_based_on_history, view,"Color songs based on history");
  linkViewProp(Config::color_range,       view,"Color songs within tempo range");
  linkViewProp(Config::color_ondisk,       view,"Color songs not on disk");
  linkViewProp(Config::color_cues,         view,"Color songs without cues");
  linkViewProp(Config::color_dcolor,       view,"Color spectrum distance");
  linkViewProp(Config::color_spectrum,     view,"Color PCA of spectrum");
  
  // view display part
  view->insertSeparator();
  view -> insertItem("Limit selection to &Tempo range",this,SLOT(toggle_temporange()));
  
  linkViewProp(Config::limit_uprange,         view,"&Up Tempo Range (0 < BPM < 0.06)");
  linkViewProp(Config::limit_downrange,       view,"&Down Tempo Range (-0.06 < BPM < 0)");
  linkViewProp(Config::limit_nonplayed,       view,"Limit selection to &Non-played songs");
  linkViewProp(Config::limit_ondisk,          view,"Limit selection to &Files on disk");
  linkViewProp(Config::limit_indistance,      view,"Limit selection to &dColor range");
  linkViewProp(Config::limit_authornonplayed, view,"Limit selection to non played authors");
  
  // view dialog part
  view -> insertSeparator();
  view -> insertItem("Frequency shaping dialog",this,SLOT(show_freq_shaping_dialog()));
  //  view->setCheckable(true);
  
  // auto menu
  linkAutoProp(Config::auto_popqueue  ,autom,"Pop queue when monitor empty");
  linkAutoProp(Config::ask_mix        ,autom,"Ask 'how did the mix go'");
  linkAutoProp(Config::open_bpmmixer  ,autom,"Open Bpm Mixer when starting program");
  //  autom->setCheckable(true);
  
  // selection menu
  selection->insertItem("&Add tag...",this,SLOT(selectionAddTags()));
  selection->insertItem("&Delete tag...",this,SLOT(selectionDelTags()));
  selection->insertItem("Set Author...",this,SLOT(selectionSetAuthor()));
  selection->insertItem("&Edit info...",this,SLOT(selectionEditInfo()));
  selection->insertItem("&Insert in album...",this,SLOT(selectionInsertInAlbum()));
  selection->insertItem("&Analyze",this,SLOT(queueAnalysis()));
  selection->insertItem("Pca Analysis Sound Color",this,SLOT(doSpectrumPca()));
  selection->insertItem("Cluster Analysis",this,SLOT(doClustering()));
  selection->insertSeparator();
  selection->insertItem("This is the main song, but don't play it",this,SLOT(selectionSetMainSong()));
#ifdef INCOMPLETE_FEATURES
  selection->insertItem("Play in 1st Extra Player",this,SLOT(selectionPlayIn3th()));
  selection->insertItem("Play in 2nd Extra Player",this,SLOT(selectionPlayIn4th()));
#endif
  selection->insertItem("Queue",this,SLOT(selectionAddQueue()));
  selection->insertSeparator();
  selection->insertItem("&Fetch selection from cdrom...",this,SLOT(fetchSelection()));
  selection->insertSeparator();
  selection->insertItem("Select all songs without tag",this,SLOT(selectAllButTagged()));
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
  queuemenu->insertItem("Copy songs to directory...",this,SLOT(queueCopySongs()));  
  
  // main menu
  main->insertItem("&Program",file);
  main->insertItem("&View",view);
  main->insertItem("&Auto",autom);
  main->insertItem("&Selection",selection);
  main->insertItem("&Queue",queuemenu);
  main->insertItem("&Management",before);
  main->insertItem("&Help",help);
  
  // create player views
  playerLayout->addWidget(new SongSelectorPlayView(header_frame,Config::players[0]));
  playerLayout->addWidget(new SongSelectorPlayView(header_frame,Config::players[1]));
  playerLayout->addWidget(new SongSelectorPlayView(header_frame,Config::players[2]));
  playerLayout->addWidget(new SongSelectorPlayView(header_frame,Config::players[3]));

  // create analyzer views
  for(int i = 0 ; i < 4 ; i ++)
    {
      SongSelectorAnalView * anal_view = new SongSelectorAnalView(header_frame,analyzers,Config::analyzers[i]);
      analLayout1->addWidget(anal_view);
      anal_view = new SongSelectorAnalView(header_frame,analyzers,Config::analyzers[4+i]);
      analLayout2->addWidget(anal_view);
    }
  header_frame->resize(header_frame->minimumSizeHint());
  
  for(int i = 0 ; i < 8; i ++)
    connect(&Config::analyzers[i],SIGNAL(stateChanged()),this,SLOT(toAnalyzeOrNot()));
};

void SongSelectorLogic::initialize_using_config()
{
  QMutexLocker _ml(&lock);
  updateProcessView(false);
};

void SongSelectorLogic::linkViewProp(ConfigState & p, QPopupMenu *m, QString t)
{
  QMutexLocker _ml(&lock);
  p.setMenuText(t);
  p.addTo(m);
  connect(&p,SIGNAL(toggled()),this,SLOT(updateItemList()));
}

void SongSelectorLogic::linkAutoProp(ConfigState & p, QPopupMenu *m, QString t)
{
  QMutexLocker _ml(&lock);
  p.setMenuText(t);
  p.addTo(m);
}

void SongSelectorLogic::toggle_temporange()
{
  QMutexLocker _ml(&lock);
  bool newstate =  !(Config::limit_uprange.isOn() || Config::limit_downrange.isOn());
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
    pos = "http://bpmdj.sourceforge.net";

  // code location
  QString bin;
  if (exists(bin="/usr/bin/mozilla") ||
      exists(bin="/usr/bin/konqueror"))
    {
      spawn(bin+" "+pos+"/index.html");
    }
  else
    QMessageBox::warning(this,"Online","Please browse to http://bpmdj.sourceforge.net/");
}

void SongSelectorLogic::doBackup()
{
  QMutexLocker _ml(&lock);
  time_t t_in=time(&t_in);
  struct tm * t = NULL;
  t = localtime(&t_in);
  vexecute("tar -cvzf bpmdj-index-backup-%d-%d-%d-%d-%d.tgz index",t->tm_mday,t->tm_mon,1900+t->tm_year,t->tm_hour,t->tm_min);
}

void* goSpectrumPca(void * whatever)
{
  SongSelectorLogic* ss = (SongSelectorLogic*)whatever;
  ss -> doSpectrumPca(true,false);
  status->message("Spectrum PCA finished",2000);
  return whatever;
}

void SongSelectorLogic::start_spectrum_pca()
{
  QMutexLocker _ml(&lock);
  pthread_t *checker = bpmdj_allocate(1,pthread_t);
  pthread_create(checker,NULL,goSpectrumPca,this); 
}

void SongSelectorLogic::start_existence_check()
{ 
  QMutexLocker _ml(&lock);
  database->start_existence_check();
}

void SongSelectorLogic::initialize_extras()
{ 
  QMutexLocker _ml(&lock);
 // first parse all the tags...
  parse_tags();
  bool ok = false;
  QListViewItem * e = NULL;
  QListViewItemIterator it(tagList);
  while(it.current() && !ok)
    {
      QListViewItem *c = it.current();
      if (c->text(TAGS_TEXT)==EMPTY)
	e = c;
      if (c->text(TAGS_OR)==TAG_TRUE)
	ok=true;
      it++;
    }
  if (!ok && e)
    e->setText(TAGS_OR,TAG_TRUE);
  
  // update statistics
  GrowingArray<Song*>* songs = database->getAllSongs();
  for(int i = 0 ; i < songs->count ; i ++)
    statistics_first_pass(songs->elements[i]);
  statistics_prepare_second_pass();
  for(int i = 0 ; i < songs->count ; i ++)
    statistics_second_pass(songs->elements[i]);
  statistics_stop_second_pass();

  // update albumlist
  for(int i = 0 ; i < songs->count ; i ++)
    {
      Song * song = songs->elements[i];
      AlbumField ** albums = song -> get_albums();
      while(*albums)
	{
	  AlbumField * album = *albums++;
	  AlbumItem  * ai = (AlbumItem*)albumList->findItem(album->name,0);
	  if (!ai) ai = new AlbumItem(album->name,albumList);
	  new AlbumItem(album->nr,song,ai);
	}
    }
  
  // check which tags were marked in the previous session
  QListViewItemIterator t(tagList);
  while(t.current())
    {
      QListViewItem * i = t.current();
      QListViewItem * f = Config::get_taglist()->findItem(i->text(0),0);
      if (f)
	{
	  i->setText(1,f->text(1));
	  i->setText(2,f->text(2));
	  i->setText(3,f->text(3));
	}
      t++;
    }
  Config::set_taglist(tagList);

  // mark all the played songs
  History::History(".played.log",database,playedList);
  
  // copy the header information from the config file
  QHeader *h = songList->header();
  copy_header(Config::get_header(),h);
  Config::set_header(h);
  updateItemList();
}

void SongSelectorLogic::parse_tags()
{
  QMutexLocker _ml(&lock);
  if (!Tags::new_tags) return;
  for(int i = 0 ; i < Tags::tag_names.count ; i++)
    addTag(Tags::tag_names.elements[i]);
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
  int itemcount = database -> getSelection(this,main,songList,Config::get_max_songs());
  songList->repaint();
  countLcd->display(itemcount);
  // nothing selected ?
  if (itemcount==0 && !alreadygavefilterwarning && Config::get_file_count())
    {
      QMessageBox::warning(this,"Empty Selection","With the current selection of filters,\n"
			   "there are no songs to be displayed.\n"
			   "If you want to modify this, go to the View menu.");
      alreadygavefilterwarning = true;
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
      AvlTree<QString> *file2song = database->getFileTreeRef();
      HistoryField ** after = main_index . get_prev_songs();
      while(after && *after)
	{
	  QString file = (*after)->file;
	  QString count = "-"+QString::number((*after)->count);
	  QString comment = (*after)->comment;
	  SongSortedByFile *s = (SongSortedByFile*)file2song->search(file);
	  if (s)
	    {
	      new HistorySong(s->song,count,comment,historyList);
	      s->song->get_distance_to_main();
	    }
	  after++;
	}
      HistoryField ** before = main_index . get_next_songs();
      while (before && *before)
	{
	  QString file = (*before)->file;
	  QString count = "+"+QString::number((*before)->count);
	  QString comment = (*before)->comment;
	  SongSortedByFile *s = (SongSortedByFile*)file2song->search(file);
	  if (s)
	    {
	      s->song->get_distance_to_main();
	      new HistorySong(s->song,count,comment,historyList);
	    }
	  before++;
	}
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
   * The timer thread wil periodically call this function. 
   * Since it is the same thread it will be able to continue
   * below. We don' want that because then we might have 
   * race conditions.
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
  processManager->checkSignals();
  analyzers->checkSignals();
  checkers.checkSignals();
  
  // we can now check whether any button has been check to start
  // an analyzer. We do this after the signal check because this
  // function will also redraw the colors for the analyzer and will
  // ensure an immediate color change when the text is updated in the 
  // signal checking round.
  toAnalyzeOrNot();
  
  // auto pop queue
  // we can only pop every 10 seconds and if the mainplayer
  // reports an error we turn of auto-popping...
  static int pop_ticks = 0;
  pop_ticks++;
  if (Config::auto_popqueue)
    if (!::monitor_song && pop_ticks > 10)
      {
	QListViewItemIterator it(queue);
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
  if (main_changed) updateItemList();
  // update the frequency picture
  updateFrequencyMap();
}

void SongSelectorLogic::show_freq_shaping_dialog()
{
  QMutexLocker _ml(&lock);
  if (!frequency_dialog)
    frequency_dialog = new FrequencyDialog();
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
	  float energy_fix = b->band(j)-a->band(j);
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
      float dbf = 199.0/(max_freq-min_freq);  //pixels per dB
      float y = 199.0;
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
	      float energy_fix = b->band(j)-a->band(j);
	      energy_fix -= min_freq;
	      p.drawPoint(j,(int)(199.0-dbf*energy_fix));
	    }
	}
    }
  p.end();
  frequency_dialog->frequency_map->setPixmap(*pm);
}

int songFileCompare(const void * a, const void* b)
{
  Song **pa=(Song**)a;
  Song **pb=(Song**)b;
  Song *A=*pa;
  Song *B=*pb;
  return strcmp(A->get_file(),B->get_file());
}

void SongSelectorLogic::checkDisc()
{
  QMutexLocker _ml(&lock);
  // 0 - which CD is this ?
  bool ok;
  QString discname = QInputDialog::getText("Check/Import Disc"
					   ,"1. Insert a CD\n2. Enter its number (without trailing slash)"
					   ,QLineEdit::Normal,QString::null,&ok,this);
  if (!ok || discname.isEmpty()) return;
  // 1 - mount cdrom
  start_mount_cdrom();
  // 2 - Scan directory
  ImportScanner scanner(this);
  scanner.scan(CDROM,discname);
  updateItemList();
  // 3 - umount cdrom
  start_umount_cdrom();
}

void SongSelectorLogic::openMixer()
{
  QMutexLocker _ml(&lock);
  if (Config::get_mixer_command().isEmpty())
    QMessageBox::message(NULL,"Please insert an appropriate command in the preferences dialog\n");
  else
    spawn(Config::get_mixer_command());
}

void SongSelectorLogic::openBpmMixer()
{
  QMutexLocker _ml(&lock);
  if (Config::get_bpm_mixer_command().isEmpty())
    QMessageBox::message(NULL,"Please insert an appropriate command in the preferences dialog\n");
  else
    spawn(Config::get_bpm_mixer_command());
}

void SongSelectorLogic::openRecorder()
{
  QMutexLocker _ml(&lock);
  if (Config::get_record_command().isEmpty())
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
	  sprintf(command,(const char*)Config::get_record_command(),(const char*)trackname);
	  spawn(command);
	}
    }
}

void SongSelectorLogic::openReplay()
{
  QMutexLocker _ml(&lock);
  if (Config::get_record_command().isEmpty())
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
	  sprintf(command,(const char*)Config::get_replay_command(),(const char*)trackname);
	  spawn(command);
	}
    }
}

void SongSelectorLogic::openRecordMixer()
{
  QMutexLocker _ml(&lock);
  if (Config::get_record_command().isNull())
    QMessageBox::message(NULL,"Please create an appropriate record_mixer command in the preferences dialog\n");
  else
    spawn((const char*)Config::get_record_mixer_command());
}

void SongSelectorLogic::doSpectrumPca(bool fulldatabase, bool update_process_view)
{
  QMutexLocker _ml(&lock);
  int count = 0;
  float ** data;
  GrowingArray<Song*>* all = NULL;
  if (fulldatabase)
    {
      all = database->getAllSongs();
      for(int i = 0 ; i < all->count ; i ++)
	if (all->elements[i]->get_spectrum() != no_spectrum) count++;
      if ( count == 0 ) return;
      data = matrix(count,spectrum_size);
      for(int i = 0, written = 0 ; i < all->count ; i ++)
	{
	  Song * svi = all->elements[i];
	  if (svi->get_spectrum() != no_spectrum)
	    {
	      assert(written<count);
	      ++ written; 
	      for (int j = 0 ; j < spectrum_size ; j++)
		data [ written ] [ j + 1 ] = svi -> get_spectrum() -> band ( j ) ;
	    }
	}
    }
  else
    {
      foreach_selected_qsong(svi, if (svi->get_spectrum()!=no_spectrum) count++);
      if ( count == 0 ) return;
      data = matrix(count,spectrum_size);
      int written = 0;
      foreach_selected_qsong(svi, 
			     if (svi->get_spectrum()!=no_spectrum)
                               {
				 ++ written;
				 for (int i = 0 ; i < spectrum_size ; i++)
				   data[written][i+1]=svi->get_spectrum()->band(i);
			       });
    }

  // 2. do principal component analysis
  char * error_msg = NULL;
  do_pca(count,spectrum_size,data,error_msg);
  if (error_msg)
    {
      QMessageBox::warning(this,"Principal Component Analysis", "An error occured, aborting pca");
      printf("Error messages is %s\n",error_msg);
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
      for(int i = 0, written = 0 ; i < all->count ; i ++)
	{
	  Song *svi= all->elements[i];
	  if (svi->get_spectrum() != no_spectrum)
	    {
	      written++;
	      float x = (data[written][1] - minx) / dx;
	      float y = (data[written][2] - miny) / dy;
	      float z = (data[written][3] - minz) / dz;
	      QColor transfer;
	      transfer.setRgb(127+(int)x,127+(int)y,127+(int)z);
	      svi->setColor(transfer);
	    }
	}
    }
  else
    {
      int written = 0;
      foreach_selected_qsong(svi, 
        if (svi->get_spectrum() != no_spectrum)
          {
	    written++;
	    float x = (data[written][1] - minx) / dx;
	    float y = (data[written][2] - miny) / dy;
	    float z = (data[written][3] - minz) / dz;
	    QColor transfer;
	    transfer.setRgb(127+(int)x,127+(int)y,127+(int)z);
	    svi->setColor(transfer);
	  });
    }
  
  // 4. clean up
  free_matrix(data, count, spectrum_size);
  if (update_process_view) 
    updateProcessView(true);
}

void SongSelectorLogic::doClustering()
{
  QMutexLocker _ml(&lock);
  // 0. Clear all existing colors
  QColor white(0,0,0);
  GrowingArray<Song*> * all = database->getAllSongs();
  for(int i=0;i<all->count;i++)
    {
      all->elements[i]->set_color(white);
      all->elements[i]->set_spectrum_string(EMPTY);
    }
  // 1. add all information in 1 cluster
  Cluster cluster;
  Cluster::reset();
  foreach_selected_qsong(svi,
			 if (svi->has_all_cluster_fields()) cluster.addPoint(svi));
  // 2. Do the Analysis
  ClusterDialog dialog(this);
  if (dialog.exec()==Rejected) return;
  SongMetriek metriek(*dialog.metrics);
  Couple * result = cluster.agglomerate(&metriek);
  // 3. Assign Colors
  result->determine_color((float)0,(float)360,0,Config::get_color_cluster_depth() - 1);
  // result->color_clusters_with_size(20,40);
  // result->color_clusters_dw2();

  // 4. Clean Up
  updateProcessView(false);
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
  FOREACH(queue, QueuedSong *, svi, 
	  toFetch[next++]=svi->getSong());
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
	  const char* filename=song->get_file();
	  fprintf(script,"%s/./music/%s\n",current_directory,filename);
	}
    }
  fclose(script);
  bpmdj_deallocate(current_directory);
}

void SongSelectorLogic::fetchSelection()
{
  QMutexLocker _ml(&lock);
  /* count nr of files to get */
  int selectionCount = 0;
  foreach_selected_qsong(svi, {svi=svi; selectionCount ++;});
  /* allocate enough space to store the songs */
  Song * * toFetch;
  int next=0;
  toFetch=(Song**)calloc(selectionCount,sizeof(Song*));
  assert(toFetch);
  next=0;
  /* read all the songs */
  foreach_selected_qsong(svi, toFetch[next++]=svi);
  /* sort the list */
  qsort(toFetch,next,sizeof(Song*),songFileCompare);
  /* umount any left over cdrom */
  start_umount_cdrom();
  /* fetch the files one by one */
  int i = 0;
  char *currentcd = 0;
  while(i<next)
    {
      Song* song=toFetch[i++];
      const char* filename=song->get_file();
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
	      bpmdj_deallocate(currentcd);
	      if (!start_umount_cdrom())
		QMessageBox::warning(this,"CD","Cannot unmount "CDROM);
	    }
	  /* ask user to input new cd */
	  currentcd=newcd;
	  QMessageBox::information(this,"CD","Please insert "+QString(newcd));
	  /* mount the cdrom */
	  if (!start_mount_cdrom())
	    QMessageBox::warning(this,"CD","Cannout mount " CDROM " (errno="+QString::number(errno)+")");
	}
      else 
	bpmdj_deallocate(newcd);
      /* create the target directory */
      char* dirc=strdup(filename);
      char* filenc=strdup(filename);
      char* dir=dirname(dirc);
      sprintf(temp,"./music/%s",dir);
      char* target=strdup(temp);
      start_mkdir(target);
      /* read the given file from the cdrom */
      sprintf(temp,CDROM"%s",index(filename,'/'));
      char* source=strdup(temp);
      start_cp(source,target);
      /* mark it on disk */
      song->set_ondisk(true);
      /* free source & target */
      bpmdj_deallocate(source);
      bpmdj_deallocate(target);
      bpmdj_deallocate(dirc);
      bpmdj_deallocate(filenc);
    }
  /* finally umount the last cdrom */
  start_umount_cdrom(false);
}

/**
 * This method queues all the selected songs in the analysis queue
 * If no analyzer is enabled we suggest the possibility to enable
 * one.
 */
void SongSelectorLogic::queueAnalysis()
{
  QMutexLocker _ml(&lock);
  foreach_selected_qsong(svi, 
			 new QueuedAnalSong(anal_queue,svi));
  bool there_is_one = false;
  for(int i = 0 ; i < 8 && !there_is_one ; i ++)
    there_is_one |= Config::analyzers[i].isEnabledOk();
  if (!there_is_one)
    QMessageBox::warning(NULL,"Analyzer needed",
			 "In order to analyze songs you need to enabled at least one analyzer\n"
			 "Please check one of the checkboxes (#1 .. #8) in the top pane\n");
}

void SongSelectorLogic::toAnalyzeOrNot()
{
  QMutexLocker _ml(&lock);
  for(int i = 0 ; i < 8 ; i ++)
    if (analyzers->slot_free(i))
      startAnotherAnalyzer(NULL,i);
}

/**
 * This function is called by the analyzer manager when a slot comes free
 * either due to a timeout or due to a descent stop. In any case we need
 * to start a new one here.
 */
void SongSelectorLogic::startAnotherAnalyzer(Song * finished_analyzing, int on_slot)
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
	  // as non analyzeable at the given slot and requeue the song
	  songAddTag(finished_analyzing,analTag);
	  new QueuedAnalSong(anal_queue,finished_analyzing);
	}
    }
  
  // update the progress bar
  float current_speed = 0.0;
  for(int i = 0 ; i < 8 ; i++)
    if (Config::analyzers[i].isEnabledOk())
      current_speed += Config::analyzers[i].songs_per_second();
  unsigned8 days = 0, hours = 0, minutes = 0, seconds = 0;
  if (current_speed != 0)
    {
      unsigned8 expected_time = (unsigned8)((float)anal_queue->childCount()/current_speed);
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
  QListViewItemIterator it(anal_queue);
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
  QString command = next->getCommand(Config::analyzers[on_slot]);
  // start it
  analyzers->start(on_slot, next->getSong(), command);
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
      QMessageBox::message(NULL,"You did not set the mandatory player commands.\n"
			   "Please fix these in the preferences box","Very interesting");
      doPreferences();
    }
  processManager->startSong(song);
}

void SongSelectorLogic::playQueueSong(QListViewItem *song)
{
  QMutexLocker _ml(&lock);
  if (!song) return;
  QueuedSong * qs = (QueuedSong*)song;
  Song * s = qs->getSong();
  if (!s) return;
  processManager->startSong(s);
}

void SongSelectorLogic::playHistorySong(QListViewItem *song)
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
  // write the indfex file
  idx->write_idx();
  // free the bastard
  delete idx;
  // now update the local songdata
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

void SongSelectorLogic::insertSongInAlbum(Song * song, const QString & album, int nr)
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
  reread_and_repaint(song);
}

void SongSelectorLogic::selectionAddTags()
{
  QMutexLocker _ml(&lock);
  bool ok;
  QString tags = QInputDialog::getText("Tags"
				       ,"Enter tags to add (seperated by spaces)"
				       ,QLineEdit::Normal,QString::null,&ok,this);
  if (ok && !tags.isEmpty())
    foreach_selected_qsong(svi, songAddTag(svi,tags));
}

void SongSelectorLogic::selectionSetAuthor()
{
  QMutexLocker _ml(&lock);
  bool ok;
  QString text = QInputDialog::getText("Set Author...", 
				       "Please enther the author name to be", 
				       QLineEdit::Normal,
				       QString::null, &ok, this );
  if ( ok ) 
    foreach_selected_qsong(svi, songSetAuthor(svi,text));
}

void SongSelectorLogic::selectionInsertInAlbum()
{
  QMutexLocker _ml(&lock);
  static QString last = "";
  static int lastnr = 0;
  AlbumBox albumbox(NULL,NULL,TRUE);
  albumbox.album->insertItem(last);
  albumbox.number->setValue(++lastnr);
  if (albumbox.exec()==QDialog::Accepted)
    {
      last = albumbox.album->currentText();
      lastnr = albumbox.number->value();
      foreach_selected_qsong(s, insertSongInAlbum(s,last,albumbox.number->value()));
    }
}

void SongSelectorLogic::selectionEditInfo()
{
  QMutexLocker _ml(&lock);
  foreach_selected_qsong(svi, songEditInfo(svi));
}

void SongSelectorLogic::selectionPlayIn3th()
{
#ifdef INCOMPLETE_FEATURES
  QMutexLocker _ml(&lock);
  foreach_selected_qsong(svi,
			 processManager->startExtraSong(2,svi);
			 return);
#endif
}

void SongSelectorLogic::selectionPlayIn4th()
{
#ifdef INCOMPLETE_FEATURES
  QMutexLocker _ml(&lock);
  foreach_selected_qsong(svi,
			 processManager->startExtraSong(3,svi);
			 return);
#endif
}

void SongSelectorLogic::selectionAddQueue()
{
  QMutexLocker _ml(&lock);
  queue->selectAll(false);
  QueuedSong *last = (QueuedSong*)queue->lastItem();
  if (last) last->setSelected(true);
  foreach_selected_qsong(svi,(new QueuedSong(svi,queue))->setSelected(true));
  queueInsert(extraSongs->value());
}

void SongSelectorLogic::doAutoMix()
{
  QMutexLocker _ml(&lock);
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
    sprintf(split_arguments, "kbpm-merge --split %s %s %s %s %s\n",
	    (mergerDialog.splitBeforeMix->isChecked() ? "--beforemix" : ""),
	    (mergerDialog.splitMiddleMix->isChecked() ? "--middlemix" : ""),
	    (mergerDialog.splitAfterMix->isChecked() ? "--aftermix" : ""),
	    (mergerDialog.splitAfterTempo->isChecked() ? "--aftertempo" : ""),
	    (mergerDialog.number->isChecked() ? "--number" : ""));
  // now create the script...
  QString old="";
  FILE * out = openScriptFile(PROCESS_MIX);
  fprintf(out,"rm -- " BPMMIXED_NAME " automix.log\n");
  
  FOREACH(queue,QueuedSong *, song,
    {
      Song * s = song->getSong();
      QString n = s->get_storedin();
      if (old=="")
	{
	  char * a = escape(n);
	  fprintf(out,"kbpm-merge %s %s\n",arguments,a);
	  bpmdj_deallocate(a);
	}
      else
	{
	  char * a = escape(old);
	  char * b = escape(n);
	  fprintf(out,"kbpm-merge %s %s %s\n",arguments,a,b);
	  bpmdj_deallocate(a);
	  bpmdj_deallocate(b);
	  if (mergerDialog.playMixes->isChecked())
	    fprintf(out,BPLAY MIXDUMP_NAME" &\n");
	}
      old=n;
    });
  if (should_split)
    fprintf(out,split_arguments);
  if (mergerDialog.wavConvert->isChecked())
    fprintf(out,"for a in *"RAW_EXT"; do "RAW2WAV" \"$a\" \"`basename $a "RAW_EXT"`\".wav; rm -- \"$a\"; done\n");
  if (mergerDialog.compress->isChecked())
    fprintf(out,ENCODE);
  fclose(out);
}

void SongSelectorLogic::selectionSetMainSong()
{
  QMutexLocker _ml(&lock);
  static bool dialog_shown=false;
  if (!dialog_shown)
    {
      dialog_shown=true;
      QMessageBox::information(this,"Removing the main song...","To remove the main song go to\n"
			       "Program|Clear Main");
    }
  foreach_selected_qsong(svi,
			 processManager->setMainSong(svi);
			 return);
}

void SongSelectorLogic::selectionDelTags()
{
  QMutexLocker _ml(&lock);
  bool ok;
  QString tags = QInputDialog::getText("Tags"
				       ,"Enter tags to delete (seperated by spaces)"
				       ,QLineEdit::Normal,QString::null,&ok,this);
  if (ok && !tags.isEmpty())
    foreach_selected_qsong(svi,songDelTag(svi,tags));
}

void SongSelectorLogic::doLicense()
{
  QMutexLocker _ml(&lock);
  ::doAbout(2);
}

void SongSelectorLogic::doAbout()
{
  QMutexLocker _ml(&lock);
  ::doAbout(1);
}

void SongSelectorLogic::searchLineEntered()
{
  QMutexLocker _ml(&lock);
  ;
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
 
  QListViewItemIterator it(tagList);
  while(it.current())
    {
      QListViewItem * c = (it++).current();
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
  
  QListViewItemIterator it(tagList);
  while(it.current())
    {
       QListViewItem * c = (it++).current();
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
  new QListViewItem(tagList,tag,TAG_FALSE,TAG_FALSE,TAG_FALSE);
}

#ifdef INCOMPLETE_FEATURES
void SongSelectorLogic::doMarkDups()
{
    QMutexLocker _ml(&lock);

  int nr=0;
  foreach_selected_qsong(svi,
    QString txt=svi->get_md5sum();
    printf("%d -- ",nr++);
    bool found=false;
    foreach_qsong(svi2,
      if (svi!=svi2 && txt==svi2->get_md5sum())
        {
	  found=true;
	  QSong::set_selected(svi2idx,true); // see foreach macro to understand this trick :)
	  break;
	});
    if (!found) QSong::set_selected(sviidx,false));  // see foreach macro to understand this trick :)
}

void SongSelectorLogic::findallsimilarnames()
{
  QMutexLocker _ml(&lock);
  
  QString text = QFileDialog::getExistingDirectory(NULL,this,NULL,"Specify directory to compare with database");
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

void SongSelectorLogic::importSongs()
{
  QMutexLocker _ml(&lock);
  
  bool explain_analysis = !Config::get_file_count();
  if (explain_analysis)
    Config::set_file_count(1);
  ImportScanner scanner(this);
  scanner.scan(MusicDir);
  database->flush_cache();
  updateItemList();
  scanner.exec();
  if (explain_analysis)
    QMessageBox::message(NULL,
			 "The importer has created a number of empty index files. Before they are usefull\n"
			 "they should be filled with appropriate meta-data. This can be done by analyzing\n"
			 "the songs. To do so. Select them in the Songs-tab, press the right mouse button\n"
			 "and choose 'Analyzers' from the popup-menu.");
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
  
  FOREACH(queue,QueuedSong *, entry,
    {
      int pos = entry->getPos();
      if (pos == p_old)
	{
	  entry->setPos(p_new);
	  return;
	}
    });
}

void SongSelectorLogic::queueShiftUp()
{
  QMutexLocker _ml(&lock);
  
  bool restart = true;
  while(restart)
    {
      restart=false;
      queue->sort();
      FOREACH(queue, QueuedSong *, entry,
	{
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
	});
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
      FOREACH(queue,QueuedSong *, entry,
	      if (entry->isSelected()) tomove = entry);
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
  QMutexLocker _ml(&lock);
  
  bool restart;
  do
    {
      restart=false;
      FOREACH(queue, QueuedSong *, entry,
	      if (entry->isSelected())
                {
		  delete(entry);
		  restart = true;
		  break;
		});
    }
  while(restart);
  queueOrder();
}

void SongSelectorLogic::queueAnker()
{
  QMutexLocker _ml(&lock);

  FOREACH(queue,QueuedSong *, entry, 
	  if (entry->isSelected())
	  entry->setAnkered(!entry->ankered()));
  queue->repaint();
}

void SongSelectorLogic::queueOrder()
{
  QMutexLocker _ml(&lock);
  
  // nu lopen we er nog eens door om ze normaal te nummeren
  int cur = 1;
  FOREACH(queue, QueuedSong * , entry, entry->setPos(cur++));
  queue->repaint();
}

void SongSelectorLogic::queueInsert(int count)
{
  QMutexLocker _ml(&lock);
  
  int cur = count+1;
  if (count==0) return;
  // eerst lopen we er door om ze allemaal een veelvoud van count te geven
  FOREACH(queue,QueuedSong *, entry,
	  entry->setPos(cur);
	  cur+=count+1);
  // if there is a main or monitor then the randomisation will
  // use that one as anker. Otherwise we refrain from using any anker
  int last_pos = ::main_song || monitor_song  ? 1 : count+1;
  FOREACH(queue,QueuedSong *, entry,
	  cur = entry->getPos();
	  if (cur>last_pos)
            {
	      if (entry->isSelected())
		  for(int i = last_pos ; i < cur; i++)
	            {
		      QueuedSong * last = new QueuedSong(queue,NULL);
		      last->setPos(i);
		      last->mark=true;
		    }
	      last_pos = cur + 1;
	    });
  // nu kunnen we dus lekker al de ontbrekende songs invullen
  queueOrder();
  FOREACH(queue,QueuedSong *, entry,
	  entry->setSelected(entry->mark);
	  entry->mark=false;);
  queueRandom();
}

void SongSelectorLogic::queueInsert()
{
  QMutexLocker _ml(&lock);
  
  bool ok;
  int count = 5;
  count = QInputDialog::getInteger("Insert Songs...","How many songs should be inserted ?",5,1,50,1,&ok);
  if (!ok) return;
  queueInsert(count);
}

void SongSelectorLogic::queueSelectSong()
{
  QMutexLocker _ml(&lock);
  ;
  queueRandom(true);
}

void SongSelectorLogic::queueRandom()
{
  QMutexLocker _ml(&lock);
  ;
  queueRandom(false);
}

bool SongSelectorLogic::rejectQueueProposal(Song * song, int position)
{
  QMutexLocker _ml(&lock);
  
  // check the two things that are playhing
  if (song==::main_song) return true;
  if (song==::monitor_song) return true;
  // check the data in the queue so far
  FOREACH(queue, QueuedSong *, queuedsong, 
	  Song * entry = queuedsong->getSong();
	  if (!entry) continue;
	  if (weighDuplicateAuthors->isChecked())
	  if (entry->get_author() == song->get_author())
	  if (abs(queuedsong->getPos()-position)<=Config::get_authorDecay()) return true;
	  if (entry->get_file() == song->get_file()) return true;);
  return false;
}


void SongSelectorLogic::filterProposedList(Song ** list, int &count, int position)
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

void SongSelectorLogic::queueCopySongs()
{
  QMutexLocker _ml(&lock);
  ;
  QString target = QFileDialog::getExistingDirectory(NULL,this,NULL,"Specify directory to dump songs to");
  if (target.isEmpty()) return;
  FOREACH(queue, QueuedSong *, qs,
	  Song* s = qs->getSong();
	  if (s) start_cp("./music/"+s->get_file(),target));
}

void SongSelectorLogic::queueRandom(bool userChoice)
{
  QMutexLocker _ml(&lock);
  
  SongMetriek * metriek = NULL;
  // this loop will in every iteration try to find an 
  // undefined element and then assign a value to it
  // if there are no longer undefined elements, the 
  // loop breaks.
  Song * firstiflacking = ::monitor_song;
  if (!firstiflacking) firstiflacking = ::main_song;
  while(true)
    {
      // find before, element and after
      QueuedSong *before =  NULL;
      QueuedSong *after = NULL;
      QueuedSong *element = NULL;
      FOREACH(queue, QueuedSong *, entry,
	      if (!element && entry->isSelected()) element = entry;
	      if (entry->ankered())
                {
		  if (element)
		    {
		      after = entry;
		      break;
		    }
		  else before = entry;
		});
      // check boundaries
      if (!element)
	break;
      // deselecting the element will make sure that this loop will always end, even if no filler song  can be found.
      element->setSelected(false);
      if (element->ankered())
	continue;
      
      // we now see which songs are the two endpoints;
      Song *a = before ? before->getSong() : firstiflacking;
      float A = before ? before->getPos()  : 0;
      if (!a)
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
      Song *c = after->getSong();
      float C = after->getPos();
      
      // find a small set of closest friends
      float B = element->getPos();
      // we take the standard song metric
      
      metriek = &SongMetriek::std;

      // find a song close to the target position
      int count;
      Song * * proposed = database->closestSongs(this, a, B-A, c, C-B, metriek, 100, count);
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
	      count = 20;
	      for(int i = 0 ; i < count ; i ++)
		songs -> insertItem ( proposed[i]->get_title()+"["+proposed[i]->get_author()+"]", i, i);
	      int result = songs->exec();
	      if (result>=0 && result < count)
		choice = proposed[result];
	    }
	  else
	    {
	      choice = proposed[0];
	    }
	  bpmdj_deallocate(proposed);
	  if (choice)
	    element->setSong(choice,choice->distance(a,B-A,c,C-B,metriek));
	  else
	    element->setSong(choice,0);
	}
    }
  queueOrder();
}

void SongSelectorLogic::selectAlbumItem(QListViewItem* i)
{
  QMutexLocker _ml(&lock);
  
  AlbumItem *album = (AlbumItem*)i;
  if (!album) return;
  if (album->get_song()) return;
  songList->clearSelection();
  // traverse through all children and find them in the songlist...
  AlbumItem *child = (AlbumItem*)album->firstChild();
  GrowingArray<Song*> * all_songs = database->getAllSongs();
  while(child)
    {
      QString file = child->text(ALBUM_FILE);
      for ( int ss = 0 ; ss < all_songs->count ; ss ++)
	{
	  if (file == all_songs->elements[ss]->get_file())
	    new QueuedSong(all_songs->elements[ss],queue);
	}
      child = (AlbumItem*)child->nextSibling();
    }
}

void SongSelectorLogic::renameAlbumItem(QListViewItem* i)
{
  QMutexLocker _ml(&lock);
  
  if (!i) return;
  AlbumItem *item = (AlbumItem*)i;
  if (!item->get_song()) return;
  item->startRename(0);
}

void SongSelectorLogic::albumItemChanged(QListViewItem* i, int col)
{
  QMutexLocker _ml(&lock);
  
  AlbumItem *item = (AlbumItem*)i;
  if (!item) return;
  Song *song = item->get_song();
  if (!song) return;
  QString file = song->get_storedin();
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
  QMutexLocker _ml(&lock);
  ;
  if (col < TAGS_OR || col > TAGS_NOT) return;
  if (!item) return;
  QString c = item -> text(col);
  item -> setText(col,c == TAG_TRUE ? TAG_FALSE : TAG_TRUE);
  updateItemList();
}

void SongSelectorLogic::deleteSongFromAlbum(AlbumItem * item)
{
  QMutexLocker _ml(&lock);
  ;
  if (!item) return;
  Song *song = item->get_song();
  if (!song) return;
  QString file = song->get_storedin();
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
  ;
  if (e->key() == Qt::Key_Escape) return;
  SongSelector::keyPressEvent(e);
}

void SongSelectorLogic::customEvent(QCustomEvent * e)
{
  if ((int)e->type()==(int)BpmDjCustom)
    {
      BpmDjEvent * E = (BpmDjEvent*)e;
      E->run(this);
    }
}

void SongSelectorLogic::openLogDialog()
{  
  QMutexLocker _ml(&lock);
  LogViewerLogic(this).exec();
}
