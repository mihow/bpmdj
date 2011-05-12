/****
 BpmDj v3.8: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

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
#ifndef __loaded__bpmdj_cpp__
#define __loaded__bpmdj_cpp__
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
#include "song-copier.h"

extern FragmentPlayer  fragmentPlayer;
extern FragmentCreator fragmentCreator;
extern IndexReader     indexReader;
extern SongCopier      songCopier;

/*-------------------------------------------
 *         Templates we need
 *-------------------------------------------*/
template class smallhistogram_type<32>;
template class smallhistogram_type<96>;
template class histogram_property<32>;
template class histogram_property<96>;
template float8 normalize_abs_max<float8>(float8*, long);
template float8 find_abs_max<float8>(float8*, long);

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
  BpmDjSplash(): QDialog(NULL,NULL,FALSE,Qt::SplashScreen)
  {
    setupUi(this);
    // read the image from memory
    QByteArray image_data;
    image_data.setRawData(logo_png,logo_png_size);
    QImage image(image_data);
    // when deleting the object we don't want to screw up our memory
    image_data.resetRawData(logo_png,logo_png_size);
    QPixmap pixmap(image);
    // set the dialog background
    setPaletteBackgroundPixmap(pixmap);
    app->processEvents();
    // show it
    show();
    QTimer * timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()), this, SLOT(close()));
    timer->start(10000,true);
  }
};

const char* programname;
QStatusBar* status = NULL;

int main(int argc, char* argv[])
{
  assert(sizeof(unsigned1)==1);
  assert(sizeof(unsigned2)==2);
  assert(sizeof(unsigned4)==4);
  assert(sizeof(unsigned8)==8);
  assert(sizeof(signed1)==1);
  assert(sizeof(signed2)==2);
  assert(sizeof(signed4)==4);
  assert(sizeof(signed8)==8);
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
  songCopier.terminate();
  existenceScanner.terminate();
  fragmentPlayer.terminate();
  fragmentCreator.terminate();
  spectrumPca.terminate();
  indexReader.terminate();
  aoPool->wait_for_finish();
  return result;
}

/**
 * @mainpage BpmDj
 *
 * This page documents some aspects of the BpmDj code that are worth knowing. First and foremost is the fact
 * that there exists a svn repository at svn+ssh//user@yellowcouch.org/repositories/BpmDj/
 * Of course you need an ssh account which can be provided if necessary. 
 *
 * <b>The development tree</b><br>
 * The development tree 
 * of BpmDj is somewhat different from the pubished (tar.gz) one. In essence, all the sources are written
 * in c++ and h++ files (as opposed to .cpp and .h files). However, each c++ and h++ file is automatically
 * converted to a cpp and h file which includes the copyright header and opens the proper namespace. So in
 * essence, if you want to edit things, you should edit the c++ and .h++ files. If you want to include 
 * something, you should alwyas use the .cpp or .h file. The development tree should be kept private since
 * it doesn't contain the proper copytight headers.
 *
 * <b>Types</b><br>
 * BpmDj relies on typoes of a specific size. These are directly reflected with a number such that we know 
 * how many bytes that type takes. For instance unsigned8 is an unsigned long integer of 8 bytes (=64 bits)
 * unsigned4 is an unsigned byte of 4 bytes loing (=16 bit). There exists signed1, signed2, signed4, signed8,
 * unsigned1, unisnged2, unsigned4, unsigned8 and float4 and float8. When writing source please use these 
 * types since it makes porting BpmDj to different architectures much easier. To get access to these
 * basic types you can include "types.h"
 *
 * <b>Code style</b>
 * The coding style for block is that each { or } is placed on an individual line.
 *
 * <b>Includes</b>
 * Includes in headers should be as minimal as possible
 */
#endif // __loaded__bpmdj_cpp__
