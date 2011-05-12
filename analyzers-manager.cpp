/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2004 Werner Van Belle
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

#include <unistd.h>
#include <dirent.h>
#include <qmessagebox.h>
#include <ctype.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qmultilineedit.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <libgen.h>
#include "preferences.h"
#include "about.h"
#include "songselector.logic.h"
#include "history.h"
#include "version.h"
#include "kbpm-dj.h"
#include "analyzers-manager.h"
#include "qsong.h"
#include "config.h"
#include "scripts.h"
#include "memory.h"

AnalyzersManager::AnalyzersManager(int count, ProcessChanged * sel) :
  BasicProcessManager(count,sel)
{
};

void AnalyzersManager::clearId(int id)
{
  ((AnalyzersProgressLogic*)listener)->startAnother(id);
}
