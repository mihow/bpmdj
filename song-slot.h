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
#ifndef __loaded__song_slot_h__
#define __loaded__song_slot_h__
using namespace std;
#line 1 "song-slot.h++"
#include <Qt/qcolor.h>
#include <Qt/qstring.h>
#include <Qt/qcheckbox.h>
#include <Qt/qcombobox.h>
#include <Qt/qlineedit.h>
#include <Qt/qpushbutton.h>
#include "index.h"
#include "song-process.h"

class Song;
class AnalyzersManager;

class SongSlot: public QObject
{
  Q_OBJECT ;
public:
  enum command_type { empty, standard, xmms };
  enum enabled_type { disabled, enabling, ok, disabling };
  enum state_type   { unchecked, checking, usefull, useless };
  enum kind_type    { player, analyzer };
private:
  kind_type kind;             // player or analyzer ?
  /**
   * the player or analyzer ID
   */
  signed4 id;                 
  QString name;               // the name of this analyzer/player
  command_type cmd;           // the command prefix
  QString remote;             // the remote location of the player, if any
  /**
   * The copysong flag is set whenever the player is remote and 
   * must receive a copy of the song first. When it is set bpmplay will
   * be started with the extra option --copymusic passed along.
   */
  bool    copysong;           //
  enabled_type estate;        // true if the user wants to use this process
  QString text;               // the name of the playing / analyzing song
  Song  * song;               // the song being analyzed/played;
  /**
   * The current running time
   */
  signed4 running_time;   
  /**
   * the accumulated running time
   */
  signed4 total_running_time;
  /**
   * The amount of stopped songs
   */
  signed4 songs_finished; 
  time_t  started_at;         // start_time when running
  /**
   * Whether this one is usable, useless or not yet checked
   */
  state_type state;
private:
  void init();
  void setText(QString t);
  void setEstate(enabled_type state);
  void setUsefullState();
  void setUselessState();
public:
  SongSlot();
  // accessors
  enabled_type enabledState() const 
  {
    return estate;
  };
  void setEnabled(bool enabled = true);
  bool isEnabledOk() const 
  {
    return estate==ok;
  };
  QString getText() const 
  {
    return text;
  };
  bool canRun() const 
  {
    return estate==ok && song==NULL;
  };
  bool isAnalyzer() const 
  {
    return kind == analyzer; 
  };
private: 
  QString getBasicCommand() const;
public:
  command_type getCmd() const 
  {
    return cmd; 
  };
  QString getPlayCommand(Index & matchto, Index & song) const;
  QString getAnalCommand(bool tempo, int bpmtechnique, float8 from, float8 to, 
		 bool spectrum, bool energy, bool rhythm, QString song) const;
  void setAnalyzer() 
    {
      kind = analyzer;
    };
  void setCommand(command_type cmd);
  void setCopyMusic(bool t);
  bool getCopyMusic()
  {
    return copysong;
  }
  QString getRemote() const 
  {
    return remote;
  };
  void setRemote(QString remote);
  bool isEmpty() const 
  {
    return cmd == empty; 
  };
  bool isBusy() const 
  {
    return song != NULL; 
  };
  void setSong(Song * s);
  Song * getSong() const 
  {
    return song;
  };
  void reread();
  int get_running_time() const 
  {
    return running_time; 
  };
  int inc_running_time();
  float4 songs_per_second() const;

  /**
   * The underlying process.
   */
  SongProcess* underlying_process;
  bool    is_free()
  {
    return underlying_process==NULL;
  }
  void    start(Song * song, SongProcess* process);
  /**
   * The stop routine is called by the process manager
   * whenever the underlying process finished.
   */
  void    stop();
  void    setNameId(QString name,int id);
  void    setName(QString name);
  QString getName() const 
  {
    return name;
  };
  int     getId()   const 
  {
    return id;
  };
  // checking of the quality of these process parameters
  void    startChecking();
  void    checkerDied();
  QString getLogName();
 public slots:
  void setup();
 signals:
  void stateChanged();
  void viewChanged();
  void timeChanged();
};

class SongSelectorAnalView: public QCheckBox
{
   Q_OBJECT ;
   SongSlot * song_process;
   AnalyzersManager * anal_processes;
   float4   relative_running_time();
 public:
   SongSelectorAnalView(QWidget * parent, AnalyzersManager * a, SongSlot & s);
 private slots:
   void updateBacking();
 public slots:
   void processChange();
   void colorChange();
 signals:
   void songKilled(Song *);
};

class SongSelectorPlayView: public QCheckBox
{
   Q_OBJECT ;
   SongSlot * song_process;
 public:
   SongSelectorPlayView(QWidget * parent, SongSlot & s);
 private slots:
   void updateBacking();
 public slots:
   void processChange();
};

class SongProcPrefView: public QWidget
{
   Q_OBJECT;
 protected:
   QComboBox   * cmd_box;
   QLineEdit   * name_edit;
   QLineEdit   * remote_edit;
   QCheckBox   * copy_music;
   SongSlot * song_process; 
 private:
   QPushButton * configure_button;
   void update_disable_enable();
 public:
   SongProcPrefView(QWidget * parent, SongSlot & s);
 public slots:
   void commandChanged();
};
#endif // __loaded__song_slot_h__
