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
#include "info.h"

extern FragmentPlayer  fragmentPlayer;
extern FragmentCreator fragmentCreator;
extern IndexReader     indexReader;
extern SongCopier      songCopier;
template class smallhistogram_type<32>;
template class smallhistogram_type<96>;
template class histogram_property<32>;
template class histogram_property<96>;
template float8 normalize_abs_max<float8>(float8*, long);
template float8 find_abs_max<float8>(float8*, long);

QApplication *app;
QMutex bpmdj_busy(QMutex::Recursive);

class RawScanner: public DirectoryScanner
{
protected:
  virtual void checkfile(const QString dir, 
			 const QString filename)
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
  virtual void checkfile(const QString  fullname, 
			 const QString  filename)
  {
    result+=QString::number(number++)+". "+filename+"\n";
  }
public:
  QString result;
  int number;
  FragScanner() : DirectoryScanner("./fragments",".wav", false),
		  result(""), number(0)
  {
  };
};

class BpmDjSplash: public QDialog, public Ui::Loader
{
public:
  BpmDjSplash(): QDialog(NULL,Qt::SplashScreen)
  {
    setupUi(this);
    // read the image from memory
    QByteArray image_data(logo_png,logo_png_size);
    QImage image(QImage::fromData(image_data));
    // set the dialog background
    QPalette p=palette();
    p.setBrush(backgroundRole(),QBrush(image));
    setPalette(p);
    app->processEvents();
    // show it
    show();
    QTimer * timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()), this, SLOT(close()));
    timer->setSingleShot(true);
    timer->start(10000);
  }
};

const char* programname;
QStatusBar* status = NULL;

void terminate_and_quit(int exitcode)
{
  songCopier.terminate();
  existenceScanner.terminate();
  fragmentPlayer.terminate();
  fragmentCreator.terminate();
  spectrumPca.terminate();
  indexReader.terminate();
  clusterer.terminate();
  aoPool->wait_for_finish();
  exit(exitcode);
}

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
  assert(sizeof(float4)==4);
  assert(sizeof(float8)==8);
  
  init_embedded_files();
  
  programname = argv[0];
  Tags::init();
  dsp_driver::init();
  QApplication application(argc,argv);
  app = &application;
  
  // 2. read the configuration
  if (!Config::load())
    {
      Error(true,"The old configuration file is no longer supported.\n"
	    "Please remove \".bpmdj\". Also make sure to read the\n"
	    "release notes because .bib files are no longer supported\n"
	    "and the index file format has changed\n");
      terminate_and_quit(10);
    }
  SongSelectorLogic main_window;
  selector = & main_window;
  removeAllLog();
  
  // 1.a first check the availability of a number of directories...
  DIR * mdir;
  DIR * idir;
  DIR * fdir;
  do
    {
      mdir = opendir(MusicDir.toAscii().data());
      idir = opendir(IndexDir.toAscii().data());
      fdir = opendir(FragmentsDir.toAscii().data());
      if (mdir == NULL || idir == NULL || fdir == NULL)
	{
	  QDialog sw;
	  Ui::SetupWizard swc;
	  swc.setupUi(&sw);
	  if (sw.exec() == QDialog::Rejected) terminate_and_quit(11);
	}
    }
  while (mdir==NULL || idir==NULL || fdir == NULL);
  
  // 1.c checking left over raw files (deze komt laatst omdat tmp_directory 
  // het juiste pad bevat)
  RawScanner raw;
  raw.scan();
  if (!raw.result.isEmpty())
    if (QMessageBox::warning(NULL,RAW_EXT " files check",
			     "There are some left over "RAW_EXT
			     " files. These are:\n\n"+raw.result,
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
  
  // maybe we could set lastWindowClosed to quit() or something similar.
  // in any case we need to check whether this still works properly then
  // application.setMainWidget(&main_window);
  main_window.show();
  main_window.start_reading_indices();

  /**
   * Starting bpmclock process 
   */
#ifdef EXT_CLOCK
  metronome = new ext_clock();
#else
  metronome = new clock_driver();
#endif
  metronome->init();
  metronome->attach_clock(false);
  
  application.exec();
  if (!Config::get_shown_aboutbox())
    {
      Config::set_shown_aboutbox(true);
      QMessageBox::information(NULL,
       "Support BpmDj",
       "If you use this software regularly, then it would be nice to place\n"
       "a link to http://bpmdj.yellowcouch.org/ on your homepage.\n"
       "(together with some nice words of course :)");
    }
  taglist2config(main_window.tagList);
  
  /**
   * Get the horizontal header here
   */
  QHeaderView *h=main_window.songListView->horizontalHeader();
  if(h) Config::set_header_state(h->saveState());
  Config::save();
  terminate_and_quit(0);
}

/**
 * @mainpage BpmDj
 *
 * This page documents some aspects of the BpmDj code that are worth knowing. 
 * First and foremost is the fact that there exists a subversion repository at 
 * svn+ssh//user@yellowcouch.org/repositories/BpmDj/
 * Of course you need an ssh account which can be provided if necessary. 
 *
 * <b>The Development Tree</b><br>
 * The development tree 
 * of BpmDj is somewhat different from the published (tar.gz) one. In essence, 
 * all the sources are written as c++ or h++ files (as opposed to .cpp and .h 
 * files). Each of those c++/h++ files are then converted to .cpp and .h files,
 * which includes the copyright header and opens the proper namespace. So, if 
 * you want to edit things, you should edit the .c++ and .h++ files. However, 
 * if you want to include a specific header file in a c++ file you should use 
 * the .h file. The development tree should be kept private since it doesn't 
 * contain the proper copyright headers.
 *
 * <b>Debugging</b>
 * This point should be mentioned relatively early. However the background
 * might not be that clear directly. This becomes clear later on. 
 * BpmDj starts a process that will fork off the actual application. This means
 * that gdb-ing the main application will not make it possible for you to 
 * trace calls etcetera. There are two solutions to this. 
 * 
 * 1- Start bpmdj and connect to the second process with gdb. You do this
 *    by looking up the bpmdj pids and take the second of the two processes.
 *    Then start gdb ./bpmdj <pid>
 * 2- Of course that approach doesn't work when bpmdj crashes. In that
 *    case use the bash process limits to ask for a core dump
 *    This is done with ulimit -c 1000000. Once the process crashed and the 
 *    core dumped, you can inspect it with gdb ./bpmdj core.55318 (or whatever
 *    it is called at your setup).
 *
 * <b>Types</b><br> BpmDj relies on types of a specific size. These are 
 * directly reflected with a number such that we know how many bytes that type 
 * takes. For instance unsigned8 is an unsigned long integer of 8 bytes (=64 
 * bits) unsigned4 is an unsigned byte of 4 bytes long (=16 bit). There exist 
 * signed1, signed2, signed4, signed8, unsigned1, unsigned2, unsigned4, 
 * unsigned8 and float4 and float8. When writing source please use these types 
 * since it makes porting BpmDj to different architectures much easier. %To get
 * access to these basic types you can include "types.h"
 *
 * <b>Processes and threads</b> %Process management and Thread management in 
 * BpmDj is complicated. Not because it is designed to be complicated, but 
 * rather because process management brings with it a bunch of concurrency 
 * problems for free. Certainly so because 
 * - X doesn't like concurrent access from different processes
 * - Qt insists on running in the main thread and should never be accessed 
 *   from any other thread 
 * - forking off processes while an ALSA PCM device is open will screw up the 
 *   alsa file descriptors
 * - signalling more than 2 dead process will ensure that one process id
 *   gets lost
 * - etcetera. There was just no end to the process/thread based concurrency
 *   problems.
 * %To avoid these we make a distinction between \ref Processes and \ref ao
 * Active objects (these can be compared to threads that receive and handle
 * messages, except without the concurrency problems). There are few
 * processes. Currently these are
 *
 * - \e bpmdj: the main program, which is the selector
 * - \e bpmdj the overseer thread, which is responsible for starting processes
 *   and tracking their state.
 * - \e bpmplay which is a process that plays one song. Typically two bpmplay
 *   processes are running at the same time.
 * - \e bpmdjraw although this is a simply script, it is forked off from a
 *   bpmplay player. This scripts is responsible for decoding the mp3 files.
 * - \e bpmclock to provide an external clock whenever wanted
 *
 * Active objects in BpmDj:
 * - <em>a song copier</em> (ActiveSongCopier), which will take a collection
 *   of songs and copy them to a target directory.
 * - an existence scanner: ActiveExistenceScanner, which goes through the
 *   music directory and checks which files exist.
 * - a fragment player: ActiveFragmentPlayer, which will play a fragment when
 *   it is ready. The fragment player also opens the appropriate DSP device.
 * - a fragment creator: ActiveFragmentCreator, which keeps track of the
 *   fragments the user might wish to listen to and decodes them. The fragment
 *   creator will spawn off the necessary processes.
 * - a Principal Component Analysis process: ActiveSpectrumPca, which analyzes
 *   the sound color of all songs after all the indices have been read in
 *   memory.
 * - an Index Reader: ActiveIndexReader, which will scan the index directory
 *   and read in the entire database.
 * - a cluster analysis algorithm, ActiveCluster, which runs through a
 *   selection of songs and groups them together based on the current distance
 *   metric.
 * - An active object which keeps track of all existing active objects: the 
 *   ActiveAoTracker.
 * 
 * <b>%Data management</b> BpmDj relies on two techniques to deal with data. The
 * first is a library that deals with reference counting of objects that are 
 * passed along. These are important for the active objects since the reference
 * counting must be threadsafe and deallocation can no longer be expected at a 
 * fixed point in the source. The second aspect of data management is the use 
 * of a \ref data abstraction layer. This is mainly responsible for saving and 
 * loading data to disk through a class called Index. The ActiveIndexReader 
 * takes care to actually read in the database.
 *
 * <b>Code style</b> The coding style for block is that each { or } is placed 
 * on an individual line. Lines are 80 characters long.
 *
 * <b>Write your own compiler</b> Whenever the same task must be written many
 * times it is a good idea to think about writing a compiler that will write 
 * the source for you. This avoids mistakes in the handwritten code and in 
 * general either works like a charm or crashes eveyrwhere. This is 
 * demonstated through the active objects library.
 */
#endif // __loaded__bpmdj_cpp__
