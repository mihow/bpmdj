/****************************************************************************
** Form interface generated from reading ui file 'tempolineanalyzer.ui'
**
** Created: Thu Jul 24 05:00:31 2003
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.2   edited Dec 19 11:45 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef TEMPOLINEDIALOG_H
#define TEMPOLINEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QSlider;
class QPushButton;

class TempoLineDialog : public QDialog
{
    Q_OBJECT

public:
    TempoLineDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~TempoLineDialog();

    QLabel* tempoline;
    QSlider* color;
    QLabel* textLabel1_2;
    QPushButton* pushButton2;
    QPushButton* pushButton51;

public slots:
    virtual void cancel();
    virtual void finish();
    virtual void startAutomaticCounter();
    virtual void balanceChanged();
    virtual void showTempoLine();

protected:
    QGridLayout* TempoLineDialogLayout;

protected slots:
    virtual void languageChange();

};

#endif // TEMPOLINEDIALOG_H
