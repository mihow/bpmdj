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

#ifndef SONGSELECTOR_LOGIC_H
#define SONGSELECTOR_LOGIC_H

#include <qtimer.h>
#include <qlistview.h>
#include <dirent.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qmenubar.h>
#include "songselector.h"
#include "config.h"

class ProcessManager;
class QSong;

class SongSelectorLogic: 
  public SongSelector
{
    Q_OBJECT
  private:
    ProcessManager *processManager;
    QTimer *timer;
    int mainTicks;
    int nextTagLine;
    QLabel *tagLines[MAXTAGS];
    QCheckBox *tagInclude[MAXTAGS];
    QCheckBox *tagExclude[MAXTAGS];
    QPopupMenu *view;
    QPopupMenu *selection;
    // colors
    int coloralreadyplayed_item;
    int colorauthorplayed_item;
    int colorinrange_item;
    int colorsongondisk_item;
    int colorcues_item;
    int colordcolor_item;
    int colorspectrum_item;
    // display
    int notyetplayed_item;
    int onlyuptemporange_item;
    int onlydowntemporange_item;
    int temporange_item;
    int onlyondisk_item;
    int onlyindistance_item;
    int onlyinspectrum_item;
    static int itemcount;
  public:
    SongSelectorLogic(QWidget*parent=0, const QString name=0);
    void findAllTags();
    void addTag(const QString tag);
    bool lookfor(const QString w);
    // timer functions
    void resetCounter();
    // process functions
    void updateProcessView();
    // tag functionality
    void parseTags(QString tags);
    void findsimilarnames(const QString & name, const QString & fullname);
  private:
    void doAbout(int pg);
    void updateItemList();
    void toggleItem(int which);
    QListViewItem *filterView(QListViewItem * who, QListViewItem * parent);
    bool doFilter(QSong * item);
    void setColor(QColor color);
    void setPlayerColor(QGroupBox *player, QColor color);
    void songAddTag(QListViewItem * song, const QString & tag);
    void songDelTag(QListViewItem * song, const QString & tag);
    void songEdit(QListViewItem* song);
  protected:
    bool filter(QSong * item);
  public slots:
    virtual void selectAllButTagged();
    // a signal from the UI to notify a forced switch
    virtual void switchMonitorToMain();
  
    virtual void timerTick();
    virtual void selectSong(QListViewItem* song);
    virtual void doPreferences();
    virtual void doAbout();
    virtual void doLegende();
    virtual void doLicense();
    virtual void doFilterChanged();
    virtual void fetchSelection();
    virtual void checkDisc();
    virtual void exportPlayList();
    virtual void selectionAddTags();
    virtual void selectionPlayIn3th();
    virtual void selectionSetMainSong();
    virtual void selectionDelTags();
    virtual void doMarkDups();
    virtual void quitButton();
    virtual void playersChanged();
    
    // color toggles...
    virtual void toggle_coloralreadyplayed();
    virtual void toggle_colorinrange();
    virtual void toggle_colorauthorplayed();
    virtual void toggle_colorsongsnotondisk();
    virtual void toggle_colorsongswithoutcues();
    virtual void toggle_colordcolor();
    virtual void toggle_colorspectrum();
    
    virtual void toggle_notyetplayed();
    virtual void toggle_onlyuptemporange();
    virtual void toggle_onlydowntemporange();
    virtual void toggle_temporange();
    virtual void toggle_onlyondisk();
    virtual void toggle_onlyindistance();
    virtual void toggle_onlyspectrum();
    
    virtual void findsimilarnames();
    virtual void findallsimilarnames();
    virtual void importMp3s();
    virtual void measureBpms();
    virtual void measureSpectra();
    virtual void doHelp();
    virtual void doOnlineHelp();
    virtual void selectionMenu();
    virtual void findWrongIdxNames();
    virtual void findWrongMp3Names();
};

#endif
