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

#ifndef SONGSELECTOR_LOGIC_H
#define SONGSELECTOR_LOGIC_H

#include <qtimer.h>
#include <qlistview.h>
#include <dirent.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qmutex.h>
#include <qmenubar.h>
#include "songselector.h"
#include "song.h"
#include "config.h"
#include "database.h"
#include "albumitem.h"
#include "qvectorview.h"
#include "freq-mapping.h"
#include "process-manager.h"
#include "analyzers-manager.h"

#define TAGS_TEXT 0
#define TAGS_OR 1
#define TAGS_AND 2
#define TAGS_NOT 3

extern const QString TAG_TRUE;
extern const QString TAG_FALSE;

class QProgressBar;
class QSong;
class ConfigState;

class SongSelectorLogic: 
public SongSelector, public ProcessChanged, public AnalyzerChanged
{
    Q_OBJECT
  private:
    FrequencyDialog *frequency_dialog;
    ProcessManager *processManager;
    AnalyzersManager * analyzers;
    QTimer *timer;
    int mainTicks;
    QPopupMenu *selection;
    QPopupMenu *queuemenu;
    QMutex lock;
  public:
    QVectorView* songList;
    DataBase *database;
    // display
    SongSelectorLogic(QWidget*parent=0, const QString name=0);
    virtual ~SongSelectorLogic();
    void initialize_extras();
    void acceptNewSong(Song* song);
    void addTag(const QString tag);
    // process functions
    virtual void resetCounter();
    virtual void updateProcessView(bool main_changed);
    // batch analyzer functions
    virtual void startAnotherAnalyzer(Song * finished_analyzing, int on_slot);
    // tag functionality
    void initialize_using_config();
  private:
    void parse_tags();
    void insertSongInAlbum(Song*, const QString & a, int nr);
    void deleteSongFromAlbum(AlbumItem *);
    void updateFrequencyMap();
    void toggleAutoItem(int which);
    QListViewItem *filterView(QListViewItem * who, QListViewItem * parent);
    void updateColors();
    void setColor(QColor color);
    void setPlayerColor(QLabel *player, QColor color);
    void songSetAuthor(Song *song, const QString & tag);
    void songAddTag(Song * song, const QString & tag);
    void songDelTag(Song * song, const QString & tag);
    void songEditInfo(Song * song);
    void queueFindAndRename(int oldpos, int newpos);
    void queueOrder();
    void linkViewProp(ConfigState & prop, QPopupMenu *menu, QString text);
    void linkAutoProp(ConfigState & prop, QPopupMenu *menu, QString text);
  public: 
    void reread_and_repaint(Song* song);
#ifdef INCOMPLETE_FEATURES
    virtual void findsimilarnames();
    virtual void findallsimilarnames();
    virtual void doMarkDups();
#endif
  public slots:
    void updateItemList();
    void start_spectrum_pca();
    void start_existence_check();
    virtual void selectAllButTagged();
    virtual void selectAllTags();
    // a signal from the UI to notify a forced switch
    virtual void switchMonitorToMain();
    
    virtual void timerTick();
    virtual void selectSong(int i);
    virtual void doPreferences();
    virtual void openMixer();
    virtual void openLogDialog();
    virtual void openBpmMixer();
    virtual void openRecorder();
    virtual void openRecordMixer();
    virtual void openReplay();
    virtual void doAbout();
    virtual void doLicense();
    virtual void searchLineEntered();
    virtual void fetchSelection();
    virtual void checkDisc();
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
    virtual void playQueueSong(QListViewItem *);
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
    virtual void playHistorySong(QListViewItem *);
    virtual void savePlayHistory();
    virtual void clearPlayHistory();

    // album actions
    virtual void albumItemChanged(QListViewItem*, int col);
    virtual void renameAlbumItem(QListViewItem*);
    virtual void selectAlbumItem(QListViewItem*);
    virtual void keyPressEvent(QKeyEvent*);
    virtual bool eventFilter(QObject*, QEvent*);

    // taglist things
    virtual void changeTagList(QListViewItem* item, const QPoint & pos, int col);

    // other
    void customEvent(QCustomEvent * e);
};

extern SongSelectorLogic * song_selector_window;
#endif
