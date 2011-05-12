/****************************************************************************
** Form interface generated from reading ui file 'spectrumanalyzer.ui'
**
** Created: Wed Jul 9 19:18:38 2003
**      by: The User Interface Compiler ($Id: spectrumanalyzer.h,v 1.1 2003/10/10 08:50:55 krubbens Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef SPECTRUMDIALOG_H
#define SPECTRUMDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QGroupBox;
class QSlider;
class QProgressBar;

class SpectrumDialog : public QDialog
{
    Q_OBJECT

public:
    SpectrumDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~SpectrumDialog();

    QPushButton* pushButton8;
    QGroupBox* groupBox1;
    QSlider* bark12;
    QSlider* bark11;
    QSlider* bark10;
    QSlider* bark9;
    QSlider* bark8;
    QSlider* bark7;
    QSlider* bark6;
    QSlider* bark5;
    QSlider* bark4;
    QSlider* bark3;
    QSlider* bark0;
    QSlider* bark1;
    QSlider* bark2;
    QSlider* bark17;
    QSlider* bark20;
    QSlider* bark22;
    QSlider* bark18;
    QSlider* bark23;
    QSlider* bark13;
    QSlider* bark19;
    QSlider* bark14;
    QSlider* bark21;
    QSlider* bark16;
    QSlider* bark15;
    QProgressBar* Progress;

public slots:
    virtual void about();
    virtual void audioRangeChanged();
    virtual void bpmRangeChanged();
    virtual void cancel();
    virtual void incBpm();
    virtual void decBpm();
    virtual void play();
    virtual void reset();
    virtual void startAutomaticCounter();
    virtual void tap();
    virtual void fetchSpectrum();

protected:
    QGridLayout* SpectrumDialogLayout;
    QGridLayout* groupBox1Layout;

protected slots:
    virtual void languageChange();

};

#endif // SPECTRUMDIALOG_H
