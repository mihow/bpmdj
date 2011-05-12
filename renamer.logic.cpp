#include <assert.h>
#include <ctype.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <stdlib.h>
#include <stdio.h>
#include "renamer.logic.h"

RenamerLogic::RenamerLogic(QWidget*parent,const QString name) :
  Renamer(parent,name),
  DirectoryScanner("")
{
}

void RenamerLogic::checkfile(const QString  pathname, const QString  filename)
{
  QString fullname = pathname+"/"+filename;
  add(filename,fullname);
}

bool RenamerLogic::matchextension(const QString  filename)
{
  return filename.contains(".mp3",0) +
    filename.contains(".idx",0);
}

bool RenamerLogic::goodName(QString name)
{
   // must end on .idx or .mp3
   QString trail = name.right(4);
   if (trail!=".mp3" && trail!=".idx")
     return false;
   // no spaces or any special characters
   // should occur anywhere in the filename
   if (name.contains(" ")) return false;
   if (name.contains("_")>1) return false;
   if (name.contains("_")==1)
     {
	int pos = name.findRev("_");
	int pos2 = name.find("[");
	if ((pos2-pos>3) || (pos<2))
	  return false;
     }
   if (name.contains("[")!=1) return false;
   if (name.contains("]")!=1) return false;
   if (name.find("[")>name.find("]")) return false;
   return true;
}

void RenamerLogic::add(const QString name, const QString pos)
{
  // check whether the filename is good enough
  if (!goodName(name))
    new QListViewItem(NameList,name,name,pos);
}

QString RenamerLogic::smallCapsInWord(QString in)
{
   char* out = strdup((const char*)in);
   char i = 0;
   bool prevupper = false;
   while(out[i])
     {
	if (isupper(out[i]))
	  {
	     if (prevupper)
	       out[i]=tolower(out[i]);
	     prevupper=true;
	  }
	else
	  {
	     prevupper=false;
	  }
	i++;
     }
   return QString(out);
}

QString RenamerLogic::capitalizeAfterSpace(QString in)
{
   char* out = strdup((const char*)in);
   char i = 0;
   bool prevspace = true;
   while(out[i])
     {
	if (islower(out[i]))
	  {
	     if (prevspace)
	       out[i]=toupper(out[i]);
	  }
	prevspace = out[i]==' ' || out[i]=='_';
	i++;
     }
   return QString(out);
}

QString RenamerLogic::removeSpaces(QString in)
{
   char * out = strdup((const char*)in);
   int i = 0;
   int j = 0;
   while(out[i])
     {
	if (out[i] != ' ')
	  {
	     out[j] = out[i];
	     j++;
	  }
	i++;
     }
   out[j]=out[i];
   return QString(out);
}

QString RenamerLogic::removeSpecials(QString in)
{
   char * out = strdup((const char*)in);
   int i = 0;
   int j = 0;
   while(out[i])
     {
	if (isalnum(out[i]) || out[i]=='&'
	    || out[i]=='(' || out[i]==')'
	    || out[i]=='[' || out[i]==']'
	    || out[i]=='{' || out[i]=='}'
	    || (out[i]=='.' && i == in.length()-4))
	  {
	     out[j]=out[i];
	     j++;
	  }
	i++;
     }
   out[j]=out[i];
   return QString(out);
}

void RenamerLogic::smallCapsInWords()
{
   QListViewItemIterator it(NameList);
   for(;it.current();++it)
     {
	QListViewItem * item = it.current();
	if (item->isSelected())
	  item->setText(0,smallCapsInWord(item->text(0)));
     }
}

QString RenamerLogic::replaceSubString(QString in)
{
   QString key = SubString->text();
   QString replace = ReplaceString->text();
   int pos = in.find(key);
   if (pos!=-1)
     return in.replace(pos,key.length(),replace);
   return in;
}

void RenamerLogic::replaceSubString()
{
   QListViewItemIterator it(NameList);
   for(;it.current();++it)
     {
	QListViewItem * item = it.current();
	if (item->isSelected())
	  item->setText(0,replaceSubString(item->text(0)));
     }
}

void RenamerLogic::capitalizeAfterSpace()
{
   QListViewItemIterator it(NameList);
   for(;it.current();++it)
     {
	QListViewItem * item = it.current();
	if (item->isSelected())
	  item->setText(0,capitalizeAfterSpace(item->text(0)));
     }
}

void RenamerLogic::removeSpaces()
{
   QListViewItemIterator it(NameList);
   for(;it.current();++it)
     {
	QListViewItem * item = it.current();
	if (item->isSelected())
	  item->setText(0,removeSpaces(item->text(0)));
     }
   changeSelection();
}

void RenamerLogic::removeSpecials()
{
   QListViewItemIterator it(NameList);
   for(;it.current();++it)
     {
	QListViewItem * item = it.current();
	if (item->isSelected())
	  item->setText(0,removeSpecials(item->text(0)));
     }
   changeSelection();
}

void RenamerLogic::removeFirstChar()
{
   QListViewItemIterator it(NameList);
   for(;it.current();++it)
     {
	QListViewItem * item = it.current();
	if (item->isSelected())
	  item->setText(0,removeFirstChar(item->text(0)));
     }
}

void RenamerLogic::deleteSubString()
{
   QString key = SubString->text();
   QListViewItemIterator it(NameList);
   for(;it.current();++it)
     {
	QListViewItem * item = it.current();
	if (item->isSelected())
	  {
	     QString txt = item->text(0);
	     txt.replace(txt.find(key),key.length(),"");
	     item->setText(0,txt);
	  }
     }
}

QString RenamerLogic::removeFirstChar(QString in)
{
   return in.right(in.length()-1);
}

QString RenamerLogic::subStringIsAuthor(QString txt)
{
   // check extention
   QString key = "["+SubString->text()+"]"+txt.right(4);
   txt = txt.left(txt.length()-4);
   txt = txt.append(key);
   return txt;
}

void RenamerLogic::subStringIsAuthor()
{
   QListViewItemIterator it(NameList);
   for(;it.current();++it)
     {
	QListViewItem * item = it.current();
	if (item->isSelected())
	  {
	     item->setText(0,subStringIsAuthor(item->text(0)));
	  }
     }
}

void RenamerLogic::changeSelection()
{
   // find smallest, this will be our key to find 
   // the greatest common substring in
   QString key;
   int l = -1;
   QListViewItemIterator it(NameList);
   for(;it.current();++it)
     {
	QListViewItem * item = it.current();
	if (item->isSelected())
	  {
	     QString txt = item->text(0);
	     if (l==-1 || txt.length()<l)
	       {
		  l=txt.length();
		  key=txt;
	       }
	  }
     }
   // now lets look for a matching substring
   bool found = false;
   QString gcs;
   for(int size = key.length(); size>0 && !found; size--)
     {
	for(int pos = key.length()-size; pos>=0 && !found; pos--)
	  {
	     QListViewItemIterator it(NameList);
	     found = true;
	     gcs = key.mid(pos,size);
	     for(;it.current() && found;++it)
	       {
		  QListViewItem * item = it.current();
		  if (item->isSelected())
		    {
		       if (!item->text(0).contains(gcs))
			 found=false;
		    }
	       }
	  }
     }
   if (!found) 
     gcs="";
   SubString->setText(gcs);
}

void RenamerLogic::realizeSelection()
{
   QListViewItemIterator it(NameList);
   for(;it.current();)
     {
	QListViewItem * item = it.current();
	if (item->isSelected())
	  {
	     // position 0 is the new name
	     // position 1 is the old name
	     // position 2 is the full name, with location prefixed.
	     // find in str2 str1 and replace it by str0
	     char fullcommand[1024];
	     QString from = item->text(2);
	     QString toreplace = item->text(1);
	     QString replaceby = item->text(0);
	     QString to = from;
	     int pos = to.find(toreplace);
	     assert(pos>=0);
	     to.replace(pos,toreplace.length(),replaceby);
	     // if from = to dont do anything otherwise
	     if (from != to)
	       {
		  if (exists(to))
		    {
		       // fix target filename...
		       int trie = 1;
		       QString nto;
		       do
			 {
			    trie++;
			    nto=to;
			    char nr[50];
			    sprintf(nr,"%d",trie);
			    nto.insert(nto.length()-4,nr);
			 }
		       while (exists(nto));
		       to=nto;
		    }
		  sprintf(fullcommand,"mv -i \"%s\" \"%s\"",((const char*)from),((const char*)to));
		  printf("Executing %s\n",fullcommand);
		  system(fullcommand);
	       }
	     // benieuwd of het dees gaat werken...
	     delete item;
	  }
	else
	  ++it;
     }
}

void RenamerLogic::ignoreSelection()
{
   QListViewItemIterator it(NameList);
   for(;it.current();)
     {
	QListViewItem * item = it.current();
	if (item->isSelected())
	  delete item;
	else ++it;
     }
}
