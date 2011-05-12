/****************************************************************************
** Form interface generated from reading ui file 'choose-analyzers.ui'
**
** Created: Fri Aug 15 14:21:37 2003
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.2   edited Dec 19 11:45 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef CHOOSEANALYZERS_H
#define CHOOSEANALYZERS_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QTabWidget;
class QWidget;
class QLabel;
class QCheckBox;
class QFrame;
class QLineEdit;
class QPushButton;

class ChooseAnalyzers : public QDialog
{
    Q_OBJECT

public:
    ChooseAnalyzers( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ChooseAnalyzers();

    QTabWidget* tabWidget2;
    QWidget* tab;
    QLabel* textLabel1;
    QCheckBox* tempoAnalyzer;
    QCheckBox* spectrumAnalyzer;
    QCheckBox* patternAnalyzer;
    QWidget* tab_2;
    QLabel* TextLabel1;
    QLabel* TextLabel2;
    QFrame* Line7;
    QLineEdit* From;
    QLineEdit* To;
    QLabel* TextLabel4;
    QPushButton* pushButton3;
    QPushButton* pushButton4;

protected:
    QGridLayout* ChooseAnalyzersLayout;
    QGridLayout* tabLayout;
    QGridLayout* tabLayout_2;

protected slots:
    virtual void languageChange();

};

#endif // CHOOSEANALYZERS_H
