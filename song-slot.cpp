/****
 BpmDj v4.2 beta: Free Dj Tools
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
#ifndef __loaded__song_slot_cpp__
#define __loaded__song_slot_cpp__
using namespace std;
#line 1 "song-slot.c++"
#include <cstdio>
#include <iostream>
#include <qlabel.h>
#include <qsizepolicy.h>
#include <qmessagebox.h>
#include "song.h"
#include "song-slot.h"
#include "scripts.h"
#include "embedded-files.h"
#include "version.h"
#include "analyzers-manager.h"
#include "log-viewer.h"
#include "overseer.h"
#include "info.h"

void SongSlot::init()
{
  underlying_process=NULL;
  remote  = "";
  copysong=false;
  cmd = empty;
  estate = disabled;
  text    = "";
  song    = NULL;
  running_time = -1;
  total_running_time = 0;
  started_at = 0;
  songs_finished = 0;
  id = -1;
  name = "";
  state = unchecked;
  kind = player;
}

float4 SongSlot::songs_per_second() const
{
  float4 t = total_running_time;
  if (t==0) return 0;
  float4 s = songs_finished;
  return s/t;
}

SongSlot::SongSlot()
{
  init();
}

void SongSlot::setEstate(enabled_type val)
{
  if (estate!=val)
    {
      estate=val;
      emit stateChanged();
      emit viewChanged();
    }
}

void SongSlot::setEnabled(bool val)
{
  /**
   * Enabling of this player
   */
  if (val)
    {
      if (estate==disabled)
	{
	  if (isEmpty())
	    {
	      QMessageBox::warning(NULL,getName(), 
			   QString("You want to enable ")
			   +getName()+" but it has no suitable\n"
			   "command. Please fill in the missing command\n"
			   "in the preferences dialog\n");
	      emit stateChanged();
	      emit viewChanged();
	      if (kind==analyzer)
		Config::open_ui(1);
	      else
		Config::open_ui(0);
	    }
	  else
	    {
	      setEstate(enabling);
	      startChecking();
	    }
	}
    }
  else 
    /**
     * Disabling of this player
     */
    {
      if (estate==disabled || estate==disabling) 
	return;
      else if (estate==ok)
	{
	  if (isBusy())
	    setEstate(disabling);
	  else
	    {
	      setEstate(disabled);
	      assert(!song);
	    }
	}
      else
	assert(0);
    }
}

void SongSlot::setRemote(QString r)
{
  r = r.trimmed();
  if (r==remote) return;
  remote = r;
  emit stateChanged();
}

void SongSlot::setName(QString n)
{
  if (name!=n)
    {
      name = n;
      emit viewChanged();
    }
}

void SongSlot::setCommand(command_type c)
{
  if (c==cmd) return;
  cmd=c;
  if (cmd==empty) setEnabled(false);
  emit stateChanged();
}

void SongSlot::setCopyMusic(bool t)
{
  if (t==copysong) return;
  copysong=t;
  emit stateChanged();
}

QString SongSlot::getBasicCommand() const
{
  assert(cmd==standard);
  QString result("bpmplay ");
  if (!remote.isEmpty())
    {
      result +="--remote "+remote+" ";
      if (copysong)
	result += "--copymusic ";
    }
  if (!name.isEmpty())
    result +="--config "+name+" ";
  result +="--verbose ";
  return result;
}

QString SongSlot::getPlayCommand(Index& match_with_index, 
				 Index & to_play_song) const
{
  if (cmd==xmms)
    {
      return QString("xmms -e music/")
	+escape(to_play_song.get_filename()) + QString(" &");
    }
  else if (cmd==standard)
    {
      QString match_with = match_with_index.get_storedin();
      QString song_name  = to_play_song.get_storedin();
      QString c1 = escape(match_with);
      QString c2 = escape(song_name);
      QString result = getBasicCommand();
      result += c2+" -m "+c1;
      return result;
    }
  assert(0);
}

QString SongSlot::getAnalCommand(bool tempo, int technique, 
				 float8 from, float8 to, 
				 bool spectrum, bool energy, 
				 bool rhythm, QString  song) const
{
  assert(cmd==standard);
  QString tempoLine = "";
  if (tempo)
    {
      char frombound[500], tobound[500];
      frombound[0]=tobound[0]=0;
      if (from)
	sprintf(frombound,"--low %g",from);
      if (to)
	sprintf(tobound,"--high %g",to);
      tempoLine=QString("--bpm ")
	+QString::number(technique)
	+OneSpace
	+QString(frombound)+OneSpace+QString(tobound);
    }
  
  QString spectrumLine = spectrum ? "--spectrum" : "";
  QString energyLine = energy ? "--energy" : "";
  QString rhythmLine = rhythm ? "--rhythm" : " ";
  
  return 
    getBasicCommand()+
    QString(" -q --batch ") + tempoLine+ " "+
    spectrumLine+" "+
    energyLine+" "+
    rhythmLine+" "+
    escape(song);
}

void SongSlot::setSong(Song * s)
{
  if (s==song) return;
  song = s;
  if (song)
    setText(song->getDisplayTitle());
  else
    setText("");
};

void SongSlot::reread()
{
  if (!song) return;
  song->reread();
  setText(song->getDisplayTitle());
  emit viewChanged();
}

int SongSlot::inc_running_time()
{
  if (running_time>=0)
    {
      int rt = running_time++;
      emit timeChanged();
      return rt;
    }
  return -1;
}

void SongSlot::start(Song * s, SongProcess* p)
{
  running_time=0;
  started_at=time(NULL);
  setSong(s);
  assert(!underlying_process);
  underlying_process=p;
}

/**
 * When the process finished we also disable
 * the process whenever we were in a disabling mood.
 */
void SongSlot::stop()
{
  if (underlying_process)
    delete underlying_process;
  underlying_process=NULL;
  songs_finished++;
  running_time = - 1;
  total_running_time+=time(NULL)-started_at;
  setSong(NULL);
  if (estate==disabling)
    setEstate(disabled);
}

void SongSlot::setup()
{
  if (cmd==standard)
    spawn((getBasicCommand()+ " --setup").toAscii(),("Setup "+getName()).toAscii());
  else
    Error(true,"A non standard player cannot be configured");
}

void SongSlot::setText(QString t)
{
  if (text==t) return;
  text = t;
  emit viewChanged();
}

void SongSlot::setNameId(QString n,int i)
{
  id = i;
  name = n;
}

QString SongSlot::getLogName()
{
  return kind == player  ? 
    "player#"   + QString::number(id) : 
    "analyzer#" + QString::number(id) ;
}

/**
 * The checking process will create a local file send it to the receiving
 * side and try to decode it. If some correct timing information returns we 
 * know that the communication succeeded and that the decoding process worked.
 * The receiving side will also be called with a verify argument to check which
 * players exist and what version they are...
 */
class CheckersManager: public DeathProcessListener
{
  virtual void process_died(SongSlot* sp)
  {
    sp->checkerDied();
  }
} checkers;

void SongSlot::startChecking()
{
  if (cmd!=standard)
    {
      setUsefullState();
      return;
    }
  if (state==checking) return;
  state = checking;

  // choose a filename & logname
  QString basename = QString::number((intptr_t)this,16);
  QString filename = basename+".ogg";
  QString indexname = basename+".idx";
  QString path = QString("music/")+filename;

  // create a sound on disk
  FILE * f = fopen(path.toAscii().data(),"wb");
  static bool shown_music_error = false;
  if (!f)
    {
      if (!shown_music_error)
	{
	  QMessageBox::critical(NULL,"Music Directory",
		"The root of the music directory should be writable by BpmDj\n"
		"Otherwise BpmDj cannot check players nor analyzers for their "
		"proper working");
	  shown_music_error = true;
	}
      setUselessState();
      return;
    }

  int written = fwrite(bpmdj_ogg,1,bpmdj_ogg_size,f);
  assert(written==bpmdj_ogg_size);
  fclose(f);

  // create an appropriate index file
  Index index;
  index.set_filename(filename);
  index.set_storedin(indexname);
  index.write_idx();

  // create a suitable command
  QString command;
  QString description = "Checking " + name;
  if (kind==player)
    command = getPlayCommand(index,index);
  else
    command = getAnalCommand(false,1,90,160,false,true,false,
			     index.get_storedin());
  command += " --check-version "VERSION;
  
  // start it
  SongProcess* proc=new SongProcess(this,&checkers);
  proc->start(command, description, false);
}

void SongSlot::checkerDied()
{
  assert(state==checking);

  /**
   * check the configuration file and decide whether it worked 
   * out well or not...
   */
  QString basename = QString::number((intptr_t)this,16);
  QString filename = basename+".ogg";
  QString indexname = basename+".idx";
  QString path = QString("music/")+filename;
  // delete sound from disk
  remove(path);
  // read the index file
  {
    Index index(indexname);
    if (index.get_time().isEmpty())
      {
	setUselessState();
	LogViewer log;
	log.goTo(getLogName());
	log.exec();
      }
    else
      setUsefullState();
  }
  // remove index file
  remove(indexname);
}

void SongSlot::setUselessState()
{
  state = useless;
  if (estate==enabling)
    setEstate(disabled);
}

void SongSlot::setUsefullState()
{
  state = usefull;
  if (estate==enabling)
    setEstate(ok);
}

//----------------------------------------------------------
// The view in the analyzer
//----------------------------------------------------------
SongSelectorAnalView::SongSelectorAnalView(QWidget * parent, 
AnalyzersManager * processes, SongSlot & proc) :
QCheckBox(parent), song_process(&proc)
{
  setTristate(true);
  setAutoFillBackground(true);
  QSizePolicy policy = sizePolicy();
  policy.setHorizontalPolicy(QSizePolicy::Expanding);
  setSizePolicy(policy);
  processChange();
  connect(song_process,SIGNAL(viewChanged()),this,SLOT(processChange()));
  connect(song_process,SIGNAL(timeChanged()),this,SLOT(colorChange()));
  connect(this,SIGNAL(stateChanged(int)),this,SLOT(updateBacking()));
  anal_processes = processes;
}


void SongSelectorAnalView::updateBacking()
{
  // if the backing is being processed then we stop the set state
  if (song_process->enabledState()!=SongSlot::enabling)
    song_process->setEnabled(isChecked());
  else
    processChange();
}

void SongSelectorPlayView::updateBacking()
{
  if (song_process->enabledState()!=SongSlot::enabling)
    song_process->setEnabled(isChecked());
  else
    processChange();
}

void SongSelectorAnalView::processChange()
{
  QString text="";
  switch(song_process->enabledState())
    {
    case SongSlot::disabled: 
      QCheckBox::setCheckState(Qt::Unchecked);      
      break;
    case SongSlot::enabling: 
      text="Switching on ";
      QCheckBox::setCheckState(Qt::PartiallyChecked); 
      break;
    case SongSlot::disabling: 
      text="Waiting for  ";
      QCheckBox::setCheckState(Qt::PartiallyChecked); 
      break;
    case SongSlot::ok:
      QCheckBox::setCheckState(Qt::Checked);       
      break;
    default: 
      assert(0);
    }
  if (song_process->getText().isEmpty())
    text+=song_process->getName();
  else
    text+=song_process->getText();
  QCheckBox::setText(text);
  colorChange();
}

float4 SongSelectorAnalView::relative_running_time()
{
  float4 r = song_process->get_running_time();
  float4 report_time = song_process->songs_per_second();
  if (report_time==0) report_time=60;
  else report_time=60/report_time;
  r/=report_time;
  if (r>1.0) r = 1.0;
  if (r<0) r = 1.0;
  return r;
}

void SongSelectorAnalView::colorChange()
{
  /**
   * Checks the timing information for
   * and alters the saturation and intensity of 
   * the related analyzer tab
   */
  QColor c;
  float4 f = relative_running_time();
  c.setHsv(song_process->getId()*240/7,
	   255-(int)(f*255.0),
	   255-(int)(f*127.0));

  QPalette palette;
  palette.setColor(backgroundRole(), c);
  setPalette(palette);
}

//----------------------------------------------------------
// The player view in the song selector
//----------------------------------------------------------
SongSelectorPlayView::SongSelectorPlayView(QWidget * parent, SongSlot & proc) :
  QCheckBox(parent), song_process(&proc)
{
  setTristate(true);
  setAutoFillBackground(true);
  QSizePolicy policy = sizePolicy();
  policy.setHorizontalPolicy(QSizePolicy::Expanding);
  setSizePolicy(policy);
  processChange();
  connect(song_process,SIGNAL(viewChanged()),this,SLOT(processChange()));
  connect(this,SIGNAL(stateChanged(int)),this,SLOT(updateBacking()));
}

void SongSelectorPlayView::processChange()
{
  QString text="";
  switch(song_process->enabledState())
    {
    case SongSlot::disabled:  
      QCheckBox::setCheckState(Qt::Unchecked);      
      break;
    case SongSlot::enabling:  
      text="Switching on ";
      QCheckBox::setCheckState(Qt::PartiallyChecked); 
      break;
    case SongSlot::disabling: 
      text="Waiting for ";
      QCheckBox::setCheckState(Qt::PartiallyChecked); 
      break;
    case SongSlot::ok:
      QCheckBox::setCheckState(Qt::Checked);
      break;
    default: 
      assert(0);
    }
  Song * song = song_process->getSong();
  if (song_process->getText().isEmpty())
    text+=song_process->getName();
  else
    {
      if (song && song->get_tempo().valid())
	text+=QString::number(((int)(song->get_tempo().tempo*100))/100.0)+" ";
      text+=song_process->getText();
    }
  QCheckBox::setText(text);
  QColor color;
  if (song) color = song->get_color();
  else color.setHsv(0,0,128);
  if (!color.isValid() || color.value()<127) color=QColor(255,255,255);
  
  QPalette palette;
  palette.setColor(backgroundRole(), color);
  setPalette(palette);
}

//----------------------------------------------------------
// The widgets in the preference box
//----------------------------------------------------------
SongProcPrefView::SongProcPrefView(QWidget * parent, SongSlot & proc): 
  QWidget(parent)
{
  QHBoxLayout * layout=new QHBoxLayout();
  song_process = & proc;
  cmd_box = new QComboBox(this);
  if (proc.isAnalyzer())
    {
      cmd_box->insertItem(SongSlot::empty,"disabled");
      cmd_box->insertItem(SongSlot::standard,"enabled");
    }
  else
    {
      cmd_box->insertItem(SongSlot::empty,"disabled");
      cmd_box->insertItem(SongSlot::standard,"standard");
      cmd_box->insertItem(SongSlot::xmms,"xmms");
    } 
  cmd_box->setCurrentIndex(song_process->getCmd()); 
  layout->addWidget(cmd_box);
  layout->addWidget(new QLabel("name",this));
  name_edit = new QLineEdit(song_process->getName(),this);
  layout->addWidget(name_edit);
  layout->addWidget(new QLabel("host",this));
  remote_edit = new QLineEdit(song_process->getRemote(),this);
  layout->addWidget(remote_edit);
  copy_music = new QCheckBox("copy song",this);
  copy_music->setChecked(song_process->getCopyMusic());
  layout->addWidget(copy_music);

  configure_button = new QPushButton("Configure",this);
  layout->addWidget(configure_button);

  update_disable_enable();
  connect(remote_edit, SIGNAL(textChanged(const QString &)),
	  this, SLOT(commandChanged()));
  connect(name_edit, SIGNAL(textChanged(const QString &)),
	  this, SLOT(commandChanged()));
  connect(cmd_box, SIGNAL(activated(int)), this, SLOT(commandChanged()));
  connect(copy_music, SIGNAL(stateChanged(int)),
	  this, SLOT(commandChanged()));
  connect(configure_button, SIGNAL(clicked()), song_process, SLOT(setup()));

  setLayout(layout);
}

void SongProcPrefView::update_disable_enable()
{
  bool enable = cmd_box->currentIndex()==SongSlot::standard;
  name_edit->setEnabled(enable);
  remote_edit->setEnabled(enable);
  if (enable)
    copy_music->setEnabled(!remote_edit->text().isEmpty());
  else
    copy_music->setEnabled(false);
}

void SongProcPrefView::commandChanged()
{
  song_process->setName(name_edit->text());
  song_process->setRemote(remote_edit->text());
  song_process->setCommand((SongSlot::command_type)cmd_box->currentIndex());
  song_process->setCopyMusic(copy_music->checkState()==Qt::Checked);
  update_disable_enable();
}
#endif // __loaded__song_slot_cpp__
