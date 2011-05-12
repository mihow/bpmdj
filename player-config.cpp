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

#include <qstring.h>
#include <qfile.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qdialog.h>
#include <qcombobox.h>
#include "player-config.h"
#include "dsp-none.h"
#include "dsp-alsa.h"
#include "dsp-oss.h"
#include "dsp-mixed.h"
#include "magic.h"
#include "songplayer.logic.h"
#include "capacity.h"
#include "scripts.h"

PlayerConfig::PlayerConfig()
{
  set_meta_name(QString(".kbpm-play"));
  init();
  load();
  save();
}

PlayerConfig::PlayerConfig(QString configname)
{
  set_meta_name(QString(".")+QString(configname)+QString(".kbpm-play"));
  init();
  load();
}

void PlayerConfig::init()
{
  init_ui_posx();
  init_ui_posy();
  init_core_rawpath();
  init_player_dsp();
  init_alsa_latency();
  init_alsa_verbose();
  init_alsa_dev(); 
  init_oss_dsp();
  init_oss_init_fragments();
  init_oss_fragments();
  init_oss_verbose();
  init_oss_nolatencyaccounting();
  init_oss_latency();
  init_mixed_channel();
  init_player_rms();
  init_player_rms_target();
  init_disabled_capacities();
}

void PlayerConfig::load_ui_position(QDialog * dialog)
{
  if (get_ui_posx()>-1 && get_ui_posy()>-1)
    dialog->move(get_ui_posx(),get_ui_posy());
}

void PlayerConfig::save_ui_position(QDialog * dialog)
{
  set_ui_posx(dialog->x());
  set_ui_posy(dialog->y());
  save();
}

void PlayerConfig::save()
{
  QFile f(get_meta_name());
  f.open(IO_WriteOnly);
  if (!f.isOpen() || !f.isWritable())
    {
      Error(true,"Configuration file \"%s\" cannot be opened for writing",(const char*)get_meta_name());
      return;
    }
  QDataStream s(&f);
  s << (Q_UINT16)MAGIC_NOW;
  s << get_core_rawpath();
  s << (Q_UINT16)get_player_dsp();
  s << (Q_UINT16)get_alsa_latency();
  s << (Q_INT8)get_alsa_verbose();
  s << get_alsa_dev();
  s << get_oss_dsp();
  s << (Q_INT8)get_oss_init_fragments();
  s << (Q_UINT16)get_oss_fragments();
  s << (Q_INT8)get_oss_verbose();
  s << (Q_INT8)get_oss_nolatencyaccounting();
  s << (Q_UINT16)get_oss_latency();
  s << (Q_UINT16)get_mixed_channel();
  s << (Q_INT8)get_player_rms();
  s << get_player_rms_target();
  s << (Q_UINT16)get_disabled_capacities();
  s << (Q_UINT16)get_ui_posx();
  s << (Q_UINT16)get_ui_posy();
}


void PlayerConfig::load()
{
  if (!QFile::exists(get_meta_name())) return;
  Q_INT8 b;
  Q_UINT16 magic;
  Q_UINT16 w;
  QString str;
  float fl;
  QFile f(get_meta_name());
  f.open(IO_ReadOnly);
  QDataStream s(&f);
  s >> magic;
  if (magic == MAGIC_2_9)
	{
	  s >> str; set_core_rawpath(str);
	  s >> w; set_player_dsp(w);
	  s >> w; set_alsa_latency(w);
	  s >> b; set_alsa_verbose(b);
	  s >> str; set_alsa_dev(str);
	  s >> str; set_oss_dsp(str);
	  s >> b; set_oss_init_fragments(b);
	  s >> w; set_oss_fragments(w);
	  s >> b; set_oss_verbose(b);
	  s >> b; set_oss_nolatencyaccounting(b);
	  s >> w; set_oss_latency(w);
	  s >> w; set_mixed_channel(w);
	  s >> b; set_player_rms(b);
	  s >> fl; set_player_rms_target(fl);
	  s >> w; set_disabled_capacities(w);
	  s >> w; set_ui_posx(w);
	  s >> w; set_ui_posy(w);
	}
  else
    printf("Kbpm-play wrong config file format\n");
}
