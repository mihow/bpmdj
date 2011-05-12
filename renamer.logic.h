#include <qlistview.h>
#include <qwidget.h>
#include "kbpm-index.h"
#include "renamer.h"

class RenamerLogic: 
  public Renamer, 
  public DirectoryScanner
{
   Q_OBJECT
 public:
   RenamerLogic(QWidget* parent=0, const QString name=0);
   void add(const QString name, const QString pos);
 protected:
   virtual void checkfile(const QString fullname, const QString filename);
   virtual bool matchextension(const QString filename);
 private:
   bool    goodName(QString name);
   QString smallCapsInWord(QString in);
   QString capitalizeAfterSpace(QString in);
   QString removeSpaces(QString in);
   QString removeSpecials(QString in);
   QString removeFirstChar(QString in);
   QString subStringIsAuthor(QString in);
   QString replaceSubString(QString in);
 public slots:
   virtual void capitalizeAfterSpace();
   virtual void smallCapsInWords();
   virtual void removeSpaces();
   virtual void removeSpecials();
   virtual void changeSelection();
   virtual void deleteSubString();
   virtual void subStringIsAuthor();
   virtual void removeFirstChar();
   virtual void replaceSubString();
   virtual void realizeSelection();
   virtual void ignoreSelection();
};
