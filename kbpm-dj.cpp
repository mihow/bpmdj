/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2005 Werner Van Belle

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

#include <dirent.h>
#include <qapplication.h>
#include <qlistview.h>
#include <qlcdnumber.h>
#include <qheader.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <stdlib.h>
#include <qprogressbar.h>
#include <qsplashscreen.h>
#include <qmessagebox.h>
#include "kbpm-dj.h"
#include "setupwizard.h"
#include "songselector.logic.h"
#include "history.h"
#include "index-reader.h"
#include "version.h"
#include "dirscanner.h"
#include "scripts.h"
#include "loader.h"
#include "tags.h"

QApplication *app;

class RawScanner: public DirectoryScanner
{
protected:
  virtual void recursing(const QString dirname)
  {
  }
  virtual void scandir(const QString dirname, const QString  checkname)
  {
    if (entered) return;
    entered=true;
    DirectoryScanner::scandir(dirname,checkname);
  }
  virtual void checkfile(const QString  fullname, const QString  filename)
  {
    result+=QString::number(number)+". "+filename+"\n";
    number++;
  }
public:
  QString result;
  bool entered;
  int number;
  RawScanner() : DirectoryScanner(RAW_EXT) 
  {
    result = QString::null;
    entered=false;
    number = 0;
  };
  virtual void scan()
  { 
    DirectoryScanner::scan(Config::get_tmp_directory(),NULL); 
  };
};

/**
 * The overriden loader object will allow us to load the picture from
 * another place
 */
class BpmDjSplash: public QSplashScreen
{
public:
  QProgressBar * progress;
  QLabel       * reading;
  BpmDjSplash(const QPixmap & pm);
};

BpmDjSplash::BpmDjSplash(const QPixmap & pm): QSplashScreen(pm)
{
  progress = new QProgressBar(this);
  progress->setGeometry( QRect( 10, 160, 378, 27 ) );
  progress->setPaletteBackgroundPixmap( pm );
  progress->setBackgroundOrigin( QLabel::ParentOrigin );
  reading = new QLabel( this, "reading" );
  reading->setGeometry( QRect( 12, 136, 376, 19 ) );
  reading->setBackgroundOrigin( QLabel::ParentOrigin );
  reading->setPaletteBackgroundPixmap( pm );
  show();
  app->processEvents();
}

BpmDjSplash * create_splash()
{
  QString logo_name = ".bpmdj-logo.png";
  QPixmap *bpmdj = new QPixmap(logo_name);
  if (bpmdj->isNull() || !Config::get_shown_aboutbox())
    {
      // if we don't have the pixmap we create a window and get it from there
      Loader * loader = new Loader();
      const QPixmap * created = loader->paletteBackgroundPixmap();
      (*bpmdj)=(*created);
      created->save(logo_name,"PNG");
    }
  return new BpmDjSplash(*bpmdj);
}

const char* programname;
QStatusBar* status = NULL;

int main(int argc, char* argv[])
{
  programname = argv[0];

  Tags::init();

  QApplication application(argc,argv);
  SongSelectorLogic main_window;
  app = &application;
  
  // 2. read the configuration
  Config::load();    
  BpmDjSplash * splash = create_splash();

  // 1.a first check the availability of a number of directories...
  DIR * mdir;
  DIR * idir;
  do
    {
      mdir = opendir(MusicDir);
      idir = opendir(IndexDir);
      if (mdir == NULL || idir == NULL)
	{
	  SetupWizard *sw = new SetupWizard(0,0,true);
	  sw->setFinishEnabled(sw->lastpage,true);
	  int res = sw->exec();
	  if (res == QDialog::Rejected)
	    exit(0);
	}
    }
  while (mdir==NULL || idir==NULL);
  
  // 1.b check the availability of programs/the PATH evironment variabe...
  //     we only check the existence of kbpm-play to guide the user. 
  if(!(execute("kbpm-play --check-version "VERSION" 2>/dev/null >/dev/null")))
    {
      QMessageBox::warning(NULL,"Missing kbpm-play",
			   "Could not start kbpm-play.\n" 
			   "Make sure the correct version of the\n"
			   "program is installed in your PATH");
      exit(0);
    }
  
  // 1.c checking left over raw files (deze komt laatst omdat tmp_directory het kuiste pad bevat)
  RawScanner raw;
  raw.scan();
  if (!raw.result.isNull())
    if (QMessageBox::warning(NULL,RAW_EXT " files check",
			     "There are some left over "RAW_EXT" files. These are:\n"+raw.result,
			     "Remove", "Ignore", 0, 0, 1)==0)
      removeAllRaw(Config::get_tmp_directory());  
  
  // 3. read all the files in memory
  main_window.initialize_using_config();
  application.setMainWidget(&main_window);
  
  // create the index in memory
  splash->progress->setEnabled(true);
  IndexReader * indexReader = new IndexReader(splash->progress, splash->reading ,main_window.database);
  Config::set_file_count(indexReader->total_files);
  if (indexReader->total_files == 0)
    QMessageBox::message(NULL,
			 "You have no songs indexed in the database. You can\n"
			 "import songs from the music directory by selecting\n"
			 "'Import Songs' from the Management menu.");
  if (indexReader->idx_files > 1000)
    QMessageBox::message(NULL,"You have more than 1000 songs\n"
			 "in index files. You can compact them\n"
			 "and ensure faster data acess by selecting\n"
			 "Management|Compact Index Directory");
  delete indexReader;
  indexReader = NULL;
  
  // 4. Retrieve tags and spectra
  application.processEvents();
  main_window.initialize_extras();
  // 5. Some extra version dependent blurb...
  if (!Config::get_shown_aboutbox())
    {
      if (MAGIC_NOW == MAGIC_2_1)
	QMessageBox::message(NULL,"The player now supports OSS and ALSA, together\n"
			     "with an extra (obligatory) parameter to specify where the\n"
			     ".raw files are to be stored. Therefore, please go to\n"
			     "Preferences and create suitable player commands");
      if (MAGIC_NOW == MAGIC_2_5)
	QMessageBox::message(NULL,"The selector has now a much better spectrum-matching\n"
			     "algorithm. Please go to Preferences|spectrum to revert it\n"
			     "back to the old (lesser accurate) version.\n"
			     "Also, be sure to look at the fragment sequencer of the player !\n");
      if (MAGIC_NOW == MAGIC_2_7)
	QMessageBox::message(NULL,"This highly improved version of BpmDj can now\n"
			     "analyze echo, rythm and composition characteristics !\n"
			     "Be sure to read the copyright and licensing information.\n");
      if (MAGIC_NOW == MAGIC_2_8)
	QMessageBox::message(NULL,"BpmDj now depends on mplayer instead of mpg123 & ogg123\n"
			     "make sure you have mplayer installed properly.\n");
    }
  
  // show the window immediatelly
  main_window.show();
  app->processEvents();

  // now we can delete the splash stuff
  delete splash;
  splash = NULL;

  // 5b. startup mixers ?
  if (Config::get_open_mixer())
    main_window.openMixer();
  if (Config::get_open_bpmmixer())
    main_window.openBpmMixer();
  
  // 6. start the application
  main_window.start_spectrum_pca();
  main_window.start_existence_check();
  int result = application.exec();
  if (!Config::get_shown_aboutbox())
    {
      Config::set_shown_aboutbox(true);
      QMessageBox::message(NULL,"If you use this software regularly, then please put a\n"
			        "link to http://bpmdj.sourceforge.net/ on your homepage\n"
			        "(together with some nice words of course :) If you don't\n"
			        "have a homepage, it should not be too difficult to create one.");
    }
  Config::save();
  return result;
}
