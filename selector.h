/****
 BpmDj v4.1: Free Dj Tools
 Copyright (C) 2001-2010 Werner Van Belle

 http://bpmdj.yellowcouch.org/

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but without any warranty; without even the implied warranty of
 merchantability or fitness for a particular purpose.  See the
 GNU General Public License for more details.

 See the authors.txt for a full list of people involved.
****/
#ifndef __loaded__selector_h__
#define __loaded__selector_h__
using namespace std;
#line 1 "selector.h++"
#include <qtimer.h>
#include <qlistview.h>
#include <dirent.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qmutex.h>
#include <qmenubar.h>
#include "song.h"
#include "config.h"
#include "database.h"
#include "albumitem.h"
#include "ui-freq-mapping.h"
#include "players-manager.h"
#include "spectrum-pca.h"
#include "analyzers-manager.h"
#include "do-fragment.h"
#include "ui-selector.h"

class QProgressBar;
class QSong;
class ConfigState;
class FragmentCreated;

class SongSelectorLogic:
  public QDialog,
  public Ui::SongSelector
{
  Q_OBJECT
private:
  Ui::FrequencyDialog  frequency_content;
  QDialog             *frequency_dialog;
  ProcessManager *processManager;
  AnalyzersManager * analyzers;
  QTimer *timer;
  int mainTicks;
  QMenu *selection;
  QMenu *queuemenu;
  QMutex lock;
public:
  QSong* songList;
  DataBase *database;
  SongSelectorLogic(QWidget*parent=0);
  virtual ~SongSelectorLogic();
  void initialize_extras();
  void acceptNewSong(Song* song);
  void addTag(const QString tag);
  virtual void resetCounter();
  virtual void updateProcessView(bool main_changed);
  virtual void startAnotherAnalyzer(Song * finished_analyzing, int on_slot);

//-----------------------------------------------------------------------
// FUNCTIONS TO HELP THE INDEXREADER 
//-----------------------------------------------------------------------

  /**
   * Will start the indexreader active object, which will go through 
   * all the index files in the index directory and read them in memory. 
   * During this process step_reading_indices will be called regularly.
   * At the end of the process stop_reading_indices will be called.
   */
  void start_reading_indices();
  /**
   * Executed as the response to an event posted by indexreader. 
   * The newsongs vector contains all the newly indexed songs. In return 
   * this function must contact the database which will actually add the
   * songs to the full database, the cache if appropriate and the 
   * visualization collection if appropriate.
   */
  void step_reading_indices(vector<Song*> *newsongs);
  /**
   * Executed as the response to an event posted by the indexReader.
   * It signals the finishing of reading the current index collection.
   */
  void stop_reading_indices(unsigned4 total);
  //--------------------------------------------------------------------------
private:
  void parse_tags();
  void updateFrequencyMap();
  void toggleAutoItem(int which);
  void updateColors();
  void setColor(QColor color);
  void setPlayerColor(QLabel *player, QColor color);
  void songSetAuthor(Song *song, const QString & tag);
  void songAddTag(Song * song, const QString & tag);
  void songDelTag(Song * song, const QString & tag);
  void songEditInfo(Song * song);
  void queueFindAndRename(int oldpos, int newpos);
  void queueOrder();
  QAction* linkViewProp(ConfigState & prop, QMenu *menu, QString text);
  QAction* linkAutoProp(ConfigState & prop, QMenu *menu, QString text);
public: 
  void reread_and_repaint(Song* song);
#ifdef INCOMPLETE_FEATURES
  virtual void findsimilarnames();
  virtual void findallsimilarnames();
  virtual void doMarkDups();
#endif
public slots:
  virtual void updateItemList();
  virtual void startExistenceCheck();
  virtual void selectAllButTagged();
  virtual void selectAllTags();
  // a signal from the UI to notify a forced switch
  virtual void switchMonitorToMain();
  
  virtual void timerTick();
  virtual void startSong(const QModelIndex& i);
  virtual void doPreferences();
  virtual void openLogDialog();
  virtual void doAbout();
  virtual void doLicense();
  virtual void searchLineEntered();
  virtual void exportPlayList();
  virtual void doSpectrumPca(bool fulldatabase = false, 
			     bool update_process_view = true);
  virtual void doClustering();
  virtual void doBackup();
  virtual void selectionAddTags();
  virtual void selectionSetAuthor();
  virtual void selectionPlayIn3th();
  virtual void selectionPlayIn4th();
  virtual void selectionSetMainSong();
  virtual void selectionDelTags();
  virtual void selectionAddQueue();
  virtual void selectionEditInfo();
  virtual void selectionInsertInAlbum();
  virtual void quitButton();
  virtual void avoidNoSongs();
  virtual void avoidSongs();
  
  // color toggles...
  virtual void toggle_temporange();
  virtual void show_freq_shaping_dialog();
  
  virtual void importSongs();
  virtual void queueAnalysis();
  virtual void clearMetaData();
  virtual void toAnalyzeOrNot();
  virtual void doOnlineHelp();
  virtual void doAutoMix();
  virtual void selectionMenu(const QPoint&);
  virtual void openQueueMenu();
  virtual void openStatistics();
  virtual void startRenamerOnSongs();
  virtual void startRenamerOnIndices();
  
  // queue actions
  virtual void playTreeSong(QTreeWidgetItem *);
  virtual void queueShiftUp();
  virtual void queueShiftDown();
  virtual void queueDelete();
  virtual void queueInsert();
  void queueInsert(int count);
  virtual void queueRandom(bool);
  virtual void queueRandom();
  bool rejectQueueProposal(Song * song, int position);
  void filterProposedList(Song ** list, int &count, int position);
  virtual void queueSelectSong();
  virtual void queueCopySongs();
  virtual void queueAnker();
  
  // history actions
  virtual void savePlayHistory();
  virtual void clearPlayHistory();
  
  // album actions
private:
  void insertSongInAlbum(Song*, const QString & a, int nr);
  void deleteSongFromAlbum(AlbumItem *);
public slots:
  virtual void albumItemChanged(QTreeWidgetItem*, int col);
  virtual void selectAlbumItem(QTreeWidgetItem*);
  /**
   * The keypress slot will check whether a delete is pressed
   * while viewing the albumList. If so, it will delete that
   * song from the album.
   */
  virtual void keyPressEvent(QKeyEvent*);
  virtual bool eventFilter(QObject*, QEvent*);

  virtual void sortByColumn(int col, Qt::SortOrder);
  // taglist things
  virtual void changeTagList(QTreeWidgetItem* item, int col);
  
  // fragment playing
  virtual void playFragment(const QModelIndex&);
  void fragmentCreated(FragmentCreated* fc);
  
  // other
  void customEvent(QEvent * e);
};

extern SongSelectorLogic *selector;
#endif // __loaded__selector_h__
