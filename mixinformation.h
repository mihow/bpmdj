/****************************************************************************
** Form interface generated from reading ui file 'mixinformation.ui'
**
** Created: Tue Oct 21 14:25:37 2003
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MIXINFORMATION_H
#define MIXINFORMATION_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QFrame;
class QPushButton;

class MixInformation : public QDialog
{
    Q_OBJECT

public:
    MixInformation( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~MixInformation();

    QLabel* textLabel5;
    QLineEdit* commentEdit;
    QFrame* line1;
    QLabel* textLabel1;
    QLabel* textLabel2;
    QLabel* toLabel;
    QLabel* fromLabel;
    QPushButton* pushButton2;
    QPushButton* pushButton1;

protected:
    QGridLayout* MixInformationLayout;

protected slots:
    virtual void languageChange();

};

#endif // MIXINFORMATION_H
