/****************************************************************************
** Form interface generated from reading ui file 'songproperties.ui'
**
** Created: Mon Oct 20 11:54:14 2003
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef ANALYZEDIALOG_H
#define ANALYZEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QLabel;
class QLineEdit;
class QFrame;

class AnalyzeDialog : public QDialog
{
    Q_OBJECT

public:
    AnalyzeDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~AnalyzeDialog();

    QPushButton* BusyToggle;
    QLabel* TextLabel1;
    QLabel* TextLabel5_2;
    QLabel* SongLabel;
    QLabel* Md5Label;
    QLabel* TextLabel2_2;
    QLabel* TextLabel3;
    QLineEdit* TagEdit;
    QLineEdit* IdxEdit;
    QPushButton* pushButton13;
    QFrame* line1;
    QPushButton* pushButton7;
    QPushButton* pushButton8;
    QPushButton* pushButton11;
    QPushButton* pushButton9;
    QPushButton* pushButton10;

protected:
    QGridLayout* AnalyzeDialogLayout;
    QHBoxLayout* Layout21;
    QVBoxLayout* Layout20;
    QVBoxLayout* Layout18;
    QVBoxLayout* Layout20_2;
    QVBoxLayout* layout20;
    QHBoxLayout* layout19;

protected slots:
    virtual void languageChange();

};

#endif // ANALYZEDIALOG_H
