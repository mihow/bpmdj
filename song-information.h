/****************************************************************************
** Form interface generated from reading ui file 'song-information.ui'
**
** Created: Tue Oct 21 22:48:50 2003
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SONGINFORMATION_H
#define SONGINFORMATION_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QFrame;
class QPushButton;

class SongInformation : public QDialog
{
    Q_OBJECT

public:
    SongInformation( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SongInformation();

    QLabel* TextLabel1;
    QLabel* TextLabel5_2;
    QLabel* idxLabel;
    QLabel* md5Label;
    QLabel* textLabel1;
    QLabel* textLabel2;
    QLabel* textLabel3;
    QLabel* textLabel4;
    QLabel* TextLabel2_2;
    QLineEdit* titleEdit;
    QLineEdit* authorEdit;
    QLineEdit* remixEdit;
    QLineEdit* versionEdit;
    QLineEdit* tagEdit;
    QFrame* line1;
    QPushButton* pushButton10;
    QPushButton* pushButton13;

protected:
    QGridLayout* SongInformationLayout;
    QHBoxLayout* Layout21;
    QVBoxLayout* Layout20;
    QVBoxLayout* Layout18;
    QHBoxLayout* layout10;
    QVBoxLayout* layout8;
    QVBoxLayout* layout9;

protected slots:
    virtual void languageChange();

};

#endif // SONGINFORMATION_H
