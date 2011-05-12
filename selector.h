/****
 BpmDj v3.6: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

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
#include "qvectorview.h"
#include "freq-mapping.h"
#include "process-manager.h"
#include "spectrum-pca.h"
#include "analyzers-manager.h"
#include "do-fragment.h"
#include "ui-selector.h"

#define TAGS_TEXT 0
#define TAGS_OR 1
#define TAGS_AND 2
#define TAGS_NOT 3

extern const QString TAG_TRUE;
extern const QString TAG_FALSE;

class QProgressBar;
class QSong;
class ConfigState;
class FragmentCreated;

class SongSelectorLogic:
  public QDialog,
  public Ui::SongSelector, 
  public ProcessChanged, 
  public AnalyzerChanged
{
  Q_OBJECT
private:
  FrequencyDialog *frequency_dialog;
  ProcessManager *processManager;
  AnalyzersManager * analyzers;
  QTimer *timer;
  int mainTicks;
  Q3PopupMenu *selection;
  Q3PopupMenu *queuemenu;
  QMutex lock;
public:
  QVectorView* songList;
  DataBase *database;
  SongSelectorLogic(QWidget*parent=0);
  virtual ~SongSelectorLogic();
  void initialize_extras();
  void acceptNewSong(Song* song);
  void addTag(const QString tag);
  virtual void resetCounter();
  virtual void updateProcessView(bool main_changed);
  virtual void startAnotherAnalyzer(Song * finished_analyzing, int on_slot);

//--------------------------------------------------------------------------------
// FUNCTIONS TO HELP THE INDEXREADER 
//--------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------

private:
  void parse_tags();
  void insertSongInAlbum(Song*, const QString & a, int nr);
  void deleteSongFromAlbum(AlbumItem *);
  void updateFrequencyMap();
  void toggleAutoItem(int which);
  Q3ListViewItem *filterView(Q3ListViewItem * who, Q3ListViewItem * parent);
  void updateColors();
  void setColor(QColor color);
  void setPlayerColor(QLabel *player, QColor color);
  void songSetAuthor(Song *song, const QString & tag);
  void songAddTag(Song * song, const QString & tag);
  void songDelTag(Song * song, const QString & tag);
  void songEditInfo(Song * song);
  void queueFindAndRename(int oldpos, int newpos);
  void queueOrder();
  void linkViewProp(ConfigState & prop, Q3PopupMenu *menu, QString text);
  void linkAutoProp(ConfigState & prop, Q3PopupMenu *menu, QString text);
public: 
  void reread_and_repaint(Song* song);
#ifdef INCOMPLETE_FEATURES
  virtual void findsimilarnames();
  virtual void findallsimilarnames();
  virtual void doMarkDups();
  virtual void checkDisc();
#endif
public slots:
  virtual void updateItemList();
  virtual void startExistenceCheck();
  virtual void selectAllButTagged();
  virtual void selectAllTags();
  // a signal from the UI to notify a forced switch
  virtual void switchMonitorToMain();
  
  virtual void timerTick();
  virtual void selectSong(int i);
  virtual void doPreferences();
  virtual void openLogDialog();
  virtual void doAbout();
  virtual void doLicense();
  virtual void searchLineEntered();
  virtual void fetchSelection();
  virtual void exportPlayList();
  virtual void doSpectrumPca(bool fulldatabase = false, bool update_process_view = true);
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
  
  // color toggles...
  virtual void toggle_temporange();
  virtual void show_freq_shaping_dialog();
  
  virtual void importSongs();
  virtual void queueAnalysis();
  virtual void toAnalyzeOrNot();
  virtual void doOnlineHelp();
  virtual void doAutoMix();
  virtual void selectionMenu();
  virtual void openQueueMenu();
  virtual void openStatistics();
  virtual void startRenamer();
  
  // queue actions
  virtual void playQueueSong(Q3ListViewItem *);
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
  virtual void playHistorySong(Q3ListViewItem *);
  virtual void savePlayHistory();
  virtual void clearPlayHistory();
  
  // album actions
  virtual void albumItemChanged(Q3ListViewItem*, int col);
  virtual void renameAlbumItem(Q3ListViewItem*);
  virtual void selectAlbumItem(Q3ListViewItem*);
  virtual void keyPressEvent(QKeyEvent*);
  virtual bool eventFilter(QObject*, QEvent*);
  
  // taglist things
  virtual void changeTagList(Q3ListViewItem* item, const QPoint & pos, int col);
  
  // fragment playing
  virtual void playFragment(int);
  void fragmentCreated(FragmentCreated* fc);
  
  // other
  void customEvent(QEvent * e);
};

extern SongSelectorLogic * song_selector_window;
#endif // __loaded__selector_h__
