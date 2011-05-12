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


/* TODO 
 * - update van een field. door een nieuw tempo op te geven. 
 *   hmmm.. updates zijn klote, liever een selector in elkaar boxen
 * - tag-edtitor maken, naar gelang nodig...
 * - tags in een listbox dumpen
 * - update van een index-file, nieuw tempo
 *   BPM-counter er in foefelen
 * - update om schrijffouten te verbeteren 
 *   naam en author in de file zelf wegsteken
 */

#include <qtimer.h>
#include <qlistview.h>
#include <dirent.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qmenubar.h>
#include "songselector.h"
#include "scanningprogress.h"
#include "renamer.logic.h"
#include "kbpm-index.h"

#define TIME_YELLOW   120
#define TIME_ORANGE   150
#define TIME_RED      180
#define MAXTAGSPERCOL 29
#define MAXTAGS (MAXTAGSPERCOL*2)

class ProcessManager;
class SongSelectorLogic: public SongSelector
{
   Q_OBJECT
 private:
   ProcessManager *processManager;
   QTimer *timer;
   int mainTicks;
   int yellowTime;
   int orangeTime;
   int redTime;
   SongIndex *dataRoot;
   int filterBpm;
   int nextTagLine;
   QLabel *tagLines[MAXTAGS];
   QCheckBox *tagInclude[MAXTAGS];
   QCheckBox *tagExclude[MAXTAGS];
   QPopupMenu *view;
   QPopupMenu *selection;
   int neglectdirstruct_item;
   int notyetplayed_item;
   int coloralreadyplayed_item;
   int onlytemporange_item;
   int colorinrange_item;
   int onlyondisk_item;
   int colornotondisk_item;
 public:
   static int    itemcount;
   static bool   color_range;
   static bool   color_played;
   static bool   color_notondisk;
 public:
   SongSelectorLogic(QWidget*parent=0,const char*name=0);
   void injectDataSet(SongIndex *data);
   void addTag(const char* tag);
   void findTags(Song* song);
   // timer functions
   void resetCounter();
   // process functions
   void updateProcessView();
   // tag functionality
   void parseTags(char* tags);
 private:
   void updateItemList();
   void toggleItem(int which);
   void flattenFilter(Song* list, QListViewItem* parent);
   void deepFilter(Song* list, QListViewItem* parent);
   void flattenFilter(Song* list, QListView* parent);
   void deepFilter(Song* list, QListView* parent);
   QListViewItem *filterView(QListViewItem * who, QListViewItem * parent);
   bool doFilter(Song * item);
   void setColor(QColor color);
   void songAddTag(QListViewItem * song, const char* tag);
   void songDelTag(QListViewItem * song, const char* tag);
   bool lookfor(const char*filename, Song*cur);
   void checkfile(const char* filename, ScanningProgress * progress);
   void scandir(const char* filename,const char* checkname, ScanningProgress * progress);
   void songEdit(QListViewItem* song);
   const char * askDir();
 protected:
   bool filter(Song * item);
 public slots:
   virtual void selectAllButTagged();
 // a signal from the UI to notify a forced switch
 virtual void switchMonitorToMain();
 
 virtual void timerTick();
 virtual void selectSong(QListViewItem* song);
 virtual void doPreferences();
 virtual void doAbout();
 virtual void doFilterChanged();
 virtual void fetchSelection();
 virtual void checkDisc();
 virtual void exportPlayList();
 virtual void selectionAddTags();
 virtual void selectionDelTags();
 virtual void doMarkDups();
 virtual void quitButton();

   virtual void toggle_neglectdirstruct();
   virtual void toggle_notyetplayed();
   virtual void toggle_coloralreadyplayed();
   virtual void toggle_onlytemporange();
   virtual void toggle_colorinrange();
   virtual void toggle_onlyondisk();
   virtual void toggle_colornotondisk();
   
 virtual void findsimilarnames();
 virtual void findsimilarnames(const char* name, const char* fullname);
 virtual void findallsimilarnames();
 virtual void findallsimilarnamesindir(const char*dirname);
 virtual void importMp3s();
 virtual void measureBpms();
 virtual void doHelp();
 virtual void doOnlineHelp();
 virtual void selectionMenu();
 virtual void findWrongIdxNames();
 virtual void findWrongMp3Names();
 virtual void findWrongNames(const char* dir, RenamerLogic *renamer);
};
