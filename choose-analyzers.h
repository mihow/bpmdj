/****************************************************************************
** Form interface generated from reading ui file 'choose-analyzers.ui'
**
** Created: Sat May 8 12:31:21 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.3   edited May 19 14:22 $)
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
class QPushButton;
class QTabWidget;
class QWidget;
class QLabel;
class QCheckBox;
class QFrame;
class QButtonGroup;
class QRadioButton;
class QLineEdit;

class ChooseAnalyzers : public QDialog
{
    Q_OBJECT

public:
    ChooseAnalyzers( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ChooseAnalyzers();

    QPushButton* pushButton3;
    QPushButton* pushButton4;
    QTabWidget* tabWidget2;
    QWidget* tab;
    QLabel* textLabel1;
    QCheckBox* tempoAnalyzer;
    QCheckBox* spectrumAnalyzer;
    QFrame* line2;
    QWidget* tab_2;
    QLabel* TextLabel4;
    QFrame* Line7;
    QButtonGroup* enveloppeFft;
    QRadioButton* resamplingScan;
    QRadioButton* ultraLongFFT;
    QRadioButton* enveloppeSpectrum;
    QRadioButton* fullAutoCorrelation;
    QLabel* TextLabel1;
    QLabel* TextLabel2;
    QLineEdit* From;
    QLineEdit* To;

protected:
    QGridLayout* ChooseAnalyzersLayout;
    QGridLayout* tabLayout;
    QGridLayout* tabLayout_2;
    QGridLayout* enveloppeFftLayout;
    QHBoxLayout* layout13;
    QVBoxLayout* layout11;
    QVBoxLayout* layout12;

protected slots:
    virtual void languageChange();

};

#endif // CHOOSEANALYZERS_H
