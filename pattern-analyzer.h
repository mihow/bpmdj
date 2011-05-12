/****************************************************************************
** Form interface generated from reading ui file 'pattern-analyzer.ui'
**
** Created: Sun Sep 14 13:38:46 2003
**      by: The User Interface Compiler ($Id: pattern-analyzer.h,v 1.1 2003/10/10 08:50:55 krubbens Exp $)
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
    QPushButton* updateButton;
    QSpinBox* beats;
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
    virtual void dumpPatternToIdx();

protected:
    QGridLayout* PatternDialogLayout;

protected slots:
    virtual void languageChange();

};

#endif // PATTERNDIALOG_H
