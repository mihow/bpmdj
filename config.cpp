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

#include <qlineedit.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qcheckbox.h>
#include <stdlib.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qmessagebox.h>
#include "preferences.logic.h"
#include "config.h"
#include "song.h"
#include "song-metric.h"
#include "metric-widget.h"
#include "kbpm-dj.h"
#include "scripts.h"
#include "magic.h"

init_singleton_var(Config,file_count,int,1000);
init_singleton_var(Config,yellowTime,int,120);
init_singleton_var(Config,orangeTime,int,150);
init_singleton_var(Config,redTime,int,180); 
init_singleton_var(Config,filterBpm,int,0);
init_singleton_var(Config,authorDecay,int,5);
init_singleton_var(Config,color_range,bool,true);
init_singleton_var(Config,color_played,bool,true);
init_singleton_var(Config,color_authorplayed,bool,true);
init_singleton_var(Config,color_ondisk,bool,true);
init_singleton_var(Config,color_cues,bool,true);
init_singleton_var(Config,color_dcolor,bool,true);
init_singleton_var(Config,color_spectrum,bool,true);
init_singleton_var(Config,limit_ondisk,bool,true);
init_singleton_var(Config,limit_nonplayed,bool,false);
init_singleton_var(Config,limit_uprange,bool,false);
init_singleton_var(Config,limit_downrange,bool,false);
init_singleton_var(Config,limit_indistance,bool,false);
init_singleton_var(Config,limit_authornonplayed,bool,false);
init_singleton_var(Config,shown_aboutbox,bool,false);
init_singleton_var(Config,open_mixer,bool,false);
init_singleton_var(Config,distance_temposcale,float,0.06);
init_singleton_var(Config,distance_spectrumweight,float,1.0);
init_singleton_var(Config,ask_mix,bool,true);
init_singleton_var(Config,auto_popqueue,bool,false);
init_singleton_var(Config,playCommand1,QString,"");
init_singleton_var(Config,playCommand2,QString,"");
init_singleton_var(Config,playCommand3,QString,"");
init_singleton_var(Config,playCommand4,QString,"");
init_singleton_var(Config,mixer_command,QString,"");
init_singleton_var(Config,record_command,QString,"");
init_singleton_var(Config,replay_command,QString,"");
init_singleton_var(Config,record_mixer_command,QString,"");
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
init_singleton_var(Config,taglist,QListView*,NULL);
init_singleton_var(Config,header,QHeader*,NULL);
init_singleton_var(Config,bpm_mixer_command,QString,"kbpm-mix --alsa");
init_singleton_var(Config,open_bpmmixer,bool,false); 
init_singleton_var(Config,distance_tempoweight,float,0.2);
init_singleton_var(Config,distance_echoweight,float,0.7);
init_singleton_var(Config,distance_rythmweight,float,2.5);
init_singleton_var(Config,distance_compositionweight,float,1.0);
init_singleton_var(Config,max_songs,int,100);
init_singleton_var(Config,color_unchecked,QColor,QColor(219,219,219));
init_singleton_var(Config,analCommand1,QString,"");
init_singleton_var(Config,analCommand2,QString,"");
init_singleton_var(Config,analCommand3,QString,"");
init_singleton_var(Config,analCommand4,QString,"");
init_singleton_var(Config,analCommand5,QString,"");
init_singleton_var(Config,analCommand6,QString,"");
init_singleton_var(Config,analCommand7,QString,"");
init_singleton_var(Config,analCommand8,QString,"");
init_singleton_var(Config,anal_bpm_from,float,90);
init_singleton_var(Config,anal_bpm_to,float,160);
init_singleton_var(Config,anal_bpm_technique,int,1);

/**
 * we don't open the mixer command because the program its normal 
 * mode of operation should be two soundcards. The mixer is a fix and
 * even almost experimental feature.
 */
void dump_header(QHeader * h)
{
  // export the header;
  int nr = h->count();
  for(int i = 0 ; i < nr ; i ++)
    printf("%d -> %d [%d]\t",i,h->mapToSection(i),h->sectionSize(i));
}

void realize_mapping(QHeader * h, int i, int j, int s)
{
  h->moveSection(j,i);
  //  dump_header(h);
  h->resizeSection(j,s);
}

void copy_header(QHeader * in, QHeader * out)
{
  for(int i = 0 ; i < in->count() ; i ++)
    {
      int j = in->mapToSection(i);
      realize_mapping(out,i,j,in->sectionSize(j));
    }
}

void Config::save()
{
  QFile f(".kbpmdj");
  f.open(IO_WriteOnly);
  if (!f.isOpen() || !f.isWritable())
    {
      Error(true,"Configuration file \".kbpmdj\" cannot be opened for writing");
      return;
    }
  QDataStream s(&f);
  s << (Q_UINT16)MAGIC_NOW;
  s << (Q_UINT16)get_file_count();
  s << (Q_UINT16)get_yellowTime();
  s << (Q_UINT16)get_orangeTime();
  s << (Q_UINT16)get_redTime();
  s << (Q_UINT16)get_filterBpm();
  s << (Q_INT8)get_color_range();
  s << (Q_INT8)get_color_played();
  s << (Q_INT8)get_color_authorplayed();
  s << (Q_INT8)get_color_ondisk();
  s << (Q_INT8)get_color_cues();
  s << (Q_INT8)get_color_dcolor();
  s << (Q_INT8)get_color_spectrum();
  s << (Q_INT8)get_authorDecay();
  s << (Q_INT8)get_limit_ondisk();
  s << (Q_INT8)get_limit_nonplayed();
  s << (Q_INT8)get_limit_uprange();
  s << (Q_INT8)get_limit_downrange();
  s << (Q_INT8)get_limit_indistance();
  s << get_playCommand1();
  s << get_playCommand2();
  s << get_playCommand3();
  s << get_playCommand4();
  s << get_distance_temposcale();
  s << get_distance_spectrumweight();
  s << (Q_INT8)get_limit_authornonplayed();
  s << (Q_INT8)get_shown_aboutbox();
  s << get_mixer_command();
  s << (Q_INT8)get_open_mixer();
  s << (Q_INT8)get_ask_mix();
  s << (Q_INT8)get_auto_popqueue();
  s << get_record_command();
  s << get_replay_command();
  s << get_record_mixer_command();
  s << get_color_tempo44();
  s << get_color_tempo54();
  s << get_color_tempo64();
  s << get_color_tempo74();
  s << get_color_tempo84();
  s << (Q_INT8)get_show_tempo54();
  s << (Q_INT8)get_show_tempo64();
  s << (Q_INT8)get_show_tempo74();
  s << (Q_INT8)get_show_tempo84();
  s << (Q_INT8)get_show_unknown_tempo();
  s << get_color_green_time();
  s << get_color_yellow_time();
  s << get_color_orange_time();
  s << get_color_red_time();
  s << (Q_INT8)get_color_main_window();
  s << (Q_INT8)get_log_spectrum_distance();
  s << get_color_played_song();
  s << get_color_played_author();
  s << get_color_unavailable();
  s << get_color_dcolor_col();
  s << (Q_UINT16)get_color_cluster_depth();
  s << get_bpm_mixer_command();
  s << (Q_INT8)get_open_bpmmixer();
  // export the taglist... tricky thing (tm)
  Q_UINT16 nr = get_taglist()->childCount();
  s << nr;
  QListViewItemIterator it(get_taglist());
  while(it.current())
    {
      QListViewItem * i = it.current();
      s << (i->text(0)); 
      bool b = i->text(1) == TAG_TRUE;
      s << (Q_INT8)b;
      b = i->text(2) == TAG_TRUE;
      s << (Q_INT8)b;
      b = i->text(3) == TAG_TRUE;
      s << (Q_INT8)b;
      it++;
    }
  // export the header;
  //  dump_header(get_header());
  nr = get_header()->count();
  s << nr;
  for(int i = 0 ; i < nr ; i ++)
    {
      int j =  get_header()->mapToSection(i);
      s <<(Q_UINT16)j;
      s <<(Q_UINT16)get_header()->sectionSize(j);
    }
  s << get_distance_tempoweight();
  s << get_distance_echoweight();
  s << get_distance_rythmweight();
  s << get_distance_compositionweight();
  s << (Q_UINT16)get_max_songs();
  // save pixmap if it is there
  s << get_color_unchecked();
  s << get_analCommand1();
  s << get_analCommand2();
  s << get_analCommand3();
  s << get_analCommand4();
  s << get_analCommand5();
  s << get_analCommand6();
  s << get_analCommand7();
  s << get_analCommand8();
  s << get_anal_bpm_from();
  s << get_anal_bpm_to();
  s << (Q_UINT16)get_anal_bpm_technique();
}

void Config::load()
{
  set_taglist(new QListView());
  set_header(new QHeader());
  if (QFile::exists(".kbpmdj"))
    {
      Q_INT8 b;
      Q_UINT16 magic;
      Q_UINT16 w;
      QString str;
      QColor clr;
      float fl;
      QFile f(".kbpmdj");
      f.open(IO_ReadOnly);
      QDataStream s(&f);
      s >> magic;
      if (magic == MAGIC_1_6)
	{
	  s >> w; set_file_count (w);
	  s >> w; set_yellowTime (w);
	  s >> w; set_orangeTime (w);
	  s >> w; set_redTime (w);
	  s >> w; set_filterBpm (w);
	  s >> b; set_color_range(b);
	  s >> b; set_color_played(b);
	  s >> b; set_color_authorplayed(b);
	  s >> b; set_authorDecay(b);
	  s >> str; set_playCommand1(str);
	  s >> str; set_playCommand2(str);
	}
      else if (magic == MAGIC_1_7)
	{
	  s >> w; set_file_count (w);
	  s >> w; set_yellowTime (w);
	  s >> w; set_orangeTime (w);
	  s >> w; set_redTime (w);
	  s >> w; set_filterBpm (w);
	  s >> b; set_color_range(b);
	  s >> b; set_color_played(b);
	  s >> b; set_color_authorplayed(b);
	  s >> b; set_color_ondisk(b);
	  s >> b; set_color_cues(b);
	  s >> b; set_color_dcolor(b);
	  s >> b; set_color_spectrum(b);
	  s >> b; set_authorDecay(b);
	  s >> b; set_limit_ondisk(b);
	  s >> b; set_limit_nonplayed(b);
	  s >> b; set_limit_uprange(b);
	  s >> b; set_limit_downrange(b);
	  s >> b; set_limit_indistance(b);
	  s >> b; // limit_inspectrum
	  s >> str; set_playCommand1(str);
	  s >> str; set_playCommand2(str);
	  s >> str; set_playCommand3(str);
	  s >> str; set_playCommand4(str);
	}
      else if (magic == MAGIC_1_8)
	{
	  s >> w; set_file_count (w);
	  s >> w; set_yellowTime (w);
	  s >> w; set_orangeTime (w);
	  s >> w; set_redTime (w);
	  s >> w; set_filterBpm (w);
	  s >> b; set_color_range(b);
	  s >> b; set_color_played(b);
	  s >> b; set_color_authorplayed(b);
	  s >> b; set_color_ondisk(b);
	  s >> b; set_color_cues(b);
	  s >> b; set_color_dcolor(b);
	  s >> b; set_color_spectrum(b);
	  s >> b; set_authorDecay(b);
	  s >> b; set_limit_ondisk(b);
	  s >> b; set_limit_nonplayed(b);
	  s >> b; set_limit_uprange(b);
	  s >> b; set_limit_downrange(b);
	  s >> b; set_limit_indistance(b);
	  s >> str; set_playCommand1(str);
	  s >> str; set_playCommand2(str);
	  s >> str; set_playCommand3(str);
	  s >> str; set_playCommand4(str);
	}
      else if (magic == MAGIC_1_9)
	{
	  s >> w; set_file_count (w);
	  s >> w; set_yellowTime (w);
	  s >> w; set_orangeTime (w);
	  s >> w; set_redTime (w);
	  s >> w; set_filterBpm (w);
	  s >> b; set_color_range(b);
	  s >> b; set_color_played(b);
	  s >> b; set_color_authorplayed(b);
	  s >> b; set_color_ondisk(b);
	  s >> b; set_color_cues(b);
	  s >> b; set_color_dcolor(b);
	  s >> b; set_color_spectrum(b);
	  s >> b; set_authorDecay(b);
	  s >> b; set_limit_ondisk(b);
	  s >> b; set_limit_nonplayed(b);
	  s >> b; set_limit_uprange(b);
	  s >> b; set_limit_downrange(b);
	  s >> b; set_limit_indistance(b);
	  s >> str; set_playCommand1(str);
	  s >> str; set_playCommand2(str);
	  s >> str; set_playCommand3(str);
	  s >> str; set_playCommand4(str);
	  s >> fl; set_distance_temposcale(fl);
	  s >> fl; set_distance_spectrumweight(fl);
	  s >> b; set_limit_authornonplayed(b);
	  s >> b; set_shown_aboutbox(b);
	}
      else if (magic == MAGIC_2_1)
	{
	  s >> w; set_file_count (w);
	  s >> w; set_yellowTime (w);
	  s >> w; set_orangeTime (w);
	  s >> w; set_redTime (w);
	  s >> w; set_filterBpm (w);
	  s >> b; set_color_range(b);
	  s >> b; set_color_played(b);
	  s >> b; set_color_authorplayed(b);
	  s >> b; set_color_ondisk(b);
	  s >> b; set_color_cues(b);
	  s >> b; set_color_dcolor(b);
	  s >> b; set_color_spectrum(b);
	  s >> b; set_authorDecay(b);
	  s >> b; set_limit_ondisk(b);
	  s >> b; set_limit_nonplayed(b);
	  s >> b; set_limit_uprange(b);
	  s >> b; set_limit_downrange(b);
	  s >> b; set_limit_indistance(b);
	  s >> str; set_playCommand1(str);
	  s >> str; set_playCommand2(str);
	  s >> str; set_playCommand3(str);
	  s >> str; set_playCommand4(str);
	  s >> fl; set_distance_temposcale(fl);
	  s >> fl; set_distance_spectrumweight(fl);
	  s >> b; set_limit_authornonplayed(b);
	  s >> b; set_shown_aboutbox(b);
	  s >> str; 
	  s >> str; set_mixer_command(str);
	  s >> b; set_open_mixer(b);
	}
      else if (magic == MAGIC_2_2)
	{
	  s >> w; set_file_count (w);
	  s >> w; set_yellowTime (w);
	  s >> w; set_orangeTime (w);
	  s >> w; set_redTime (w);
	  s >> w; set_filterBpm (w);
	  s >> b; set_color_range(b);
	  s >> b; set_color_played(b);
	  s >> b; set_color_authorplayed(b);
	  s >> b; set_color_ondisk(b);
	  s >> b; set_color_cues(b);
	  s >> b; set_color_dcolor(b);
	  s >> b; set_color_spectrum(b);
	  s >> b; set_authorDecay(b);
	  s >> b; set_limit_ondisk(b);
	  s >> b; set_limit_nonplayed(b);
	  s >> b; set_limit_uprange(b);
	  s >> b; set_limit_downrange(b);
	  s >> b; set_limit_indistance(b);
	  s >> str; set_playCommand1(str);
	  s >> str; set_playCommand2(str);
	  s >> str; set_playCommand3(str);
	  s >> str; set_playCommand4(str);
	  s >> fl; set_distance_temposcale(fl);
	  s >> fl; set_distance_spectrumweight(fl);
	  s >> b; set_limit_authornonplayed(b);
	  s >> b; set_shown_aboutbox(b);
	  s >> str; 
	  s >> str; set_mixer_command(str);
	  s >> b; set_open_mixer(b);
	  s >> b; set_ask_mix(b);
	  s >> b; set_auto_popqueue(b);
	}
      else if (magic == MAGIC_2_4)
	{
	  s >> w; set_file_count (w);
	  s >> w; set_yellowTime (w);
	  s >> w; set_orangeTime (w);
	  s >> w; set_redTime (w);
	  s >> w; set_filterBpm (w);
	  s >> b; set_color_range(b);
	  s >> b; set_color_played(b);
	  s >> b; set_color_authorplayed(b);
	  s >> b; set_color_ondisk(b);
	  s >> b; set_color_cues(b);
	  s >> b; set_color_dcolor(b);
	  s >> b; set_color_spectrum(b);
	  s >> b; set_authorDecay(b);
	  s >> b; set_limit_ondisk(b);
	  s >> b; set_limit_nonplayed(b);
	  s >> b; set_limit_uprange(b);
	  s >> b; set_limit_downrange(b);
	  s >> b; set_limit_indistance(b);
	  s >> str; set_playCommand1(str);
	  s >> str; set_playCommand2(str);
	  s >> str; set_playCommand3(str);
	  s >> str; set_playCommand4(str);
	  s >> fl; set_distance_temposcale(fl);
	  s >> fl; set_distance_spectrumweight(fl);
	  s >> b; set_limit_authornonplayed(b);
	  s >> b; set_shown_aboutbox(b);
	  s >> str; 
	  s >> str; set_mixer_command(str);
	  s >> b; set_open_mixer(b);
	  s >> b; set_ask_mix(b);
	  s >> b; set_auto_popqueue(b);
	  s >> str; set_record_command(str);
	  s >> str; set_replay_command(str);
	  s >> str; set_record_mixer_command(str);
	}
      else if (magic == MAGIC_2_5)
	{
	  s >> w; set_file_count (w);
	  s >> w; set_yellowTime (w);
	  s >> w; set_orangeTime (w);
	  s >> w; set_redTime (w);
	  s >> w; set_filterBpm (w);
	  s >> b; set_color_range(b);
	  s >> b; set_color_played(b);
	  s >> b; set_color_authorplayed(b);
	  s >> b; set_color_ondisk(b);
	  s >> b; set_color_cues(b);
	  s >> b; set_color_dcolor(b);
	  s >> b; set_color_spectrum(b);
	  s >> b; set_authorDecay(b);
	  s >> b; set_limit_ondisk(b);
	  s >> b; set_limit_nonplayed(b);
	  s >> b; set_limit_uprange(b);
	  s >> b; set_limit_downrange(b);
	  s >> b; set_limit_indistance(b);
	  s >> str; set_playCommand1(str);
	  s >> str; set_playCommand2(str);
	  s >> str; set_playCommand3(str);
	  s >> str; set_playCommand4(str);
	  s >> fl; set_distance_temposcale(fl);
	  s >> fl; set_distance_spectrumweight(fl);
	  s >> b; set_limit_authornonplayed(b);
	  s >> b; set_shown_aboutbox(b);
	  s >> str; 
	  s >> str; set_mixer_command(str);
	  s >> b; set_open_mixer(b);
	  s >> b; set_ask_mix(b);
	  s >> b; set_auto_popqueue(b);
	  s >> str; set_record_command(str);
	  s >> str; set_replay_command(str);
	  s >> str; set_record_mixer_command(str);
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
	  // read tag data
	  s >> w; 
	  while(w-->0)
	    {
	      QString t;
	      Q_INT8 i,m,e;
	      s >> t;
	      s >> i; 
	      s >> m; 
	      s >> e; 
	      new QListViewItem(get_taglist(), t, i ? TAG_TRUE : TAG_FALSE,
				m ? TAG_TRUE : TAG_FALSE,
				e ? TAG_TRUE : TAG_FALSE);
	    }
	  // read header
	  s >> w; 
	  for(int i = 0 ; i < w ; i++)
	    get_header()->addLabel("");
	  for(int i = 0 ; i < w ; i++)
	    {
	      Q_UINT16 m1,m2; s >> m1; s >> m2;
	      realize_mapping(get_header(),i,m1,m2);
	    }
	}
      else if (magic == MAGIC_2_6)
	{
	  s >> w; set_file_count (w);
	  s >> w; set_yellowTime (w);
	  s >> w; set_orangeTime (w);
	  s >> w; set_redTime (w);
	  s >> w; set_filterBpm (w);
	  s >> b; set_color_range(b);
	  s >> b; set_color_played(b);
	  s >> b; set_color_authorplayed(b);
	  s >> b; set_color_ondisk(b);
	  s >> b; set_color_cues(b);
	  s >> b; set_color_dcolor(b);
	  s >> b; set_color_spectrum(b);
	  s >> b; set_authorDecay(b);
	  s >> b; set_limit_ondisk(b);
	  s >> b; set_limit_nonplayed(b);
	  s >> b; set_limit_uprange(b);
	  s >> b; set_limit_downrange(b);
	  s >> b; set_limit_indistance(b);
	  s >> str; set_playCommand1(str);
	  s >> str; set_playCommand2(str);
	  s >> str; set_playCommand3(str);
	  s >> str; set_playCommand4(str);
	  s >> fl; set_distance_temposcale(fl);
	  s >> fl; set_distance_spectrumweight(fl);
	  s >> b; set_limit_authornonplayed(b);
	  s >> b; set_shown_aboutbox(b);
	  s >> str; 
	  s >> str; set_mixer_command(str);
	  s >> b; set_open_mixer(b);
	  s >> b; set_ask_mix(b);
	  s >> b; set_auto_popqueue(b);
	  s >> str; set_record_command(str);
	  s >> str; set_replay_command(str);
	  s >> str; set_record_mixer_command(str);
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
	  s >> str; set_bpm_mixer_command(str);
	  s >> b; set_open_bpmmixer(b);
	  // read tag data
	  s >> w; 
	  while(w-->0)
	    {
	      QString t;
	      Q_INT8 i,m,e;
	      s >> t;
	      s >> i; 
	      s >> m; 
	      s >> e; 
	      new QListViewItem(get_taglist(), t, i ? TAG_TRUE : TAG_FALSE,
				m ? TAG_TRUE : TAG_FALSE,
				e ? TAG_TRUE : TAG_FALSE);
	    }
	  // read header
	  s >> w; 
	  for(int i = 0 ; i < w ; i++)
	    get_header()->addLabel("");
	  for(int i = 0 ; i < w ; i++)
	    {
	      Q_UINT16 m1,m2; s >> m1; s >> m2;
	      realize_mapping(get_header(),i,m1,m2);
	    }
	}
      else if (magic == MAGIC_2_7)
	{
	  s >> w; set_file_count (w);
	  s >> w; set_yellowTime (w);
	  s >> w; set_orangeTime (w);
	  s >> w; set_redTime (w);
	  s >> w; set_filterBpm (w);
	  s >> b; set_color_range(b);
	  s >> b; set_color_played(b);
	  s >> b; set_color_authorplayed(b);
	  s >> b; set_color_ondisk(b);
	  s >> b; set_color_cues(b);
	  s >> b; set_color_dcolor(b);
	  s >> b; set_color_spectrum(b);
	  s >> b; set_authorDecay(b);
	  s >> b; set_limit_ondisk(b);
	  s >> b; set_limit_nonplayed(b);
	  s >> b; set_limit_uprange(b);
	  s >> b; set_limit_downrange(b);
	  s >> b; set_limit_indistance(b);
	  s >> str; set_playCommand1(str);
	  s >> str; set_playCommand2(str);
	  s >> str; set_playCommand3(str);
	  s >> str; set_playCommand4(str);
	  s >> fl; set_distance_temposcale(fl);
	  s >> fl; set_distance_spectrumweight(fl);
	  s >> b; set_limit_authornonplayed(b);
	  s >> b; set_shown_aboutbox(b);
	  s >> str; 
	  s >> str; set_mixer_command(str);
	  s >> b; set_open_mixer(b);
	  s >> b; set_ask_mix(b);
	  s >> b; set_auto_popqueue(b);
	  s >> str; set_record_command(str);
	  s >> str; set_replay_command(str);
	  s >> str; set_record_mixer_command(str);
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
	  s >> str; set_bpm_mixer_command(str);
	  s >> b; set_open_bpmmixer(b);
	  // read tag data
	  s >> w; 
	  while(w-->0)
	    {
	      QString t;
	      Q_INT8 i,m,e;
	      s >> t;
	      s >> i; 
	      s >> m; 
	      s >> e; 
	      new QListViewItem(get_taglist(), t, i ? TAG_TRUE : TAG_FALSE,
				m ? TAG_TRUE : TAG_FALSE,
				e ? TAG_TRUE : TAG_FALSE);
	    }
	  // read header
	  s >> w; 
	  for(int i = 0 ; i < w ; i++)
	    get_header()->addLabel("");
	  for(int i = 0 ; i < w ; i++)
	    {
	      Q_UINT16 m1,m2; s >> m1; s >> m2;
	      realize_mapping(get_header(),i,m1,m2);
	    }
	  float F;
	  s >> F; set_distance_tempoweight(F);
	  s >> F; set_distance_echoweight(F);
	  s >> F; set_distance_rythmweight(F);
	  s >> F; set_distance_compositionweight(F);
	  s >> w; set_max_songs(w);
	}
      else if (magic == MAGIC_2_8)
	{
	  s >> w; set_file_count (w);
	  s >> w; set_yellowTime (w);
	  s >> w; set_orangeTime (w);
	  s >> w; set_redTime (w);
	  s >> w; set_filterBpm (w);
	  s >> b; set_color_range(b);
	  s >> b; set_color_played(b);
	  s >> b; set_color_authorplayed(b);
	  s >> b; set_color_ondisk(b);
	  s >> b; set_color_cues(b);
	  s >> b; set_color_dcolor(b);
	  s >> b; set_color_spectrum(b);
	  s >> b; set_authorDecay(b);
	  s >> b; set_limit_ondisk(b);
	  s >> b; set_limit_nonplayed(b);
	  s >> b; set_limit_uprange(b);
	  s >> b; set_limit_downrange(b);
	  s >> b; set_limit_indistance(b);
	  s >> str; set_playCommand1(str);
	  s >> str; set_playCommand2(str);
	  s >> str; set_playCommand3(str);
	  s >> str; set_playCommand4(str);
	  s >> fl; set_distance_temposcale(fl);
	  s >> fl; set_distance_spectrumweight(fl);
	  s >> b; set_limit_authornonplayed(b);
	  s >> b; set_shown_aboutbox(b);
	  s >> str; 
	  s >> str; set_mixer_command(str);
	  s >> b; set_open_mixer(b);
	  s >> b; set_ask_mix(b);
	  s >> b; set_auto_popqueue(b);
	  s >> str; set_record_command(str);
	  s >> str; set_replay_command(str);
	  s >> str; set_record_mixer_command(str);
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
	  s >> str; set_bpm_mixer_command(str);
	  s >> b; set_open_bpmmixer(b);
	  // read tag data
	  s >> w; 
	  while(w-->0)
	    {
	      QString t;
	      Q_INT8 i,m,e;
	      s >> t;
	      s >> i; 
	      s >> m; 
	      s >> e; 
	      new QListViewItem(get_taglist(), t, i ? TAG_TRUE : TAG_FALSE,
				m ? TAG_TRUE : TAG_FALSE,
				e ? TAG_TRUE : TAG_FALSE);
	    }
	  // read header
	  s >> w; 
	  for(int i = 0 ; i < w ; i++)
	    get_header()->addLabel("");
	  for(int i = 0 ; i < w ; i++)
	    {
	      Q_UINT16 m1,m2; s >> m1; s >> m2;
	      realize_mapping(get_header(),i,m1,m2);
	    }
	  float F;
	  s >> F; set_distance_tempoweight(F);
	  s >> F; set_distance_echoweight(F);
	  s >> F; set_distance_rythmweight(F);
	  s >> F; set_distance_compositionweight(F);
	  s >> w; set_max_songs(w);
	  s >> clr; set_color_unchecked(clr);
	}
      else if (magic == MAGIC_2_9)
	{
	  s >> w; set_file_count (w);
	  s >> w; set_yellowTime (w);
	  s >> w; set_orangeTime (w);
	  s >> w; set_redTime (w);
	  s >> w; set_filterBpm (w);
	  s >> b; set_color_range(b);
	  s >> b; set_color_played(b);
	  s >> b; set_color_authorplayed(b);
	  s >> b; set_color_ondisk(b);
	  s >> b; set_color_cues(b);
	  s >> b; set_color_dcolor(b);
	  s >> b; set_color_spectrum(b);
	  s >> b; set_authorDecay(b);
	  s >> b; set_limit_ondisk(b);
	  s >> b; set_limit_nonplayed(b);
	  s >> b; set_limit_uprange(b);
	  s >> b; set_limit_downrange(b);
	  s >> b; set_limit_indistance(b);
	  s >> str; set_playCommand1(str);
	  s >> str; set_playCommand2(str);
	  s >> str; set_playCommand3(str);
	  s >> str; set_playCommand4(str);
	  s >> fl; set_distance_temposcale(fl);
	  s >> fl; set_distance_spectrumweight(fl);
	  s >> b; set_limit_authornonplayed(b);
	  s >> b; set_shown_aboutbox(b);
	  s >> str; set_mixer_command(str);
	  s >> b; set_open_mixer(b);
	  s >> b; set_ask_mix(b);
	  s >> b; set_auto_popqueue(b);
	  s >> str; set_record_command(str);
	  s >> str; set_replay_command(str);
	  s >> str; set_record_mixer_command(str);
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
	  s >> str; set_bpm_mixer_command(str);
	  s >> b; set_open_bpmmixer(b);
	  // read tag data
	  s >> w; 
	  while(w-->0)
	    {
	      QString t;
	      Q_INT8 i,m,e;
	      s >> t;
	      s >> i; 
	      s >> m; 
	      s >> e; 
	      new QListViewItem(get_taglist(), t, i ? TAG_TRUE : TAG_FALSE,
				m ? TAG_TRUE : TAG_FALSE,
				e ? TAG_TRUE : TAG_FALSE);
	    }
	  // read header
	  s >> w; 
	  for(int i = 0 ; i < w ; i++)
	    get_header()->addLabel("");
	  for(int i = 0 ; i < w ; i++)
	    {
	      Q_UINT16 m1,m2; s >> m1; s >> m2;
	      realize_mapping(get_header(),i,m1,m2);
	    }
	  float F;
	  s >> F; set_distance_tempoweight(F);
	  s >> F; set_distance_echoweight(F);
	  s >> F; set_distance_rythmweight(F);
	  s >> F; set_distance_compositionweight(F);
	  s >> w; set_max_songs(w);
	  s >> clr; set_color_unchecked(clr);
	  s >> str; set_analCommand1(str);
	  s >> str; set_analCommand2(str);
	  s >> str; set_analCommand3(str);
	  s >> str; set_analCommand4(str);
	  s >> str; set_analCommand5(str);
	  s >> str; set_analCommand6(str);
	  s >> str; set_analCommand7(str);
	  s >> str; set_analCommand8(str);
	  s >> fl;  set_anal_bpm_from(fl);
	  s >> fl;  set_anal_bpm_to(fl);
	  s >> w;   set_anal_bpm_technique(w);
	}
      else
	printf("Wrong config file format\n");
      if (magic != MAGIC_NOW)
	{
	  set_shown_aboutbox(false);
	  if (MAGIC_NOW == MAGIC_2_1)
	    {
	      set_playCommand1("");
	      set_playCommand2("");
	      set_playCommand3("");
	      set_playCommand4("");
	    }
	}
      //      dump_header(get_header());
      calc_and_cache();
    }
}

bool Config::open_ui(int pane)
{
  char tmp[50];
  PreferencesLogic preferences(NULL,NULL,TRUE);
  preferences.playerCommand1 -> setText ( get_playCommand1() ) ;
  preferences.playerCommand2 -> setText ( get_playCommand2() ) ;
  preferences.playerCommand3 -> setText ( get_playCommand3() ) ;
  preferences.playerCommand4 -> setText ( get_playCommand4() ) ;

  preferences.analyzerCommand1 -> setText( get_analCommand1() );
  preferences.analyzerCommand2 -> setText( get_analCommand2() );
  preferences.analyzerCommand3 -> setText( get_analCommand3() );
  preferences.analyzerCommand4 -> setText( get_analCommand4() );
  preferences.analyzerCommand5 -> setText( get_analCommand5() );
  preferences.analyzerCommand6 -> setText( get_analCommand6() );
  preferences.analyzerCommand7 -> setText( get_analCommand7() );
  preferences.analyzerCommand8 -> setText( get_analCommand8() );

  sprintf(tmp,"%d",get_yellowTime());
  preferences.yellowTime->setText(tmp);
  sprintf(tmp,"%d",get_orangeTime());
  preferences.orangeTime->setText(tmp);
  sprintf(tmp,"%d",get_redTime());
  preferences.redTime->setText(tmp);
  preferences.greenTimeColor -> setPaletteBackgroundColor( get_color_green_time ());
  preferences.yellowTimeColor -> setPaletteBackgroundColor( get_color_yellow_time ());
  preferences.orangeTimeColor -> setPaletteBackgroundColor( get_color_orange_time ());
  preferences.redTimeColor -> setPaletteBackgroundColor( get_color_red_time ());
  preferences.color_main_window -> setChecked(get_color_main_window());
  
  preferences.authorDecay  -> setValue ( get_authorDecay() ) ;
  preferences.tempoSpin    -> setValue ( lround(get_distance_temposcale()*100.0) ) ;
  preferences.mixerCommand -> setText  ( get_mixer_command() ) ;
  preferences.record       -> setText  ( get_record_command() ) ;
  preferences.record_mixer -> setText  ( get_record_mixer_command() ) ;
  preferences.replay       -> setText  ( get_replay_command() ) ;

  preferences.colorTempo44 -> setPaletteBackgroundColor( get_color_tempo44 ());
  preferences.colorTempo54 -> setPaletteBackgroundColor( get_color_tempo54 ());
  preferences.colorTempo64 -> setPaletteBackgroundColor( get_color_tempo64 ());
  preferences.colorTempo74 -> setPaletteBackgroundColor( get_color_tempo74 ());
  preferences.colorTempo84 -> setPaletteBackgroundColor( get_color_tempo84 ());

  preferences.showTempo54 -> setChecked( get_show_tempo54 ());
  preferences.showTempo64 -> setChecked( get_show_tempo64 ());
  preferences.showTempo74 -> setChecked( get_show_tempo74 ());
  preferences.showTempo84 -> setChecked( get_show_tempo84 ());
  preferences.showUnknownTempo -> setChecked( get_show_unknown_tempo ());

  preferences.colorPlayedSong -> setPaletteBackgroundColor( get_color_played_song ());
  preferences.colorPlayedAuthor -> setPaletteBackgroundColor( get_color_played_author ());
  preferences.colorUnavailableSong -> setPaletteBackgroundColor( get_color_unavailable ());
  preferences.colorUncheckedSong -> setPaletteBackgroundColor( get_color_unchecked ());
  preferences.colordColorCol -> setPaletteBackgroundColor( get_color_dcolor_col ());

  preferences.dColorMetric->spectrumSpin -> setValue ( (int)(get_distance_spectrumweight()*100.0) );
  preferences.dColorMetric->tempoDistanceSpin -> setValue( (int)(get_distance_tempoweight()*100.0) );
  preferences.dColorMetric->echoSpin -> setValue( (int)(get_distance_echoweight()*100.0) );
  preferences.dColorMetric->rythmSpin -> setValue( (int)(get_distance_rythmweight()*100.0) );
  preferences.dColorMetric->compositionSpin -> setValue( (int)(get_distance_compositionweight()*100.0) );
  preferences.max_songs->setValue(get_max_songs());

  preferences.clusterDepth -> setValue (get_color_cluster_depth ());

  preferences.bpmmixcmd -> setText( get_bpm_mixer_command () );
  preferences.subtabs -> setCurrentItem( pane );

  preferences.From -> setText(get_anal_bpm_from() ? QString::number(get_anal_bpm_from()) : "");
  preferences.To   -> setText(get_anal_bpm_to() ? QString::number(get_anal_bpm_to()): "");
  preferences.bpmalgo -> setButton(get_anal_bpm_technique()-1);

  if (preferences.exec()==QDialog::Accepted)
    {
      set_playCommand1(preferences.playerCommand1->text());
      set_playCommand2(preferences.playerCommand2->text());
      set_playCommand3(preferences.playerCommand3->text());
      set_playCommand4(preferences.playerCommand4->text());

      set_analCommand1(preferences.analyzerCommand1->text());
      set_analCommand2(preferences.analyzerCommand2->text());
      set_analCommand3(preferences.analyzerCommand3->text());
      set_analCommand4(preferences.analyzerCommand4->text());
      set_analCommand5(preferences.analyzerCommand5->text());
      set_analCommand6(preferences.analyzerCommand6->text());
      set_analCommand7(preferences.analyzerCommand7->text());
      set_analCommand8(preferences.analyzerCommand8->text());

      set_yellowTime(atoi(preferences.yellowTime->text()));
      set_orangeTime(atoi(preferences.orangeTime->text()));
      set_redTime(atoi(preferences.redTime->text()));
      set_authorDecay ( preferences.authorDecay->value());
      set_distance_temposcale ( (float)(preferences.tempoSpin->value())/100.0);

      set_mixer_command ( preferences.mixerCommand->text());
      set_record_command ( preferences.record->text());
      set_replay_command ( preferences.replay->text());
      set_record_mixer_command ( preferences.record_mixer->text());
      
      set_color_tempo44(preferences.colorTempo44->paletteBackgroundColor());
      set_color_tempo54(preferences.colorTempo54->paletteBackgroundColor());
      set_color_tempo64(preferences.colorTempo64->paletteBackgroundColor());
      set_color_tempo74(preferences.colorTempo74->paletteBackgroundColor());
      set_color_tempo84(preferences.colorTempo84->paletteBackgroundColor());

      set_show_tempo54 (preferences.showTempo54->isChecked());
      set_show_tempo64 (preferences.showTempo64->isChecked());
      set_show_tempo74 (preferences.showTempo74->isChecked());
      set_show_tempo84 (preferences.showTempo84->isChecked());
      set_show_unknown_tempo(preferences.showUnknownTempo->isChecked());
      
      set_color_red_time(preferences.redTimeColor -> paletteBackgroundColor( ));
      set_color_orange_time(preferences.orangeTimeColor -> paletteBackgroundColor( ));
      set_color_yellow_time(preferences.yellowTimeColor -> paletteBackgroundColor( ));
      set_color_green_time(preferences.greenTimeColor -> paletteBackgroundColor( ));
      set_color_main_window(preferences.color_main_window->isChecked());

      set_color_played_song(preferences.colorPlayedSong -> paletteBackgroundColor( ));
      set_color_played_author(preferences.colorPlayedAuthor -> paletteBackgroundColor( ));
      set_color_unavailable(preferences.colorUnavailableSong -> paletteBackgroundColor( ));
      set_color_unchecked(preferences.colorUncheckedSong -> paletteBackgroundColor( ));
      set_color_dcolor_col(preferences.colordColorCol -> paletteBackgroundColor( ));
      set_color_cluster_depth(preferences.clusterDepth -> value());
      set_bpm_mixer_command(preferences.bpmmixcmd -> text());
      
      set_distance_tempoweight       ( (float)(preferences.dColorMetric->tempoDistanceSpin->value())/100.0);
      set_distance_spectrumweight    ( (float)(preferences.dColorMetric->spectrumSpin->value())/100.0);
      set_distance_echoweight        ( (float)(preferences.dColorMetric->echoSpin->value())/100.0);
      set_distance_rythmweight       ( (float)(preferences.dColorMetric->rythmSpin->value())/100.0);
      set_distance_compositionweight ( (float)(preferences.dColorMetric->compositionSpin->value())/100.0);
      set_max_songs(preferences.max_songs->value());

      set_anal_bpm_from(atof(preferences.From -> text()));
      set_anal_bpm_to(  atof(preferences.To   -> text()));
      set_anal_bpm_technique(preferences.bpmalgo -> selectedId()+1);
      if (get_anal_bpm_from()>get_anal_bpm_to())
	{
	  float t=get_anal_bpm_from();
	  set_anal_bpm_from(get_anal_bpm_to());
	  set_anal_bpm_to(t);
	}

      save();
      calc_and_cache();
      return true;
    }
  return false;
}
/*      if (bounds->resamplingScan->isChecked()) technique = 1;
      else if (bounds->ultraLongFFT->isChecked()) technique = 2;
      else if (bounds->enveloppeSpectrum->isChecked()) technique = 3;
      else if (bounds->fullAutoCorrelation->isChecked()) technique = 4;
*/

void Config::calc_and_cache()
{
  SongMetriek::std.set_tempo_limits(get_distance_temposcale());
  SongMetriek::std.set_show_harmonics(get_show_tempo54(),get_show_tempo64(),get_show_tempo74(),get_show_tempo84());
  SongMetriek::std.set_unknown_tempo_matches(get_show_unknown_tempo());
  SongMetriek::std.set_tempo_weight(get_distance_tempoweight());
  SongMetriek::std.set_spectrum_weight(get_distance_spectrumweight());
  SongMetriek::std.set_echo_weight(get_distance_echoweight());
  SongMetriek::std.set_rythm_weight(get_distance_rythmweight());
  SongMetriek::std.set_composition_weight(get_distance_compositionweight());
  SongMetriek::std.prepare();
}
