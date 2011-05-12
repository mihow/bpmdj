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

#include <qapplication.h>
#include <qmessagebox.h>
#include <qlistview.h>
#include <qlcdnumber.h>
#include <qheader.h>
#include <qgroupbox.h>
#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <qradiobutton.h>
#include <libgen.h>
#include <time.h>
#include <sys/times.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include "memory.h"
#include "common.h"
#include "analyzers-progress.logic.h"

/*-------------------------------------------
 *         Parsing arguments 
 *-------------------------------------------*/
QApplication *app;
AnalyzersProgressLogic *progress;

void options_failure(char* err)
{
  QMessageBox::critical(NULL,
			"Command line arguments",
			QString(err),
			QMessageBox::Ok,QMessageBox::NoButton);
  printf("Usage:  kbpm-batch [<user>@<host>]*\n\n"
	 "   E.g: kbpm-batch localhost werner@asus werner@orbit root@tnm\n"
	 "   See the website for more details\n\n"
	 "  %s\n\n",err);
  exit(1);
}

bool show_error(int err, int err2, const char*text)
{
  if (err==err2)
    {
      const QString a=QString("Error");
      const QString b=QString(text);
      QMessageBox::critical(NULL,a,b,QMessageBox::Ok,0,0);
      return true;
    }
  return false;
}

void * go(void* neglect)
{
  app->exec();
  delete(progress);
  delete(app);
  return neglect;
}

int main(int argc, char *argv[])
{
  // create application
  app = new QApplication(argc,argv);
  // counter number of hosts
  int arguments = 0;
  for(int i = 1 ; i < argc ; i ++)
    if (argv[i][0]!='-')
      arguments++;
  if (arguments==0)
    options_failure("requires at least one host");
  // and create new window
  progress = new AnalyzersProgressLogic(arguments);
  // make it the main window
  app->setMainWidget(progress);
  progress->show();
  // install all hosts
  for(int i = 1 ; i < argc ; i ++)
    if (argv[i][0]!='-')
      progress->addHost(--arguments,argv[i]);
  // fetch all commands
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  FILE * f = fopen("analyze.sh","rb");
  if (!f) options_failure("The file analyze.sh doesn't exist");
  while ((read = getline(&line, &len, f)) != -1) 
    {
      //printf("Retrieved line of length %zu :\n", read);
      // printf("%d ", line[len-3]);
      if ( read > 0 && line [ read -1 ] == '\n' )
	line[read-1]=0;
      progress->addLine(line);
    }
  if (line)
    free(line);
  // start the batch processing
  progress->start();
  go(NULL);
}
