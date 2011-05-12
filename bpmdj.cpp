/****
 BpmDj: Free Dj Tools
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
using namespace std;
#line 1 "bpmdj.c++"
#include <qtimer.h>
#include <dirent.h>
#include <qapplication.h>
#include <qlistview.h>
#include <qlcdnumber.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <stdlib.h>
#include <qprogressbar.h>
#include <qmessagebox.h>
#include <qimage.h>
#include "bpmdj.h"
#include "ui-setupwizard.h"
#include "selector.h"
#include "history.h"
#include "index-reader.h"
#include "version.h"
#include "dirscanner.h"
#include "scripts.h"
#include "ui-loader.h"
#include "tags.h"
#include "qsong.h"
#include "magic.h"
#include "embedded-files.h"
#include "histogram-property.h"
#include "smallhistogram-type.h"
#include "signals.h"
#include "fragment-player.h"
#include "fragment-creator.h"

extern FragmentPlayer  fragmentPlayer;
extern FragmentCreator fragmentCreator;
extern IndexReader     indexReader;

/*-------------------------------------------
 *         Templates we need
 *-------------------------------------------*/
template class smallhistogram_type<32>;
template class smallhistogram_type<96>;
template class histogram_property<32>;
template class histogram_property<96>;
template double normalize_abs_max<double>(double*, long);
template double find_abs_max<double>(double*, long);

/*-------------------------------------------
 *         BpmDj Main Startup
 *-------------------------------------------*/
QApplication *app;
QMutex bpmdj_busy(true);

class RawScanner: public DirectoryScanner
{
protected:
  virtual void checkfile(const QString dir, const QString filename)
  {
    result+=QString::number(number++)+". "+filename+"\n";
  }
public:
  QString result;
  int number;
  RawScanner(): DirectoryScanner(".",RAW_EXT,false), result(""), number(0)
  {
  };
};

class FragScanner: public DirectoryScanner
{
protected:
  virtual void checkfile(const QString  fullname, const QString  filename)
  {
    result+=QString::number(number++)+". "+filename+"\n";
  }
public:
  QString result;
  int number;
  FragScanner() : DirectoryScanner("./fragments",".wav",false), result(""), number(0)
  {
  };
};

/**
 * The overriden loader object will allow us to load the picture from
 * another place
 */
class BpmDjSplash: public QDialog, public Ui::Loader
{
public:
  BpmDjSplash();
};

BpmDjSplash::BpmDjSplash(): QDialog(NULL,NULL,FALSE,Qt::SplashScreen)
{
  setupUi(this);
  // read the image from memory
  QByteArray image_data;
  image_data.setRawData(logo_png,logo_png_size);
  QImage image(image_data);
  // when deleting the object we don't want to screw our memory
  image_data.resetRawData(logo_png,logo_png_size);
  QPixmap pixmap(image);
  // set the dialog background
  setPaletteBackgroundPixmap(pixmap);
  app->processEvents();
  // show it
  show();
  QTimer * timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()), SLOT(close()));
  timer->start(10000,true);
}

const char* programname;
QStatusBar* status = NULL;

int main(int argc, char* argv[])
{
  init_embedded_files();
  programname = argv[0];
  Tags::init();
  printf("Created the application\n");
  QApplication application(argc,argv);
  app = &application;
  setup_signals();
  
  // 2. read the configuration
  if (!Config::load())
    {
      Error(true,"The old configuration file is no longer supported.\n"
	    "Please remove \".bpmdj\". Also make sure to read the\n"
	    "release notes because .bib files are no longer supported\n"
	    "and the index file format has changed\n");
      exit(10);
    }
  SongSelectorLogic main_window;
  song_selector_window = & main_window;
  removeAllLog();
  
  // 1.a first check the availability of a number of directories...
  DIR * mdir;
  DIR * idir;
  DIR * fdir;
  do
    {
      mdir = opendir(MusicDir);
      idir = opendir(IndexDir);
      fdir = opendir(FragmentsDir);
      if (mdir == NULL || idir == NULL || fdir == NULL)
	{
	  QDialog sw(0,0,true);
	  Ui::SetupWizard swc;
	  swc.setupUi(&sw);
	  if (sw.exec() == QDialog::Rejected) exit(0);
	}
    }
  while (mdir==NULL || idir==NULL || fdir == NULL);
  
  // 1.c checking left over raw files (deze komt laatst omdat tmp_directory het kuiste pad bevat)
  RawScanner raw;
  raw.scan();
  if (!raw.result.isEmpty())
    if (QMessageBox::warning(NULL,RAW_EXT " files check",
			     "There are some left over "RAW_EXT" files. These are:\n\n"+raw.result,
			     "Remove", "Ignore", 0, 0, 1)==0)
      removeAllRaw("./");
  
  FragScanner frag;
  frag.scan();
  if (!frag.result.isEmpty())
    if (QMessageBox::warning(NULL,"Fragment files check",
			     "There are some left over fragment files.",
			     "Remove", "Ignore", 0, 0, 1)==0)
      start_rm("./fragments/*.wav");
  
  // 4. Some extra version dependent blurb...
  if (!Config::get_shown_aboutbox())
    if (MAGIC_NOW == MAGIC_3_5)
      QMessageBox::message(NULL,"Qt3 -> Qt4 bump:\n"
			   "From this version on the source code has been ported to "
			   "Qt4. This has been a non trivial port so I expect that "
			   "some bugs will show up. Don't hesitate to send detailed "
			   "bug reports to werner@yellowcouch.org or post them at "
			   "http://bpmdj.yellowcouch.org/bugzilla/");
  BpmDjSplash splash;
  application.setMainWidget(&main_window);
  main_window.show();
  main_window.start_reading_indices();
  int result = application.exec();
  if (!Config::get_shown_aboutbox())
    {
      Config::set_shown_aboutbox(true);
      QMessageBox::message(NULL,"If you use this software regularly, then please put a\n"
			   "link to http://bpmdj.yellowcouch.org/ on your homepage\n"
			   "(together with some nice words of course :)");
    }
  taglist2config(main_window.tagList);
  Config::save();
  existenceScanner.terminate();
  fragmentPlayer.terminate();
  fragmentCreator.terminate();
  spectrumPca.terminate();
  indexReader.terminate();
  while(true) sleep(1000);
  return result;
}
