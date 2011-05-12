/****************************************************************************
** Form interface generated from reading ui file 'patternanalyzer.ui'
**
** Created: Sun Jul 13 15:59:12 2003
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.2   edited Dec 19 11:45 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef PATTERNDIALOG_H
#define PATTERNDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSlider;
class QLabel;
class QPushButton;
class QSpinBox;

class PatternDialog : public QDialog
{
    Q_OBJECT

public:
    PatternDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~PatternDialog();

    QSlider* periodDelta;
    QSlider* periodDelta10;
    QLabel* textLabel1;
    QLabel* pattern;
    QSlider* color;
    QLabel* textLabel1_2;
    QPushButton* pushButton4;
    QPushButton* pushButton2;
    QLabel* projection;
    QPushButton* pushButton51;
    QSpinBox* measures;
    QLabel* textLabel1_3;

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
    QGridLayout* PatternDialogLayout;

protected slots:
    virtual void languageChange();

};

#endif // PATTERNDIALOG_H
