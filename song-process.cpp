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

#include <qlabel.h>
#include <qsizepolicy.h>
#include <qmessagebox.h>
#include "song.h"
#include "song-process.h"
#include "scripts.h"
#include "embedded-files.h"
#include "version.h"
#include "analyzers-manager.h"
#include "log-viewer.logic.h"

void SongProcess::init()
{
  remote  = "";
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

float SongProcess::songs_per_second() const
{
  float t = total_running_time;
  if (t==0) return 0;
  float s = songs_finished;
  return s/t;
}

SongProcess::SongProcess()
{
  init();
}

void SongProcess::setEstate(enabled_type val)
{
  if (estate!=val)
    {
      estate=val;
      emit stateChanged();
      emit viewChanged();
    }
}

void SongProcess::setEnabled(bool val)
{
  if (val)
    {
      if (estate==disabled)
	{
	  if (isEmpty())
	    {
	      QMessageBox::warning(NULL,getName(), 
				   QString("You want to enable ")+getName()+" but it has no suitable\n"
				   "command. Please fill in the missing command\n"
				   "in the preferences dialog\n");
	      emit stateChanged();
	      emit viewChanged();
	      if (kind==analyzer)
		Config::open_ui(2);
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
    if (estate!=disabled)
      {
	setEstate(disabled);
	if (song)
	  stop();
      }
}

void SongProcess::setRemote(QString r)
{
  r = r.stripWhiteSpace();
  if (r==remote) return;
  remote = r;
  emit stateChanged();
}

void SongProcess::setName(QString n)
{
  if (name!=n)
    {
      name = n;
      emit viewChanged();
    }
}

void SongProcess::setCommand(command_type c)
{
  if (c==cmd) return;
  cmd=c;
  if (cmd==empty) setEnabled(false);
  emit stateChanged();
}

void SongProcess::setOldCommand(QString s)
{
  // we now go back and try to figure out
  QString command = s.stripWhiteSpace();
  int idx;
  // what the command prefix is 
  cmd=empty;
  if (command.find("xmms")>=0) cmd=xmms;
  if (command.find("kbpm-play")>=0) cmd=standard;
  if (cmd==empty) return;
  // what the remote host is
  idx = command.find("--remote ");
  remote = QString::null;
  if (idx>=0)
    {
      idx += 9;
      QString host = command.mid(idx);
      host = host.stripWhiteSpace();
      idx = host.find(' ');
      if (idx) host=host.left(idx);
      remote = host.stripWhiteSpace();
    }
  // what the configuration name is
  idx = command.find("--config ");
  if (idx>=0)
    {
      idx += 9;
      QString host = command.mid(idx);
      host = host.stripWhiteSpace();
      idx = host.find(' ');
      if (idx) host=host.left(idx);
      name = host.stripWhiteSpace();
    }
  // print out some convertion information
  printf("command: %s\n  name: %s\n  remote: %s\n  prefix: %d\n",
	 (const char*)command,
	 (const char*)name,
	 (const char*)remote,
	 (int)cmd);
}

QString SongProcess::getBasicCommand() const
{
  assert(cmd==standard);
  QString result("kbpm-play ");
  if (!remote.isEmpty())
    result +="--remote "+remote+" ";
  if (!name.isEmpty())
    result +="--config "+name+" ";
  result +="--verbose ";
  return result;
}

QString SongProcess::getPlayCommand(Index& match_with_index, Index & to_play_song) const
{
  if (cmd==xmms)
    {
      return QString("xmms -e music/")+escape(to_play_song.get_filename()) + " &";
    }
  else if (cmd==standard)
    {
      QString match_with = match_with_index.get_storedin();
      QString song_name  = to_play_song.get_storedin();
      char * c1 = escape(match_with);
      char * c2 = escape(song_name);
      QString result = getBasicCommand();
      result += QString(c2)+" -m "+QString(c1);
      bpmdj_deallocate(c1);
      bpmdj_deallocate(c2);
      return result;
    }
  assert(0);
}

QString SongProcess::getAnalCommand(bool tempo, int technique, double from, double to, bool spectrum, bool energy, bool rythm, QString  song) const
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
      tempoLine=QString("--bpm ")+QString::number(technique)+" "+frombound+" "+tobound;
    }
  
  QString spectrumLine = spectrum ? "--spectrum" : "";
  QString energyLine = energy ? "--energy" : "";
  QString rythmLine = rythm ? "--rythm" : " ";
  
  return 
    getBasicCommand()+
    QString(" -q --batch ") + tempoLine+ " "+
    spectrumLine+" "+
    energyLine+" "+
    rythmLine+" "+
    QString(escape(song));
}

void SongProcess::setSong(Song * s)
{
  if (s==song) return;
  song = s;
  if (song)
    setText(song->getDisplayTitle());
  else
    setText("");
};

void SongProcess::reread()
{
  if (!song) return;
  song->reread();
  setText(song->getDisplayTitle());
  emit viewChanged();
}

int SongProcess::inc_running_time()
{
  if (running_time>=0)
    {
      int rt = running_time++;
      emit timeChanged();
      return rt;
    }
  return -1;
}

void SongProcess::start(Song * s)
{
  running_time=0;
  started_at=time(NULL);
  setSong(s);
}

void SongProcess::stop()
{
  songs_finished++;
  running_time = - 1;
  total_running_time+=time(NULL)-started_at;
  setSong(NULL);
}

void SongProcess::setup()
{
  if (cmd==standard)
    spawn(getBasicCommand()+ " --setup");
  else
    {
      QMessageBox::information(NULL,
			       "Non standard player",
			       "A non standard player cannot be configured",
			       QMessageBox::Ok);
    }
}

void SongProcess::setText(QString t)
{
  if (text==t) return;
  text = t;
  emit viewChanged();
}

void SongProcess::setNameId(QString n,int i)
{
  id = i;
  name = n;
}

QString SongProcess::getLogName()
{
  return kind == player  ? 
    "player#"   + QString::number(id) : 
    "analyzer#" + QString::number(id) ;
}

/**
 * The checking process will create a local file send it to the receiving side and try to decode
 * it. If some correct timing information returns we know that the communication succeeded and
 * that the decoding process worked.
 * The receiving side will also be called with a verify argument ot check which players
 * exist and what version they are...
 */
void SongProcess::startChecking()
{
  if (cmd!=standard)
    {
      setUsefullState();
      return;
    }
  if (state==checking) return;
  state = checking;

  // choose a filename & logname
  QString basename = QString::number((unsigned long)this,16);
  QString filename = basename+".ogg";
  QString indexname = basename+".idx";
  QString path = QString("music/")+filename;

  // create a sound on disk
  FILE * f = fopen((const char*)path,"wb");
  static bool shown_music_error = false;
  if (!f)
    {
      if (!shown_music_error)
	{
	  QMessageBox::critical(NULL,"Music Directory",
				"The root of the music directory should be writeable by bpmdj\n"
				"Otherwise bpmdj cannot check players nor analyzers for their proper working");
	  shown_music_error = true;
	}
      setUselessState();
      return;
    }

  int written = fwrite(noise_ogg,1,noise_ogg_size,f);
  assert(written==noise_ogg_size);
  fclose(f);

  // create an appropriate index file
  Index index;
  index.set_filename(filename);
  index.set_storedin(indexname);
  index.write_idx();

  // create a suitable command
  QString command;
  if (kind==player)
    command = getPlayCommand(index,index);
  else
    command = getAnalCommand(false,1,90,160,false,true,false,index.get_storedin());
  command += " --check-version "VERSION;

  // start it
  checkers.start(this,command, getLogName());
}

void SongProcess::checkerDied()
{
  assert(state==checking);

  // check the config file and decide whether it worked out well or not...
  QString basename = QString::number((unsigned long)this,16);
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
	// dump into the error log
	LogViewerLogic log;
	log.goTo(getLogName());
	log.exec();
      }
    else
      setUsefullState();
  }
  // remove index file
  remove(indexname);
}

void SongProcess::setUselessState()
{
  state = useless;
  if (estate==enabling)
    setEstate(disabled);
}

void SongProcess::setUsefullState()
{
  state = usefull;
  if (estate==enabling)
    setEstate(ok);
}

//----------------------------------------------------------
// The checkers manager
//----------------------------------------------------------
CheckersManager checkers;

CheckersManager::CheckersManager() :
  BasicProcessManager(4+8)
{
  songprocesses = bpmdj_allocate(12,SongProcess*);
  for(int i = 0 ; i < 12 ; i ++)
    songprocesses[i]=NULL;
};

void CheckersManager::clearId(int id)
{
  BasicProcessManager::clearId(id);
  SongProcess * sp = songprocesses[id];
  songprocesses[id]=NULL;
  sp->checkerDied();
}

void CheckersManager::start(SongProcess *sp, QString command, QString log)
{
  for(int i = 0 ; i < 12 ; i++)
    if (!songprocesses[i])
      {
	songprocesses[i]=sp;
	BasicProcessManager::start(i,command, log, false);
	return;
      }
  Fatal("Cannot start a new process since process table is full\n");
}

//----------------------------------------------------------
// The view in the analyzer
//----------------------------------------------------------
SongSelectorAnalView::SongSelectorAnalView(QWidget * parent, AnalyzersManager * processes, SongProcess & proc) :
  QCheckBox(parent,""), song_process(&proc)
{
  setTristate(true);
  QSizePolicy policy = sizePolicy();
  policy.setHorData(QSizePolicy::Expanding);
  setSizePolicy(policy);
  report_time = 60;
  processChange();
  connect(song_process,SIGNAL(viewChanged()),this,SLOT(processChange()));
  connect(song_process,SIGNAL(timeChanged()),this,SLOT(colorChange()));
  connect(this,SIGNAL(stateChanged(int)),this,SLOT(updateBacking()));
  anal_processes = processes;
}

void SongSelectorAnalView::updateBacking()
{
  // if the backing is being processed then we stop the set state
  if (song_process->enabledState()!=SongProcess::enabling)
    song_process->setEnabled(isChecked());
  else
    {
      if (song_process->enabledState()==SongProcess::disabled)
	anal_processes->songKilled(song_process);
      processChange();
    }
}

void SongSelectorAnalView::processChange()
{
  switch(song_process->enabledState())
    {
    case SongProcess::disabled: QCheckBox::setState(QButton::Off); break;
    case SongProcess::enabling: QCheckBox::setState(QButton::NoChange); break;
    case SongProcess::ok: QCheckBox::setState(QButton::On); break;
    default: assert(0);
    }
  QString text = song_process->getText();
  if (text.isEmpty())
    text=song_process->getName();
  QCheckBox::setText(text);
  colorChange();
}

float SongSelectorAnalView::relative_running_time()
{
  float r = song_process->get_running_time();
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
  float f = relative_running_time();
  c.setHsv(song_process->getId()*240/7,
	   255-(int)(f*255.0),
	   255-(int)(f*127.0));
  QCheckBox::setBackgroundColor(c);
}

//----------------------------------------------------------
// The player view in the song selector
//----------------------------------------------------------
SongSelectorPlayView::SongSelectorPlayView(QWidget * parent, SongProcess & proc) :
  QCheckBox(parent,""), song_process(&proc)
{
  setTristate(true);
  QSizePolicy policy = sizePolicy();
  policy.setHorData(QSizePolicy::Expanding);
  setSizePolicy(policy);
  processChange();
  connect(song_process,SIGNAL(viewChanged()),this,SLOT(processChange()));
  connect(this,SIGNAL(stateChanged(int)),this,SLOT(updateBacking()));
}

void SongSelectorPlayView::updateBacking()
{
  if (song_process->enabledState()!=SongProcess::enabling)
    song_process->setEnabled(isChecked());
  else
    processChange();
}

void SongSelectorPlayView::processChange()
{
  switch(song_process->enabledState())
    {
    case SongProcess::disabled: QCheckBox::setState(QButton::Off);      break;
    case SongProcess::enabling: QCheckBox::setState(QButton::NoChange); break;
    case SongProcess::ok:       QCheckBox::setState(QButton::On);       break;
    default:
      assert(0);
    }
  QString text = song_process->getText();
  if (text.isEmpty())
    text=song_process->getName();
  QCheckBox::setText(text);
  Song * song = song_process->getSong();
  QColor color;
  if (song) color = song->get_color();
  else color.setHsv(0,0,128);
  QCheckBox::setBackgroundColor(color);
}

//----------------------------------------------------------
// The widgets in the preference box
//----------------------------------------------------------
SongProcPrefView::SongProcPrefView(QWidget * parent, SongProcess & proc) : QHBox(parent)
{
  song_process = & proc;

  cmd_box = new QComboBox(this);
  cmd_box->insertItem("empty",SongProcess::empty);
  cmd_box->insertItem("standard",SongProcess::standard);
  if (!proc.isAnalyzer())
    cmd_box->insertItem("xmms",SongProcess::xmms);
  cmd_box->setCurrentItem(song_process->getCmd());
  new QLabel("name",this);
  name_edit = new QLineEdit(song_process->getName(),this);
  new QLabel("host",this);
  remote_edit = new QLineEdit(song_process->getRemote(),this);
  configure_button = new QPushButton("Configure",this);
  update_disable_enable();
  connect(remote_edit, SIGNAL(textChanged(const QString &)), this, SLOT(commandChanged()));
  connect(name_edit, SIGNAL(textChanged(const QString &)), this, SLOT(commandChanged()));
  connect(cmd_box, SIGNAL(activated(int)), this, SLOT(commandChanged()));
  connect(configure_button, SIGNAL(clicked()), song_process, SLOT(setup()));
}

void SongProcPrefView::update_disable_enable()
{
  bool enable = cmd_box->currentItem()==SongProcess::standard;
  name_edit->setEnabled(enable);
  remote_edit->setEnabled(enable);
}

void SongProcPrefView::commandChanged()
{
  song_process->setName(name_edit->text());
  song_process->setRemote(remote_edit->text());
  song_process->setCommand((SongProcess::command_type)cmd_box->currentItem());
  update_disable_enable();
}
