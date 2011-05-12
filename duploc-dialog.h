/****************************************************************************
** Form interface generated from reading ui file 'duploc-dialog.ui'
**
** Created: Fri Apr 9 17:54:30 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.3   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef DUPLOCDIALOG_H
#define DUPLOCDIALOG_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QTextBrowser;

class DuplocDialog : public QDialog
{
    Q_OBJECT

public:
    DuplocDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DuplocDialog();

    QLabel* matrixview;
    QTextBrowser* lefttext;
    QTextBrowser* righttext;

protected:
    QGridLayout* DuplocDialogLayout;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;

};

#endif // DUPLOCDIALOG_H
