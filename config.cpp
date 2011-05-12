/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001 Werner Van Belle
 See 'BeatMixing.ps' for more information

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
#include <stdlib.h>
#include "preferences.h"
#include "config.h"

int  Config::file_count = 1000;
int  Config::yellowTime = 120;   
int  Config::orangeTime = 150;
int  Config::redTime = 180; 
int  Config::filterBpm;
int  Config::authorDecay = 5;
bool Config::color_range = true;
bool Config::color_played = true;
bool Config::color_authorplayed = true;
QString Config::playCommand1 = "kbpm-play -d /dev/dsp2 -x /dev/mixer1 -p 0 0   -m \"%s\" \"%s\"";
QString Config::playCommand2 = "kbpm-play -d /dev/dsp1 -x /dev/mixer  -p 0 400 -m \"%s\" \"%s\"";

void Config::save()
{
  QFile f(".kbpmdj");
  f.open(IO_WriteOnly);
  QDataStream s(&f);
  s << (Q_UINT16)0xBDE0;
  s << (Q_UINT16)file_count;
  s << (Q_UINT16)yellowTime;
  s << (Q_UINT16)orangeTime;
  s << (Q_UINT16)redTime;
  s << (Q_UINT16)filterBpm;
  s << (Q_INT8)color_range;
  s << (Q_INT8)color_played;
  s << (Q_INT8)color_authorplayed;
  s << (Q_INT8)authorDecay;
  s << playCommand1;
  s << playCommand2;
}

void Config::load()
{
  if (QFile::exists(".kbpmdj"))
    {
      Q_INT8 b;
      Q_UINT16 w;
      QFile f(".kbpmdj");
      f.open(IO_ReadOnly);
      QDataStream s(&f);
      s >> w;
      if (w == 0xBDE0)
	{
	  printf("Loading config v1.6\n");
	  s >> w; file_count = w;
	  s >> w; yellowTime = w;
	  s >> w; orangeTime = w;
	  s >> w; redTime = w;
	  s >> w; filterBpm = w;
	  s >> b; color_range = b;
	  s >> b; color_played = b;
	  s >> b; color_authorplayed = b;
	  s >> b; authorDecay = b;
	  s >> playCommand1;
	  s >> playCommand2;
	}
      else
	printf("Wrong config file format\n");
    }
}

void Config::openUi()
{
  char tmp[50];
  PreferencesDialog preferences(NULL,NULL,TRUE);
  preferences.playerCommand1->setText(playCommand1);
  preferences.playerCommand2->setText(playCommand2);
  sprintf(tmp,"%d",yellowTime);
  preferences.yellowTime->setText(tmp);
  sprintf(tmp,"%d",orangeTime);
  preferences.orangeTime->setText(tmp);
  sprintf(tmp,"%d",redTime);
  preferences.redTime->setText(tmp);
  preferences.authorDecay->setValue(authorDecay);
  if (preferences.exec()==QDialog::Accepted)
    {
      playCommand1=preferences.playerCommand1->text();
      playCommand2=preferences.playerCommand2->text();
      yellowTime=atoi(preferences.yellowTime->text());
      orangeTime=atoi(preferences.orangeTime->text());
      redTime=atoi(preferences.redTime->text());
      authorDecay=preferences.authorDecay->value();
      save();
    }
}

