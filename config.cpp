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
#ifndef __loaded__config_cpp__
#define __loaded__config_cpp__
using namespace std;
#line 1 "config.c++"
#include <qlineedit.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <stdlib.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qmessagebox.h>
#include <cstdio>
#include "bpmdj-pref.h"
#include "config.h"
#include "song.h"
#include "song-metric.h"
#include "metric-widget.h"
#include "bpmdj.h"
#include "scripts.h"
#include "magic.h"
#include "info.h"
#include "qt-helpers.h"
#include "tags.h"
#include "qstring-factory.h"

ConfigState::ConfigState(bool val): QObject(), state(val)
{
}

bool ConfigState::isOn() const
{
  return state;
}

QAction* ConfigState::addTo(QMenu * menu, QString menu_text)
{
  item = menu->addAction(menu_text,this,SLOT(toggle()));
  assert(item);
  item->setCheckable(true);
  update();
  return item;
}

void ConfigState::update()
{
  if (item)
    item->setChecked(state);
}

void ConfigState::toggle()
{
  setOn(!state);
}

void ConfigState::set(bool new_state)
{
  state = new_state;
}

void ConfigState::setOn(bool new_state)
{
  state = new_state;
  update();
  emit toggled();
}

init_singleton_var(Config,file_count,int,1000);
init_singleton_var(Config,yellowTime,int,120);
init_singleton_var(Config,orangeTime,int,150);
init_singleton_var(Config,redTime,int,180); 
init_singleton_var(Config,filterBpm,int,0);
init_singleton_var(Config,authorDecay,int,5);
ConfigState Config::color_range(true);
ConfigState Config::color_played(true);
ConfigState Config::color_authorplayed(true);
ConfigState Config::color_ondisk(true);
ConfigState Config::color_cues(true);
ConfigState Config::color_dcolor(true);
ConfigState Config::color_spectrum(true);
ConfigState Config::limit_ondisk(true);
ConfigState Config::limit_nonplayed(false);
ConfigState Config::limit_uprange(false);
ConfigState Config::limit_downrange(false);
ConfigState Config::limit_indistance(false);
ConfigState Config::limit_authornonplayed(false);
ConfigState Config::color_songs_based_on_history(true);
init_singleton_var(Config,shown_aboutbox,bool,false);
// ConfigState Config::open_mixer(false);
init_singleton_var(Config,distance_temposcale,float4,0.06);
init_singleton_var(Config,distance_spectrumweight,float4,0.11);
ConfigState Config::ask_mix(true);
ConfigState Config::auto_popqueue(false);
SongSlot Config::players[4];
//init_singleton_var(Config,mixer_command,QString,"");
init_singleton_var(Config,color_tempo44,QColor,QColor(0,255,0));
init_singleton_var(Config,color_tempo54,QColor,QColor(255,0,0));
init_singleton_var(Config,color_tempo64,QColor,QColor(255,0,0));
init_singleton_var(Config,color_tempo74,QColor,QColor(255,0,0));
init_singleton_var(Config,color_tempo84,QColor,QColor(255,255,0));
init_singleton_var(Config,show_tempo54,bool,false);
init_singleton_var(Config,show_tempo64,bool,false);
init_singleton_var(Config,show_tempo74,bool,false);
init_singleton_var(Config,show_tempo84,bool,true);
init_singleton_var(Config,show_unknown_tempo,bool,true);
init_singleton_var(Config,color_green_time,QColor,QColor(0,255,0));
init_singleton_var(Config,color_yellow_time,QColor,QColor(255,255,0));
init_singleton_var(Config,color_orange_time,QColor,QColor(255,170,0));
init_singleton_var(Config,color_red_time,QColor,QColor(255,0,0));
init_singleton_var(Config,color_main_window,bool,false);
init_singleton_var(Config,log_spectrum_distance,bool,true);
init_singleton_var(Config,color_played_song,QColor,QColor(255,0,0));
init_singleton_var(Config,color_played_author,QColor,QColor(255,170,0));
init_singleton_var(Config,color_unavailable,QColor,QColor(127,127,127));
init_singleton_var(Config,color_dcolor_col,QColor,QColor(255,255,0));
init_singleton_var(Config,color_cluster_depth,int,3);
//init_singleton_var(Config,taglist,QTreeWidget*,NULL);
init_singleton_var(Config,header_state,QByteArray,NULL);
//init_singleton_var(Config,bpm_mixer_command,QString,"kbpm-mix --alsa");
// ConfigState Config::open_bpmmixer(false);
init_singleton_var(Config,distance_tempoweight,float4,0.02);
init_singleton_var(Config,distance_echoweight,float4,0.62);
init_singleton_var(Config,distance_rhythmweight,float4,0.26);
init_singleton_var(Config,distance_compositionweight,float4,0.33);
init_singleton_var(Config,max_songs,int,100);
init_singleton_var(Config,color_unchecked,QColor,QColor(219,219,219));
SongSlot Config::analyzers[8];
init_singleton_var(Config,anal_bpm_from,float4,90);
init_singleton_var(Config,anal_bpm_to,float4,160);
init_singleton_var(Config,anal_bpm_technique,int,1);
init_singleton_var(Config,color_alltime,QColor,QColor(0,0,255));
ConfigState Config::play_fragments(false);
init_singleton_var(Config,tag_include,QString,"");
init_singleton_var(Config,tag_mustbe,QString,"");
init_singleton_var(Config,tag_exclude,QString,"");

void Config::save()
{
  QFile f(".bpmdj");
  f.open(QIODevice::WriteOnly);
  if (!f.isOpen() || !f.isWritable())
    {
      Error(true,"Configuration file \".bpmdj\" cannot be opened for writing");
      return;
    }
  QDataStream s(&f);
  s << (quint16)MAGIC_NOW;
  s << (quint16)get_file_count();
  s << (quint16)get_yellowTime();
  s << (quint16)get_orangeTime();
  s << (quint16)get_redTime();
  s << (quint16)get_filterBpm();
  s << (qint8)color_range;
  s << (qint8)color_played;
  s << (qint8)color_authorplayed;
  s << (qint8)color_ondisk;
  s << (qint8)color_cues;
  s << (qint8)color_dcolor;
  s << (qint8)color_spectrum;
  s << (qint8)get_authorDecay();
  s << (qint8)limit_ondisk;
  s << (qint8)limit_nonplayed;
  s << (qint8)limit_uprange;
  s << (qint8)limit_downrange;
  s << (qint8)limit_indistance;
  for(int i = 0 ; i < 4 ; i ++)
    {
      s << (quint16)players[i].getCmd();
      s << players[i].getName();
      s << players[i].getRemote();
      s << (qint8)analyzers[i].getCopyMusic();
    }
  s << get_distance_temposcale();
  s << get_distance_spectrumweight();
  s << (qint8)limit_authornonplayed;
  s << (qint8)get_shown_aboutbox();
  s << QString(""); // get_mixer_command();
  s << (qint8)0; // the obsolete open_mixer flag
  s << (qint8)ask_mix;
  s << (qint8)auto_popqueue;
  s << QString(""); // get_record_command();
  s << QString(""); // was get_replay_command();
  s << QString(""); // get_record_mixer_command();
  s << get_color_tempo44();
  s << get_color_tempo54();
  s << get_color_tempo64();
  s << get_color_tempo74();
  s << get_color_tempo84();
  s << (qint8)get_show_tempo54();
  s << (qint8)get_show_tempo64();
  s << (qint8)get_show_tempo74();
  s << (qint8)get_show_tempo84();
  s << (qint8)get_show_unknown_tempo();
  s << get_color_green_time();
  s << get_color_yellow_time();
  s << get_color_orange_time();
  s << get_color_red_time();
  s << (qint8)get_color_main_window();
  s << (qint8)get_log_spectrum_distance();
  s << get_color_played_song();
  s << get_color_played_author();
  s << get_color_unavailable();
  s << get_color_dcolor_col();
  s << (quint16)get_color_cluster_depth();
  s << QString(""); // get_bpm_mixer_command();
  s << (qint8)0 ; // open_bpmmixer;
  quint16 nr = 0;
  s << nr;
  s << get_header_state();
  s << get_distance_tempoweight();
  s << get_distance_echoweight();
  s << get_distance_rhythmweight();
  s << get_distance_compositionweight();
  s << (quint16)get_max_songs();
  // save pixmap if it is there
  s << get_color_unchecked();
  for(int i = 0 ; i < 8 ; i ++)
    {
      s << (quint16)analyzers[i].getCmd();
      s << analyzers[i].getName();
      s << analyzers[i].getRemote();
      s << (qint8)analyzers[i].getCopyMusic();
    }
  s << get_anal_bpm_from();
  s << get_anal_bpm_to();
  s << (quint16)get_anal_bpm_technique();
  s << (qint8) color_songs_based_on_history;
  s << get_color_alltime();
  s << (qint8)play_fragments;
  s << get_tag_include();
  s << get_tag_mustbe();
  s << get_tag_exclude();
}

bool Config::load()
{
  // initialize the analyzers and players
  for(int i = 0 ; i < 8 ; i++)
    {
      analyzers[i].setNameId("analyzer"+QString::number(i),i);
      analyzers[i].setAnalyzer();
    }
  for(int i = 0 ; i < 4 ; i++)
    players[i].setNameId("player"+QString::number(i),i);
  QTreeWidget * taglist = new QTreeWidget();
  if (QFile::exists(".bpmdj"))
    {
      qint8 b;
      quint16 magic;
      quint16 w;
      QString str;
      QColor clr;
      float4 fl;
      QFile f(".bpmdj");
      f.open(QIODevice::ReadOnly);
      QDataStream s(&f);
      s >> magic;
      if ( magic < MAGIC_3_5 )
	{
	  return false;
	}
      if (magic == MAGIC_3_5 || magic == MAGIC_3_6 || magic == MAGIC_3_7)
	{
	  s >> w; set_file_count (w);
	  s >> w; set_yellowTime (w);
	  s >> w; set_orangeTime (w);
	  s >> w; set_redTime (w);
	  s >> w; set_filterBpm (w);
	  s >> b; color_range.state=b;
	  s >> b; color_played.state=b;
	  s >> b; color_authorplayed.state=b;
	  s >> b; color_ondisk.state=b;
	  s >> b; color_cues.state=b;
	  s >> b; color_dcolor.state=b;
	  s >> b; color_spectrum.state=b;
	  s >> b; set_authorDecay(b);
	  s >> b; limit_ondisk.state=b;
	  s >> b; limit_nonplayed.state=b;
	  s >> b; limit_uprange.state=b;
	  s >> b; limit_downrange.state=b;
	  s >> b; limit_indistance.state=b;
	  for(int i = 0 ; i < 4 ; i ++)
	    {
	      s >> w;   players[i].setCommand((SongSlot::command_type)w);
	      s >> str; players[i].setName(str);
	      s >> str; players[i].setRemote(str);
	    }
	  s >> fl; set_distance_temposcale(fl);
	  s >> fl; set_distance_spectrumweight(fl);
	  s >> b; limit_authornonplayed.state=b;
	  s >> b; set_shown_aboutbox(b);
	  s >> str; //set_mixer_command(str);
	  s >> b; // open_mixer.set(b);
	  s >> b; ask_mix.set(b);
	  s >> b; auto_popqueue.set(b);
	  s >> str; // set_record_command(str);
	  s >> str; // set_replay_command(str);
	  s >> str; // set_record_mixer_command(str);
	  s >> clr; set_color_tempo44(clr);
	  s >> clr; set_color_tempo54(clr);
	  s >> clr; set_color_tempo64(clr);
	  s >> clr; set_color_tempo74(clr);
	  s >> clr; set_color_tempo84(clr);
	  s >> b; set_show_tempo54(b);
	  s >> b; set_show_tempo64(b);
	  s >> b; set_show_tempo74(b);
	  s >> b; set_show_tempo84(b);
	  s >> b; set_show_unknown_tempo(b);
	  s >> clr; set_color_green_time(clr);
	  s >> clr; set_color_yellow_time(clr);
	  s >> clr; set_color_orange_time(clr);
	  s >> clr; set_color_red_time(clr);
	  s >> b; set_color_main_window(b);
	  s >> b; set_log_spectrum_distance(b);
	  s >> clr; set_color_played_song(clr);
	  s >> clr; set_color_played_author(clr);
	  s >> clr; set_color_unavailable(clr);
	  s >> clr; set_color_dcolor_col(clr);
	  s >> w; set_color_cluster_depth (w);
	  s >> str; // set_bpm_mixer_command(str);
	  s >> b; // open_bpmmixer.set(b);
	  // read tag data
	  s >> w; assert (w==0);
	  /*	  while(w-->0)
	    {
	      QString t;
	      qint8 i,m,e;
	      s >> t;
	      s >> i; 
	      s >> m; 
	      s >> e; 
	      new QTreeWidgetItem(taglist, t, i ? TAG_TRUE : TAG_FALSE,
				m ? TAG_TRUE : TAG_FALSE,
				e ? TAG_TRUE : TAG_FALSE);
				}
	  */
	  // read header
	  s >> w; 
	  for(int i = 0 ; i < w ; i++)
	    {
	      quint16 m1,m2; s >> m1; s >> m2;
	      // realize_mapping(get_header(),i,m1,m2);
	    }
	  float4 F;
	  s >> F; set_distance_tempoweight(F);
	  s >> F; set_distance_echoweight(F);
	  s >> F; set_distance_rhythmweight(F);
	  s >> F; set_distance_compositionweight(F);
	  s >> w; set_max_songs(w);
	  s >> clr; set_color_unchecked(clr);
	  for(int i = 0 ; i < 8 ; i ++)
	    {
	      s >> w;   analyzers[i].setCommand((SongSlot::command_type)w);
	      s >> str; analyzers[i].setName(str);
	      s >> str; analyzers[i].setRemote(str);
	    }
	  s >> fl;  set_anal_bpm_from(fl);
	  s >> fl;  set_anal_bpm_to(fl);
	  s >> w;   set_anal_bpm_technique(w);
	  s >> b;   color_songs_based_on_history.set(b);
	  s >> clr; set_color_alltime(clr);
	  s >> b;   play_fragments.set(b);
	  s >> str; set_tag_include(str);
	  s >> str; set_tag_mustbe(str);
	  s >> str; set_tag_exclude(str);
	}
      else if (magic == MAGIC_4_0 || magic == MAGIC_4_1)
	{
	  s >> w; set_file_count (w);
	  s >> w; set_yellowTime (w);
	  s >> w; set_orangeTime (w);
	  s >> w; set_redTime (w);
	  s >> w; set_filterBpm (w);
	  s >> b; color_range.state=b;
	  s >> b; color_played.state=b;
	  s >> b; color_authorplayed.state=b;
	  s >> b; color_ondisk.state=b;
	  s >> b; color_cues.state=b;
	  s >> b; color_dcolor.state=b;
	  s >> b; color_spectrum.state=b;
	  s >> b; set_authorDecay(b);
	  s >> b; limit_ondisk.state=b;
	  s >> b; limit_nonplayed.state=b;
	  s >> b; limit_uprange.state=b;
	  s >> b; limit_downrange.state=b;
	  s >> b; limit_indistance.state=b;
	  for(int i = 0 ; i < 4 ; i ++)
	    {
	      s >> w;   players[i].setCommand((SongSlot::command_type)w);
	      s >> str; players[i].setName(str);
	      s >> str; players[i].setRemote(str);
	      s >> b; players[i].setCopyMusic(b);
	    }
	  s >> fl; set_distance_temposcale(fl);
	  s >> fl; set_distance_spectrumweight(fl);
	  s >> b; limit_authornonplayed.state=b;
	  s >> b; set_shown_aboutbox(b);
	  s >> str; //set_mixer_command(str);
	  s >> b; // open_mixer.set(b);
	  s >> b; ask_mix.set(b);
	  s >> b; auto_popqueue.set(b);
	  s >> str; // set_record_command(str);
	  s >> str; // set_replay_command(str);
	  s >> str; // set_record_mixer_command(str);
	  s >> clr; set_color_tempo44(clr);
	  s >> clr; set_color_tempo54(clr);
	  s >> clr; set_color_tempo64(clr);
	  s >> clr; set_color_tempo74(clr);
	  s >> clr; set_color_tempo84(clr);
	  s >> b; set_show_tempo54(b);
	  s >> b; set_show_tempo64(b);
	  s >> b; set_show_tempo74(b);
	  s >> b; set_show_tempo84(b);
	  s >> b; set_show_unknown_tempo(b);
	  s >> clr; set_color_green_time(clr);
	  s >> clr; set_color_yellow_time(clr);
	  s >> clr; set_color_orange_time(clr);
	  s >> clr; set_color_red_time(clr);
	  s >> b; set_color_main_window(b);
	  s >> b; set_log_spectrum_distance(b);
	  s >> clr; set_color_played_song(clr);
	  s >> clr; set_color_played_author(clr);
	  s >> clr; set_color_unavailable(clr);
	  s >> clr; set_color_dcolor_col(clr);
	  s >> w; set_color_cluster_depth (w);
	  s >> str; // set_bpm_mixer_command(str);
	  s >> b; // open_bpmmixer.set(b);
	  // read tag data
	  s >> w; assert (w==0);
	  /*	  while(w-->0)
	    {
	      QString t;
	      qint8 i,m,e;
	      s >> t;
	      s >> i; 
	      s >> m; 
	      s >> e; 
	      new QTreeWidgetItem(taglist, t, i ? TAG_TRUE : TAG_FALSE,
				m ? TAG_TRUE : TAG_FALSE,
				e ? TAG_TRUE : TAG_FALSE);
				}
	  */
	  // read header
	  s >> w; 
	  for(int i = 0 ; i < w ; i++)
	    {
	      quint16 m1,m2; s >> m1; s >> m2;
	      // realize_mapping(get_header(),i,m1,m2);
	    }
	  float4 F;
	  s >> F; set_distance_tempoweight(F);
	  s >> F; set_distance_echoweight(F);
	  s >> F; set_distance_rhythmweight(F);
	  s >> F; set_distance_compositionweight(F);
	  s >> w; set_max_songs(w);
	  s >> clr; set_color_unchecked(clr);
	  for(int i = 0 ; i < 8 ; i ++)
	    {
	      s >> w;   analyzers[i].setCommand((SongSlot::command_type)w);
	      s >> str; analyzers[i].setName(str);
	      s >> str; analyzers[i].setRemote(str);
	      s >> b; analyzers[i].setCopyMusic(b);
	    }
	  s >> fl;  set_anal_bpm_from(fl);
	  s >> fl;  set_anal_bpm_to(fl);
	  s >> w;   set_anal_bpm_technique(w);
	  s >> b;   color_songs_based_on_history.set(b);
	  s >> clr; set_color_alltime(clr);
	  s >> b;   play_fragments.set(b);
	  s >> str; set_tag_include(str);
	  s >> str; set_tag_mustbe(str);
	  s >> str; set_tag_exclude(str);
	}
      else if (magic == MAGIC_4_1b)
	{
	  s >> w; set_file_count (w);
	  s >> w; set_yellowTime (w);
	  s >> w; set_orangeTime (w);
	  s >> w; set_redTime (w);
	  s >> w; set_filterBpm (w);
	  s >> b; color_range.state=b;
	  s >> b; color_played.state=b;
	  s >> b; color_authorplayed.state=b;
	  s >> b; color_ondisk.state=b;
	  s >> b; color_cues.state=b;
	  s >> b; color_dcolor.state=b;
	  s >> b; color_spectrum.state=b;
	  s >> b; set_authorDecay(b);
	  s >> b; limit_ondisk.state=b;
	  s >> b; limit_nonplayed.state=b;
	  s >> b; limit_uprange.state=b;
	  s >> b; limit_downrange.state=b;
	  s >> b; limit_indistance.state=b;
	  for(int i = 0 ; i < 4 ; i ++)
	    {
	      s >> w;   players[i].setCommand((SongSlot::command_type)w);
	      s >> str; players[i].setName(str);
	      s >> str; players[i].setRemote(str);
	      s >> b; players[i].setCopyMusic(b);
	    }
	  s >> fl; set_distance_temposcale(fl);
	  s >> fl; set_distance_spectrumweight(fl);
	  s >> b; limit_authornonplayed.state=b;
	  s >> b; set_shown_aboutbox(b);
	  s >> str; //set_mixer_command(str);
	  s >> b; // open_mixer.set(b);
	  s >> b; ask_mix.set(b);
	  s >> b; auto_popqueue.set(b);
	  s >> str; // set_record_command(str);
	  s >> str; // set_replay_command(str);
	  s >> str; // set_record_mixer_command(str);
	  s >> clr; set_color_tempo44(clr);
	  s >> clr; set_color_tempo54(clr);
	  s >> clr; set_color_tempo64(clr);
	  s >> clr; set_color_tempo74(clr);
	  s >> clr; set_color_tempo84(clr);
	  s >> b; set_show_tempo54(b);
	  s >> b; set_show_tempo64(b);
	  s >> b; set_show_tempo74(b);
	  s >> b; set_show_tempo84(b);
	  s >> b; set_show_unknown_tempo(b);
	  s >> clr; set_color_green_time(clr);
	  s >> clr; set_color_yellow_time(clr);
	  s >> clr; set_color_orange_time(clr);
	  s >> clr; set_color_red_time(clr);
	  s >> b; set_color_main_window(b);
	  s >> b; set_log_spectrum_distance(b);
	  s >> clr; set_color_played_song(clr);
	  s >> clr; set_color_played_author(clr);
	  s >> clr; set_color_unavailable(clr);
	  s >> clr; set_color_dcolor_col(clr);
	  s >> w; set_color_cluster_depth (w);
	  s >> str; // set_bpm_mixer_command(str);
	  s >> b; // open_bpmmixer.set(b);
	  // read tag data
	  s >> w; assert (w==0);
	  // read header
	  QByteArray ba;
	  s >> ba;
	  set_header_state(ba);
	  float4 F;
	  s >> F; set_distance_tempoweight(F);
	  s >> F; set_distance_echoweight(F);
	  s >> F; set_distance_rhythmweight(F);
	  s >> F; set_distance_compositionweight(F);
	  s >> w; set_max_songs(w);
	  s >> clr; set_color_unchecked(clr);
	  for(int i = 0 ; i < 8 ; i ++)
	    {
	      s >> w;   analyzers[i].setCommand((SongSlot::command_type)w);
	      s >> str; analyzers[i].setName(str);
	      s >> str; analyzers[i].setRemote(str);
	      s >> b; analyzers[i].setCopyMusic(b);
	    }
	  s >> fl;  set_anal_bpm_from(fl);
	  s >> fl;  set_anal_bpm_to(fl);
	  s >> w;   set_anal_bpm_technique(w);
	  s >> b;   color_songs_based_on_history.set(b);
	  s >> clr; set_color_alltime(clr);
	  s >> b;   play_fragments.set(b);
	  s >> str; set_tag_include(str);
	  s >> str; set_tag_mustbe(str);
	  s >> str; set_tag_exclude(str);
	}
      else
	printf("Wrong configuration file format\n");
      calc_and_cache();
      // if the magic was smaller than 3_5 we convert the TAGLIST
      if (magic < MAGIC_3_5)
	taglist2config(taglist);
    }
  return true;
}

void taglist2strings(QTreeWidget * tagList, QString &in, QString& must, 
		     QString&out)
{
  stdTreeWidgetIterator i(tagList);
ITERATE_OVER(i)

    QString tag = i.val()->text(TAGS_TEXT);
    if (i.val()->text(TAGS_OR)==TAG_TRUE) in+=OneSpace+tag+OneSpace;
    if (i.val()->text(TAGS_AND)==TAG_TRUE) must+=OneSpace+tag+OneSpace;
    if (i.val()->text(TAGS_NOT)==TAG_TRUE) out+=OneSpace+tag+OneSpace;
  } 
}

void taglist2config(QTreeWidget *taglist)
{
  QString in,must,out;
  in=must=out="";
  taglist2strings(taglist,in,must,out);
  Config::set_tag_include(in);
  Config::set_tag_exclude(out);
  Config::set_tag_mustbe(must);
}

bool Config::open_ui(int pane)
{
  BpmDjPreferences preferences;
  preferences.yellowTime->setText(tonumber((unsigned4)get_yellowTime()));
  preferences.orangeTime->setText(tonumber((unsigned4)get_orangeTime()));
  preferences.redTime->setText(tonumber((unsigned4)get_redTime()));
  
  setBackgroundColor(preferences.greenTimeColor,get_color_green_time ());
  setBackgroundColor(preferences.yellowTimeColor, get_color_yellow_time ());
  setBackgroundColor(preferences.orangeTimeColor, get_color_orange_time ());
  setBackgroundColor(preferences.redTimeColor, get_color_red_time ());
  
  preferences.color_main_window->setChecked(get_color_main_window());
  
  preferences.authorDecay->setValue( get_authorDecay() ) ;
  preferences.tempoSpin->setValue( lround(get_distance_temposcale()*100.0) ) ;
  
  setBackgroundColor(preferences.colorTempo44,get_color_tempo44 ());
  setBackgroundColor(preferences.colorTempo54,get_color_tempo54 ());
  setBackgroundColor(preferences.colorTempo64,get_color_tempo64 ());
  setBackgroundColor(preferences.colorTempo74,get_color_tempo74 ());
  setBackgroundColor(preferences.colorTempo84,get_color_tempo84 ());
  
  setBackgroundColor(preferences.colorAlltimePlaycount,
			 get_color_alltime ());
  
  preferences.showTempo54 -> setChecked( get_show_tempo54 ());
  preferences.showTempo64 -> setChecked( get_show_tempo64 ());
  preferences.showTempo74 -> setChecked( get_show_tempo74 ());
  preferences.showTempo84 -> setChecked( get_show_tempo84 ());
  preferences.showUnknownTempo -> setChecked( get_show_unknown_tempo ());

  setBackgroundColor(preferences.colorPlayedSong,get_color_played_song ());
  setBackgroundColor(preferences.colorPlayedAuthor,
			 get_color_played_author());
  setBackgroundColor(preferences.colorUnavailableSong,
			 get_color_unavailable ());
  setBackgroundColor(preferences.colorUncheckedSong,get_color_unchecked ());
  setBackgroundColor(preferences.colordColorCol,get_color_dcolor_col ());

  preferences.dColorMetric->spectrumSpin -> 
    setValue ( (int)(0.5+get_distance_spectrumweight()*100.0) );
  preferences.dColorMetric->tempoDistanceSpin -> 
    setValue( (int)(0.5+get_distance_tempoweight()*100.0) );
  preferences.dColorMetric->echoSpin -> 
    setValue( (int)(0.5+get_distance_echoweight()*100.0) );
  preferences.dColorMetric->rhythmSpin -> 
    setValue( (int)(0.5+get_distance_rhythmweight()*100.0) );
  preferences.dColorMetric->compositionSpin -> 
    setValue( (int)(0.5+get_distance_compositionweight()*100.0) );
  preferences.max_songs->setValue(get_max_songs());

  preferences.clusterDepth -> setValue (get_color_cluster_depth ());

  // preferences.bpmmixcmd -> setText( get_bpm_mixer_command () );
  preferences.subtabs -> setCurrentIndex( pane );

  preferences.From -> setText(get_anal_bpm_from() ? 
			      QString::number(get_anal_bpm_from()) : "");
  preferences.To   -> setText(get_anal_bpm_to() ? 
			      QString::number(get_anal_bpm_to()): "");
  
  /**
   * Man man man man man, what a crap has trolltech put off here.
   * I keep on spending time on converting the Qt3 version to Qt4.
   * Couldn't they make this process easier ?? The documentation is
   * half hearted. Functionality gets lost and things I relied on 
   * broke down. So here we now want to set the right button.
   */  
  signed4 nr = get_anal_bpm_technique();
  if (nr<1 || nr>5)
    Warning("The BPM analysis technique required doesn't exist. "
	    "Setting to the first available");
  if (nr==1) preferences.resamplingScan->setChecked(true);
  else if (nr==2) preferences.ultraLongFFT->setChecked(true);
  else if (nr==3) preferences.enveloppeSpectrum->setChecked(true);
  else if (nr==4) preferences.fullAutoCorrelation->setChecked(true);
  else if (nr==5) preferences.weightedEnvCor->setChecked(true);
  else if (nr==6) preferences.experimentalAlg->setChecked(true);
  else assert(0);

  if (preferences.exec()!=QDialog::Accepted) return false;

  set_yellowTime(qtos4(preferences.yellowTime->text()));
  set_orangeTime(qtos4(preferences.orangeTime->text()));
  set_redTime(qtos4(preferences.redTime->text()));
  set_authorDecay (preferences.authorDecay->value());
  set_distance_temposcale ( (float4)(preferences.tempoSpin->value())/100.0);
  
  set_color_tempo44(backgroundColor(preferences.colorTempo44));
  set_color_tempo54(backgroundColor(preferences.colorTempo54));
  set_color_tempo64(backgroundColor(preferences.colorTempo64));
  set_color_tempo74(backgroundColor(preferences.colorTempo74));
  set_color_tempo84(backgroundColor(preferences.colorTempo84));
  set_color_alltime(backgroundColor(preferences.colorAlltimePlaycount));
  
  set_show_tempo54 (preferences.showTempo54->isChecked());
  set_show_tempo64 (preferences.showTempo64->isChecked());
  set_show_tempo74 (preferences.showTempo74->isChecked());
  set_show_tempo84 (preferences.showTempo84->isChecked());
  set_show_unknown_tempo(preferences.showUnknownTempo->isChecked());
  
  set_color_red_time(backgroundColor(preferences.redTimeColor));
  set_color_orange_time(backgroundColor(preferences.orangeTimeColor));
  set_color_yellow_time(backgroundColor(preferences.yellowTimeColor));
  set_color_green_time(backgroundColor(preferences.greenTimeColor));
  set_color_main_window(preferences.color_main_window->isChecked());
  
  set_color_played_song(backgroundColor(preferences.colorPlayedSong));
  set_color_played_author(backgroundColor(preferences.colorPlayedAuthor));
  set_color_unavailable(backgroundColor(preferences.colorUnavailableSong));
  set_color_unchecked(backgroundColor(preferences.colorUncheckedSong));
  set_color_dcolor_col(backgroundColor(preferences.colordColorCol));
  set_color_cluster_depth(preferences.clusterDepth -> value());
  
  set_distance_tempoweight((float4)(preferences.dColorMetric->
				    tempoDistanceSpin->value())/100.0);
  set_distance_spectrumweight((float4)(preferences.dColorMetric->
				       spectrumSpin->value())/100.0);
  set_distance_echoweight((float4)(preferences.dColorMetric->
				   echoSpin->value())/100.0);
  set_distance_rhythmweight((float4)(preferences.dColorMetric->
				     rhythmSpin->value())/100.0);
  set_distance_compositionweight((float4)(preferences.dColorMetric->
					  compositionSpin->value())/100.0);
  set_max_songs(preferences.max_songs->value());
  
  set_anal_bpm_from(qtof4(preferences.From->text()));
  set_anal_bpm_to(qtof4(preferences.To->text()));
  
  nr=0;
  if (preferences.resamplingScan->isChecked()) nr=1;
  if (preferences.ultraLongFFT->isChecked()) nr=2;
  if (preferences.enveloppeSpectrum->isChecked()) nr=3;
  if (preferences.fullAutoCorrelation->isChecked()) nr=4;
  if (preferences.weightedEnvCor->isChecked()) nr=5;
  if (preferences.experimentalAlg->isChecked()) nr=6;
  assert(nr);
  set_anal_bpm_technique(nr);
  if (get_anal_bpm_from()>get_anal_bpm_to())
    {
      float4 t=get_anal_bpm_from();
      set_anal_bpm_from(get_anal_bpm_to());
      set_anal_bpm_to(t);
    }
  save();
  calc_and_cache();
  return true;
}

void Config::calc_and_cache()
{
  SongMetriek::std.set_tempo_limits(get_distance_temposcale());
  SongMetriek::std.set_show_harmonics(get_show_tempo54(),
				      get_show_tempo64(),
				      get_show_tempo74(),
				      get_show_tempo84());
  SongMetriek::std.set_unknown_tempo_matches(get_show_unknown_tempo());
  SongMetriek::std.set_tempo_weight(get_distance_tempoweight());
  SongMetriek::std.set_spectrum_weight(get_distance_spectrumweight());
  SongMetriek::std.set_echo_weight(get_distance_echoweight());
  SongMetriek::std.set_rhythm_weight(get_distance_rhythmweight());
  SongMetriek::std.set_composition_weight(get_distance_compositionweight());
  SongMetriek::std.prepare();
}
#endif // __loaded__config_cpp__
