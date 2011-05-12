/****************************************************************************
** Form interface generated from reading ui file 'impulseanalyzer.ui'
**
** Created: Fri Jul 18 01:20:08 2003
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.2   edited Dec 19 11:45 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef IMPULSEDIALOG_H
#define IMPULSEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QSlider;
class QPushButton;

class ImpulseDialog : public QDialog
{
    Q_OBJECT

public:
    ImpulseDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ImpulseDialog();

    QLabel* impulse;
    QSlider* color;
    QLabel* textLabel1_2;
    QPushButton* pushButton2;
    QPushButton* pushButton51;

public slots:
    virtual void slantChanged();
    virtual void showPattern();
    virtual void cancel();
    virtual void finish();
    virtual void startAutomaticCounter();
    virtual void tap();
    virtual void fixPhases();
    virtual void setTempo();
    virtual void balanceChanged();

protected:
    QGridLayout* ImpulseDialogLayout;

protected slots:
    virtual void languageChange();

};

#endif // IMPULSEDIALOG_H
