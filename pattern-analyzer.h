/****************************************************************************
** Form interface generated from reading ui file 'pattern-analyzer.ui'
**
** Created: Mon Nov 17 09:50:05 2003
**      by: The User Interface Compiler ($Id: pattern-analyzer.h,v 1.2 2003/11/23 11:17:02 krubbens Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef PATTERNDIALOG_H
#define PATTERNDIALOG_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QTabWidget;
class QWidget;
class QSlider;
class QLabel;
class QPushButton;
class QSpinBox;
class QComboBox;
class QLineEdit;

class PatternDialog : public QDialog
{
    Q_OBJECT

public:
    PatternDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~PatternDialog();

    QTabWidget* tabWidget2;
    QWidget* tab;
    QSlider* periodDelta;
    QSlider* periodDelta10;
    QLabel* textLabel1;
    QSlider* color;
    QLabel* textLabel1_2;
    QPushButton* pushButton4;
    QPushButton* pushButton2;
    QPushButton* updateButton;
    QSpinBox* beats;
    QLabel* textLabel1_3;
    QLabel* pattern;
    QLabel* projection;
    QWidget* tab_2;
    QLabel* filterImage;
    QLabel* textLabel1_4;
    QLabel* textLabel2;
    QLabel* textLabel4;
    QLabel* textLabel3;
    QComboBox* hicut;
    QComboBox* locut;
    QLineEdit* order;
    QSpinBox* stacking;

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
    virtual void filterChanged();

protected:
    QGridLayout* PatternDialogLayout;
    QGridLayout* tabLayout;
    QGridLayout* tabLayout_2;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;

};

#endif // PATTERNDIALOG_H
