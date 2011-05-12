/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle
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
#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlcdnumber.h>
#include <qlineedit.h>
#include <qframe.h>
#include <qheader.h>
#include <qlistview.h>
#include <qsplitter.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qheader.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qframe.h>
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
#include "foreach.h"
#include "qvectorview.h"
#include "tags.h"
#include "common.h"
#include "freq-mapping.h"
#include "memory.h"

#define foreach_qsong(V,B)  for(int V##idx = 0 ; V##idx < QSong::get_song_count() ; V##idx++) { Song * V = QSong::get_songs(V##idx); B ; } 
#define foreach_selected_qsong(V,B) foreach_qsong ( V, if ( QSong::get_selected(V##idx) ) { B ; } ; )

SongSelectorLogic::~SongSelectorLogic()
{
  delete processManager;
  delete database;
}

SongSelectorLogic::SongSelectorLogic(QWidget * parent, const QString name) :
  SongSelector(parent,name)
{
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
  songList->setShowSortIndicator( TRUE );
  songList->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, songList->sizePolicy().hasHeightForWidth() ) );
  songList->setFrameShape( QListView::Panel );
  songList->setResizePolicy( QScrollView::Manual );
  songList->setAllColumnsShowFocus( TRUE );
  
  songListFrameLayout->addWidget( songList, 0, 0 );
  
  connect( songList, SIGNAL( doubleClicked(int) ), this, SLOT( selectSong(int) ) );
  connect( songList, SIGNAL( ctrlReturnPressed(int) ), this, SLOT( selectSong(int) ) );
  connect( songList, SIGNAL( returnPressed(int) ), this, SLOT( selectionMenu() ) );
  connect( songList, SIGNAL( rightButtonPressed(int,const QPoint&,int) ), this, SLOT( selectionMenu() ) );
  
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
  frequency_dialog = NULL;

  // create menu structure
  QMenuBar   * main = new QMenuBar(this);
  QPopupMenu * help = new QPopupMenu(this);
  QPopupMenu * before = new QPopupMenu(this);
  QPopupMenu * file = new QPopupMenu(this);
  selection = new QPopupMenu(this);
  queuemenu = new QPopupMenu(this);
  view  = new QPopupMenu(this);
  autom = new QPopupMenu(this);

  // file menu
  file->insertItem("&Preferences",this,SLOT(doPreferences()));
  file->insertItem("&Switch Monitor To Main",this,SLOT(switchMonitorToMain()));
  file->insertItem("Bpm Mixing Desk", this, SLOT (openBpmMixer()));
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
  coloralreadyplayed_item = view -> insertItem("Color already played titles",this,SLOT(toggle_coloralreadyplayed()));
  colorauthorplayed_item  = view -> insertItem("Color already played authors",this,SLOT(toggle_colorauthorplayed()));
  colorinrange_item       = view -> insertItem("Color songs within tempo range",this,SLOT(toggle_colorinrange()));
  colorsongondisk_item    = view -> insertItem("Color songs not on disk",this,SLOT(toggle_colorsongsnotondisk()));
  colorcues_item          = view -> insertItem("Color songs without cues",this,SLOT(toggle_colorsongswithoutcues()));
  colordcolor_item        = view -> insertItem("Color spectrum distance",this, SLOT(toggle_colordcolor()));
  colorspectrum_item      = view -> insertItem("Color PCA of spectrum",this,SLOT(toggle_colorspectrum()));
  
  // view display part
  view->insertSeparator();
  temporange_item           = view -> insertItem("Limit selection to &Tempo range",this,SLOT(toggle_temporange()));
  onlyuptemporange_item     = view -> insertItem("&Up Tempo Range (0 < BPM < 0.06)",this,SLOT(toggle_onlyuptemporange()));
  onlydowntemporange_item   = view -> insertItem("&Down Tempo Range (-0.06 < BPM < 0)",this,SLOT(toggle_onlydowntemporange()));
  notyetplayed_item         = view -> insertItem("Limit selection to &Non-played songs",this,SLOT(toggle_notyetplayed()));
  onlyondisk_item           = view -> insertItem("Limit selection to &Files on disk",this,SLOT(toggle_onlyondisk()));
  onlyindistance_item       = view -> insertItem("Limit selection to &dColor range",this,SLOT(toggle_onlyindistance()));
  onlynonplayedauthors_item = view -> insertItem("Limit selection to non played authors",this,SLOT(toggle_onlynonplayedauthors()));  

  // view dialog part
  view -> insertSeparator();
  view -> insertItem("Frequency shaping dialog",this,SLOT(show_freq_shaping_dialog()));
  view->setCheckable(true);
  
  // auto menu
  auto_popq_item   = autom->insertItem("Pop queue when monitor empty",this,SLOT(toggle_autopop()));
  auto_askmix_item = autom->insertItem("Ask 'how did the mix go'",this,SLOT(toggle_askmix()));
  auto_mixer_item  = autom->insertItem("Open mixer when starting program",this,SLOT(toggle_openmixer()));
  auto_bpmmixer_item  = autom->insertItem("Open Bpm Mixer when starting program",this,SLOT(toggle_openbpmmixer()));
  autom->setCheckable(true);
  
  // selection menu
  selection->insertItem("&Add tag...",this,SLOT(selectionAddTags()));
  selection->insertItem("&Delete tag...",this,SLOT(selectionDelTags()));
  selection->insertItem("&Edit info...",this,SLOT(selectionEditInfo()));
  selection->insertItem("&Insert in album...",this,SLOT(selectionInsertInAlbum()));
  selection->insertItem("&Analyze Bpm/Spectrum ...",this,SLOT(batchAnalyzing()));
  selection->insertItem("Pca Analysis Sound Color",this,SLOT(doSpectrumPca()));
  selection->insertItem("Cluster Analysis Sound Color",this,SLOT(doSpectrumClustering()));
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
};

void SongSelectorLogic::initialize_using_config()
{
  view->setItemChecked(coloralreadyplayed_item,   Config::color_played);
  view->setItemChecked(colorinrange_item      ,   Config::color_range);
  view->setItemChecked(colorauthorplayed_item ,   Config::color_authorplayed);
  view->setItemChecked(colorsongondisk_item   ,   Config::color_ondisk);
  view->setItemChecked(colorcues_item         ,   Config::color_cues);
  view->setItemChecked(colordcolor_item       ,   Config::color_dcolor);
  view->setItemChecked(colorspectrum_item     ,   Config::color_spectrum);
  view->setItemChecked(onlyondisk_item,           Config::limit_ondisk);
  view->setItemChecked(notyetplayed_item,         Config::limit_nonplayed);
  view->setItemChecked(onlyuptemporange_item,     Config::limit_uprange);
  view->setItemChecked(onlydowntemporange_item,   Config::limit_downrange);
  view->setItemChecked(onlyindistance_item,       Config::limit_indistance);
  view->setItemChecked(onlynonplayedauthors_item, Config::limit_authornonplayed);
  autom->setItemChecked(auto_popq_item,           Config::auto_popqueue);
  autom->setItemChecked(auto_askmix_item,         Config::ask_mix);
  autom->setItemChecked(auto_mixer_item,          Config::open_mixer);
  autom->setItemChecked(auto_bpmmixer_item,       Config::open_bpmmixer);
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

void SongSelectorLogic::toggle_openbpmmixer()
{
  toggleAutoItem(auto_bpmmixer_item);
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

void SongSelectorLogic::findAllAlbums()
{
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
  // do spectrum analaysis
  doSpectrumPca(true,false);
  // update albumlist
  int count;
  GrowingArray<Song*>* songs = database->getAllSongs();
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
      QListViewItem * f = Config::taglist->findItem(i->text(0),0);
      if (f)
	{
	  i->setText(1,f->text(1));
	  i->setText(2,f->text(2));
	  i->setText(3,f->text(3));
	}
      t++;
    }
  Config::taglist = tagList;
  // check which tags were marked in the previous session
  QHeader *h = songList->header();
  for(int i = 0 ; i < Config::header->count() ; i ++)
    {
      int m = Config::header->sectionSize(i);
      songList->setColumnWidth(i,m);
      m = Config::header->mapToIndex(i);
      songList->setSectionPos(i,m);
    }
  Config::header = h;
  updateItemList();
}

void SongSelectorLogic::parse_tags()
{
  if (!Tags::new_tags) return;
  for(int i = 0 ; i < Tags::tag_names.count ; i++)
    addTag(Tags::tag_names.elements[i]);
  Tags::new_tags=false;
}

void SongSelectorLogic::acceptNewSong(Song* song)
{
  database->add(song);
}

static bool alreadygavefilterwarning = false;

void SongSelectorLogic::updateItemList()
{
  parse_tags();
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
	Index main_index(main->get_storedin());
	AvlTree<QString> *file2song = database->getFileTreeRef();
	HistoryField ** after = main_index . get_prev_songs();
	while(after && *after)
	  {
	     char* file = (*after)->file;
	     QString count = "-"+QString::number((*after)->count);
	     QString comment = (*after)->comment;
	     SongSortedByFile *s = (SongSortedByFile*)file2song->search(file);
	     if (s)
	       {
		 new HistorySong(s->song,count,comment,historyList);
		 s->song->getDistance();
	       }
	     after++;
	  }
	HistoryField ** before = main_index . get_next_songs();
	while (before && *before)
	  {
	     char* file = (*before)->file;
	     QString count = "+"+QString::number((*before)->count);
	     QString comment = (*before)->comment;
	     SongSortedByFile *s = (SongSortedByFile*)file2song->search(file);
	     if (s)
	       {
		 s->song->getDistance();
		 new HistorySong(s->song,count,comment,historyList);
	       }
	     before++;
	  }
     }
}

void SongSelectorLogic::timerTick()
{
  mainLCD->display(++mainTicks);
  if (mainTicks==1)
    setColor(Config::color_green_time);
  else if (mainTicks==Config::yellowTime)
    setColor(Config::color_yellow_time);
  else if (mainTicks==Config::orangeTime)
    setColor(Config::color_orange_time);
  else if (mainTicks==Config::redTime)
    setColor(Config::color_red_time);
  processManager->checkSignals();
  
  // auto pop queue
  if (Config::auto_popqueue)
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

void SongSelectorLogic::setColor(QColor color)
{
  if (Config::color_main_window)
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
      tempo1->setText(song->tempo_str());
      color=song->get_color();
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
      tempo2->setText(song->tempo_str());
      color = song->get_color();
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
      tempo3->setText(song->tempo_str());
      color = song->get_color();
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
      tempo4->setText(song->tempo_str());
      color = song->get_color();
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

void SongSelectorLogic::show_freq_shaping_dialog()
{
  if (!frequency_dialog)
    frequency_dialog = new FrequencyDialog();
  if (frequency_dialog->isVisible())
    frequency_dialog->hide();
  else
    frequency_dialog->show();
}

void SongSelectorLogic::updateFrequencyMap()
{
  if (!frequency_dialog || !frequency_dialog->isVisible()) return;
  QPixmap *pm = new QPixmap(spectrum_size,200);
  QPainter p;
  p.begin(pm);
  QRect r(0,0,spectrum_size,200);
  p.fillRect(r,Qt::white);

  Song * song_a = processManager->playing_songs[0];
  Song * song_b = processManager->playing_songs[1];
  
  spectrum_type a = song_a ? song_a->get_spectrum() : no_spectrum;
  spectrum_type b = song_b ? song_b->get_spectrum() : no_spectrum;
  spectrum_freq min_freq = 1000;
  spectrum_freq max_freq = -1000;

  for (int j = 0; j < spectrum_size ; j++)
    {
      if (a!=no_spectrum && b!=no_spectrum 
	  && a[j]>0 && b[j]>0 && b[j]/a[j]>0)
	{
	  float energy_fix = 10.0 * log(b[j]/a[j]);
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
	  if (a!=no_spectrum && b!=no_spectrum
	      && a[j]>0 && b[j]>0 && b[j]/a[j]>0)
	    {
	      float energy_fix = 10.0 * log(b[j]/a[j]);
	      // printf("band %d, %g\n",j,energy_fix);
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

void SongSelectorLogic::openBpmMixer()
{
  if (Config::bpm_mixer_command.isEmpty())
    QMessageBox::message(NULL,"Please insert an appropriate command in the preferences dialog\n");
  else
    spawn(Config::bpm_mixer_command);
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

void SongSelectorLogic::doSpectrumPca(bool fulldatabase, bool update_process_view)
{
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
		data [ written ] [ j + 1 ] = svi -> get_spectrum() [ j ] ;
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
				   data[written][i+1]=svi->get_spectrum()[i];
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
    updateProcessView();
}

void SongSelectorLogic::doSpectrumClustering()
{
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
			 if (svi->get_spectrum()!=no_spectrum)
			 cluster.addPoint(svi));
  // 2. Do the Analysis
  SongMetriek metriek(0.0,1.0);
  Couple * result = cluster.agglomerate(&metriek);
  // 3. Assign Colors
  result->determine_color((float)0,(float)360,0,Config::color_cluster_depth - 1);
  // result->color_clusters_with_size(20,40);
  // result->color_clusters_dw2();

  // 4. Clean Up
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
  deallocate(current_directory);
}

void SongSelectorLogic::fetchSelection()
{
  /* count nr of files to get */
  int selectionCount = 0;
  foreach_selected_qsong(svi,selectionCount ++);
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
  /* write script to disk */
  
  FILE* script=openScriptFile(FETCHFILES);
  fprintf(script,"# writing %d files\n",next);
  fprintf(script,UMOUNT_CDROM);
  fprintf(script,EJECT_CDROM);
  int i = 0;
  char* currentcd=0;
  while(i<next)
    {
      Song* song=toFetch[i++];
      const char* filename=song->get_file();
      char* newcd=strdup(filename);
      char temp[500];
      char* breakat=index(newcd,'/');
      assert(breakat);
      *breakat=0;
      if (currentcd==0 || strcmp(currentcd,newcd)!=0)
	{
	  if (currentcd)
	    {
	      deallocate(currentcd);
	      fprintf(script,UMOUNT_CDROM);
	      fprintf(script,EJECT_CDROM);
	    }
	  currentcd=newcd;
	  fprintf(script,INSERT_CD_DIALOG,newcd);
	  fprintf(script,MOUNT_CDROM);
	}
      else 
	deallocate(newcd);
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
      deallocate(source);
      deallocate(target);
      deallocate(dirc);
      deallocate(filenc);
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
	      deallocate(currentcd);
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
	deallocate(newcd);
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
      song->set_ondisk(true);
      /* free source & target */
      deallocate(source);
      deallocate(target);
      deallocate(dirc);
      deallocate(filenc);
    }
  /* finally umount the last cdrom */
  execute(UMOUNT_CDROM);
}

void SongSelectorLogic::batchAnalyzing()
{
  char tempoLine[2048];
  char spectrumLine[2048];
  char energyLine[2035];
  sprintf(tempoLine,"");
  sprintf(spectrumLine,"");
  sprintf(energyLine,"");
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
  if (bounds->energyAnalyzer->isChecked())
    sprintf(energyLine,"--energy");
  // write out executable batch processing for every line
  FILE* script=openScriptFile(PROCESS_ANALYZERS);
  int count = 0;
  foreach_selected_qsong(svi,count++);
  int nr = 0;
  foreach_selected_qsong(svi,
    svi->realize();
    fprintf(script,"echo ======= %d / %d ==============\n",nr++,count-1);
    fprintf(script,"kbpm-play -q --batch %s %s %s \"%s\"\n",
	    tempoLine,spectrumLine,energyLine,
	    (const char*)svi->get_storedin()));
  fclose(script);
  chmod(PROCESS_ANALYZERS, S_IRUSR | S_IWUSR | S_IXUSR);
  QString hosts = bounds->hostlist->text();
  if (hosts.isEmpty() || hosts.isNull())
    spawn(PROCESS_ANALYZERS);
  else
    spawn("kbpm-batch "+hosts);
}

void SongSelectorLogic::selectSong(int i)
{
  Song * song = QSong::songEssence(i);
  if (!song) return;
  processManager->startSong(song);
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

void SongSelectorLogic::songAddTag(Song *song, const QString & tag)
{
  song->realize();
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
  song->reread();
}

void SongSelectorLogic::insertSongInAlbum(Song * song, const QString & album, int nr)
{
  song->realize();
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
  song->realize();
  QString index = song->get_storedin();
  Index * idx = new Index(index);
  idx->executeInfoDialog();
  if (idx->changed())
    {
      idx->write_idx();
      delete idx;
      song->reread();
      return;
    }
  delete idx;
}

void SongSelectorLogic::songDelTag(Song * song, const QString & tag)
{
  song->realize();
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
  song->reread();
}

void SongSelectorLogic::selectionAddTags()
{
  bool ok;
  QString tags = QInputDialog::getText("Tags"
				       ,"Enter tags to add (seperated by spaces)"
				       ,QLineEdit::Normal,QString::null,&ok,this);
  if (ok && !tags.isEmpty())
    foreach_selected_qsong(svi, songAddTag(svi,tags));
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
      last = albumbox.album->currentText();
      lastnr = albumbox.number->value();
      foreach_selected_qsong(s, insertSongInAlbum(s,last,albumbox.number->value()));
    }
}

void SongSelectorLogic::selectionEditInfo()
{
  foreach_selected_qsong(svi, songEditInfo(svi));
}

void SongSelectorLogic::selectionPlayIn3th()
{
  foreach_selected_qsong(svi,
			 processManager->startExtraSong(2,svi);
			 return);
}

void SongSelectorLogic::selectionPlayIn4th()
{
  foreach_selected_qsong(svi,
			 processManager->startExtraSong(3,svi);
			 return);
}

void SongSelectorLogic::selectionAddQueue()
{
  queue->selectAll(false);
  QueuedSong *last = (QueuedSong*)queue->lastItem();
  if (last) last->setSelected(true);
  foreach_selected_qsong(svi,(new QueuedSong(svi,queue))->setSelected(true));
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
    sprintf(split_arguments, "kbpm-merge --split %s %s %s %s %s\n",
	    (mergerDialog.splitBeforeMix->isChecked() ? "--beforemix" : ""),
	    (mergerDialog.splitMiddleMix->isChecked() ? "--middlemix" : ""),
	    (mergerDialog.splitAfterMix->isChecked() ? "--aftermix" : ""),
	    (mergerDialog.splitAfterTempo->isChecked() ? "--aftertempo" : ""),
	    (mergerDialog.number->isChecked() ? "--number" : ""));
  // now create the script...
  QString old="";
  FILE * out = openScriptFile(PROCESS_MIX);
  fprintf(out,RM BPMMIXED_NAME " automix.log\n");
  
  FOREACH(queue,QueuedSong *, song,
    {
      Song * s = song->getSong();
      s->realize();
      QString n = s->get_storedin();
      if (old=="")
	fprintf(out,"kbpm-merge %s \"%s\"\n",arguments,(const char*)n);
      else
	{
	  fprintf(out,"kbpm-merge %s \"%s\" \"%s\"\n",arguments,(const char*)old,(const char*)n);
	  if (mergerDialog.playMixes->isChecked())
	    fprintf(out,BPLAY MIXDUMP_NAME" &\n");
	}
      old=n;
    });
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
  foreach_selected_qsong(svi,
			 processManager->setMainSong(svi);
			 return);
}

void SongSelectorLogic::selectionDelTags()
{
  bool ok;
  QString tags = QInputDialog::getText("Tags"
				       ,"Enter tags to delete (seperated by spaces)"
				       ,QLineEdit::Normal,QString::null,&ok,this);
  if (ok && !tags.isEmpty())
    foreach_selected_qsong(svi,songDelTag(svi,tags));
}

void SongSelectorLogic::doMarkDups()
{
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
  songList->setSelected(0,true);
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
  Config::open_bpmmixer = autom->isItemChecked(auto_bpmmixer_item);
  // WVB -- misschien is het hier nuttig van onmiddelijk de nodige acties te nemen...
}

void SongSelectorLogic::quitButton()
{
   close();
}

void SongSelectorLogic::selectAllButTagged()
{
  QListViewItemIterator it(tagList);
  while(it.current())
    {
      QListViewItem * c = (it++).current();
      if (c->text(TAGS_TEXT)!=EMPTY)
	c->setText(TAGS_NOT,TAG_TRUE);
    }
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
  FOREACH(queue,QueuedSong *, entry, 
	  if (entry->isSelected())
	  entry->setAnkered(!entry->ankered()));
  queue->repaint();
}

void SongSelectorLogic::queueOrder()
{
  // nu lopen we er nog eens door om ze normaal te nummeren
  int cur = 1;
  FOREACH(queue, QueuedSong * , entry, entry->setPos(cur++));
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
  FOREACH(queue,QueuedSong *, entry, 
	  entry->setPos(cur);
	  cur+=count+1);
  // dan lopen we er door om de extra fields te inserten
  cur = 1;
  FOREACH(queue,QueuedSong *, entry,
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
  FOREACH(queue, QueuedSong *, queuedsong, 
	  Song * entry = queuedsong->getSong();
	  if (!entry) continue;
	  if (weighDuplicateAuthors->isChecked())
	  if (entry->get_author() == song->get_author())
	  if (abs(queuedsong->getPos()-position)<=Config::authorDecay) return true;
	  if (entry->get_file() == song->get_file()) return true;);
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
  if (target.isEmpty()) return;
  FOREACH(queue, QueuedSong *, qs,
	  Song* s = qs->getSong();
	  if (s) vexecute(CP"\"./music/"+s->get_file()+"\" "+target));
}

void SongSelectorLogic::queueRandom(bool userChoice)
{
  SongMetriek * metriek = NULL;
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
      metriek = new SongMetriek(tempoWeight->value(),spectrumWeight->value());
      Point *b = a->percentToward(c, metriek, D);
      // find a song close to the target position
      int count;
      Song * * proposed = database->closestSongs(this, (Song*)b, metriek, 100, count);
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
	  deallocate(proposed);
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
  int count;
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

