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

#include <dirent.h>
#include <qapplication.h>
#include <qlistview.h>
#include <qlcdnumber.h>
#include <qheader.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <stdlib.h>
#include <qptrvector.h>
#include <qprogressbar.h>
#include <qmessagebox.h>
#include <qimage.h>
#include <qcstring.h>
#include <qintcache.h>
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
#include "qsong.h"
#include "magic.h"
#include "embedded-files.h"
#include "histogram-property.h"
#include "smallhistogram-type.h"
#include "avltree.cpp"
#include "growing-array.cpp"
#include "signals-template.cpp"

/*-------------------------------------------
 *         Templates we need
 *-------------------------------------------*/

// types
template class smallhistogram_type<32>;
template class smallhistogram_type<96>;
template class histogram_property<32>;
template class histogram_property<96>;

// signals
template double normalize_abs_max<double>(double*, long);
template double find_abs_max<double>(double*, long);

// trees
template class AvlTree<QString>;
template class Node<QString>;
template class AvlTree<Song*>;
template class Node<Song*>;
template class Sentinel<QString>;
template class Sentinel<Song*>;

// arrays
template class GrowingArray<Song*>;
template class GrowingArray<int>;
template class GrowingArray<BasicProcessManager*>;
template class GrowingArray<QString>;

// qt based templates
template class QIntCache<QPixmap>;
template class QPtrVector<QVectorView::Column>;

/*-------------------------------------------
 *         BpmDj Main Startup
 *-------------------------------------------*/
QApplication *app;
QMutex bpmdj_busy(true);

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
    DirectoryScanner::scan("./",NULL); 
  };
};

/**
 * The overriden loader object will allow us to load the picture from
 * another place
 */
class BpmDjSplash: public Loader
{
public:
  BpmDjSplash();
};

BpmDjSplash::BpmDjSplash(): Loader(NULL,NULL,FALSE,WStyle_Customize | WStyle_Splash)
{
  // read the image from memory
  QByteArray image_data;
  image_data.setRawData(logo_png,logo_png_size);
  QImage image(image_data);
  // when deleting the object we don't want to screw our memory
  image_data.resetRawData(logo_png,logo_png_size);
  QPixmap pixmap(image);
  // set the dialog background
  setPaletteBackgroundPixmap(pixmap);
  progress->setProgress(0,0);
  app->processEvents();
  // show it
  show();
  // make sure it is there
  app->processEvents();
}

const char* programname;
QStatusBar* status = NULL;

int main(int argc, char* argv[])
{
  init_embedded_files();

  programname = argv[0];

  Tags::init();

  QApplication application(argc,argv);
  app = &application;
  
  // 2. read the configuration
  if (!Config::load())
    {
      Error(true,"The old configuration file is no longer supported.\n"
	    "Please remove \".kbpmdj\". Also make sure to read the\n"
	    "release notes because .bib files are no longer supported\n"
	    "and the index file format has changed\n");
      exit(10);
    }
  SongSelectorLogic main_window;
  song_selector_window = & main_window;
  BpmDjSplash * splash = new BpmDjSplash();
  removeAllLog();
  
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
  
  // 1.c checking left over raw files (deze komt laatst omdat tmp_directory het kuiste pad bevat)
  RawScanner raw;
  raw.scan();
  if (!raw.result.isNull())
    if (QMessageBox::warning(NULL,RAW_EXT " files check",
			     "There are some left over "RAW_EXT" files. These are:\n"+raw.result,
			     "Remove", "Ignore", 0, 0, 1)==0)
      removeAllRaw("./");
  
  // 3. read all the files in memory
  main_window.initialize_using_config();
  application.setMainWidget(&main_window);
  
  // create the index in memory
  splash->progress->setEnabled(true);
  IndexReader * indexReader = new IndexReader(splash->progress, splash->reading ,main_window.database, Config::get_file_count());
  Config::set_file_count(indexReader->total_files);
  if (indexReader->total_files == 0)
    QMessageBox::message(NULL,
			 "You have no songs indexed in the database. You can\n"
			 "import songs from the music directory by selecting\n"
			 "'Import Songs' from the Management menu.");
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
  if (Config::open_bpmmixer)
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
			        "(together with some nice words of course :)");
    }
  Config::save();
  return result;
}
