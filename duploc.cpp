/****
 Duploc: Duplocate Line Locator
 Copyright (C) 2003 Werner Van Belle, based on work done
                    at the universirty of Bern.

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
#include <qlineedit.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qlcdnumber.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qprogressbar.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <qprogressdialog.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <qtextbrowser.h>
#include <assert.h>
#include <qmessagebox.h>
#include <ctype.h>
#include "duploc.h"

int arg_count;
char * * arg_array;

Duploc::Duploc(QWidget *parent, const char * name, bool modal, WFlags f) :
  DuplocDialog(parent,name,modal,f)
{
  line = (char**)malloc(sizeof(char*)*1);
  orig = (char**)malloc(sizeof(char*)*1);
  lines = 1;
  lastline = 0;
  readFiles();
  // dump();
  correlate();
  fold();
  showPattern();
}

void Duploc::readFiles()
{
  for(int i = 1 ; i < arg_count ; i ++)
    readFile(arg_array[i]);
}

char * readline(FILE * f)
{
  // lees tekens tot we aan de '\n' zitten...
  int pos=0;
  char line[1000];
  int c;
  while ( pos < 1000 && (c = fgetc(f)) != -1 && c!='\n')
    line[pos++]=c;
  assert(pos!=1000);
  line[pos]=0;
  if (c==-1) return 0;
  return strdup(line);
}

char * stripline(char * line_in)
{
  int pos_in = 0;
  int pos_out = 0;
  char line_out[1000];
  
  // strip beginnen spaces
  while( line_in[pos_in] && 
	 ( line_in[pos_in]==' '
	   || line_in[pos_in]=='\t') )
    pos_in++;
  
  // copy till the end
  while ( ( line_out[pos_out++] = toupper(line_in[pos_in++]) ) ) ;
  
  // remove trailing
  pos_out--;
  while ( pos_out>=0 && 
	  (line_out[pos_out] == '\t' || line_out[pos_out]==' ') )
    line_out[pos_out--]=0;
  
  return strdup(line_out);
}

void Duploc::addline(char* filename, int nr, char * o, char * l)
{
  if (lastline>=lines)
    {
      lines*=2;
      line = (char**)realloc(line,sizeof(char*)*lines);
      orig = (char**)realloc(orig,sizeof(char*)*lines);
    }
  char str[1000];
  sprintf(str,"%s(%4d) : %s",filename,nr,o);
  orig[lastline] = strdup(str);
  line[lastline++]=l;
}

void Duploc::readFile(char* filename)
{
  printf("Reading %s\n",filename);
  FILE * file = fopen(filename,"rb");
  assert(file);
  char * line, * orig;
  int nr = 0;
  while ( (orig = readline(file)) )
    {
      nr++;
      line = stripline(orig);
      if (*line)
	{
	  if (line[0]=='}' && !line[1])
	    continue;
	  if (line[0]=='{' && !line[1])
	    continue;
	  addline(filename,nr,orig,line);
	}
    }
  fclose(file);
}

void Duploc::showPattern()
{
  QPixmap * pm = new QPixmap(keepxc,keepyc);
  QPainter p;
  p.begin(pm);
  p.fillRect(0,0,lastline,lastline,Qt::white);
  p.setPen(Qt::black);
  for(int i = 0; i < keepxc ; i ++)
    for(int j = 0 ; j < keepyc ; j ++)
      if (matrix[mapx[i]][mapy[j]])
	p.drawPoint(i,j);
  p.end();
  matrixview->setPixmap(*pm);
}

void Duploc::dump()
{
  for(int i = 0 ; i < lastline; i ++)
    printf("%s\n",line[i]);
}

void Duploc::correlate()
{
  printf("Started correlating: %d lines\n",lastline);
  matrix = (bool**)malloc(lastline*sizeof(bool*));
  for(int i = 0 ; i < lastline; i ++)
    matrix[i]=(bool*)malloc(lastline*sizeof(bool));
  for(int i = 0 ; i < lastline ; i ++)
    for(int j = 0 ; j < lastline ; j ++)
      matrix[i][j]=false;
  for(int i = 0 ; i < lastline - 1; i ++)
    for(int j = i + 1 ; j < lastline; j ++)
      matrix[i][j]=strcmp(line[i],line[j])==0;
}

void Duploc::fold()
{
  printf("Started folding: %d lines\n",lastline);
  keepx = (bool*)malloc(lastline*sizeof(bool));
  keepy = (bool*)malloc(lastline*sizeof(bool));
  for(int i = 0 ; i < lastline ; i++)
    {
      keepx[i]=true;
      keepy[i]=false;
    }
  for(int i = 0 ; i < lastline - 1; i ++)
    for(int j = i ; j < lastline - 1 ; j ++)
      {
	if (matrix[i][j] && matrix[i+1][j+1])
	  {
	    keepx[i]=keepy[j]=true;
	    keepx[i+1]=keepy[j+1]=true;
	    break;
	  }
      } 
  keepxc = 0;
  keepyc = 0;
  for(int i = 0 ; i < lastline ; i++)
    if (keepx[i]) keepxc++;
  for(int i = 0 ; i < lastline ; i++)
    if (keepy[i]) keepyc++;
  mapx = (int*)malloc(sizeof(int)*keepxc);
  mapy = (int*)malloc(sizeof(int)*keepyc);
  keepxc = 0;
  keepyc = 0;
  for(int i = 0 ; i < lastline ; i++)
    if (keepx[i]) mapx[keepxc++]=i;
  for(int i = 0 ; i < lastline ; i++)
    if (keepy[i]) mapy[keepyc++]=i;
  printf("%d -- %d\n",keepxc,keepyc);
  //for(int i = 0 ; i < keepxc ; i++)
  //printf("%d to %d\n",i,mapx[i]);
}
  
void Duploc::mousePressEvent(QMouseEvent * e)
{
  int x=e->x()-matrixview->x();
  int y=e->y()-matrixview->y();
  x = (int)((double)keepxc*(double)x/(double)(matrixview->width()));
  y = (int)((double)keepyc*(double)y/(double)(matrixview->height()));
  printf("Clicked at position %d,%d\n",x,y);
  int a=x,b=y,c=x,d=y;
  int result = 0;
  bool A = true;
  while( !result &&
	 ( a >=0 || 
	   b < keepyc || 
	   c < keepxc ||  
	   d >= 0 ) )
    {
      if (a>=0 && b<keepyc)
	if (matrix[mapx[a]][mapy[b]])
	  {
	    printf("Match %d,%d\n -- %s \n -- %s \n",mapx[a],mapy[b],
		   line[mapx[a]],line[mapy[b]]);
	    result = 2;
	    break;
	  }
      if (c<keepxc && d>=0)
	if (matrix[mapx[c]][mapy[d]])
	  {
	    printf("Match %d,%d\n -- %s \n -- %s \n",mapx[c],mapy[d],
		   line[mapx[c]],line[mapy[d]]);
	    result = 1;
	    break;
	  }
      if (A)
	{
	  a--;
	  b++;
	}
      else
	{
	  c++;
	  d--;
	}
      A=!A;
    }
  // found the nearest match...
  if (result == 0) return;
  if (result == 1) {a = c; b = d; };
  printf("Nearest match is %d,%d\n",a,b);
  a = mapx[a];
  b = mapy[b];
  printf("Which brings us to lines %d,%d\n",a,b);
  QString left="", right="";
  for (int i = 0; i < 10 ; i ++)
    {
      left+=QString(orig[i+a])+"\n";
      right+=QString(orig[i+b])+"\n";
    }
  lefttext->setText(left);
  righttext->setText(right);
}


int main(int argc, char* argv[])
{
  arg_count = argc;
  arg_array = argv;
  QApplication * app = new QApplication(argc,argv);
  Duploc *duploc = new Duploc();
  app->setMainWidget(duploc);
  duploc->show();
  app->exec();
}
