/****************************************************************************
** Form interface generated from reading ui file 'playercommandwizard.ui'
**
** Created: Sat May 8 12:31:21 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.3   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef PLAYERCOMMANDWIZARD_H
#define PLAYERCOMMANDWIZARD_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QButtonGroup;
class QCheckBox;
class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;

class PlayerCommandWizard : public QDialog
{
    Q_OBJECT

public:
    PlayerCommandWizard( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~PlayerCommandWizard();

    QButtonGroup* buttonGroup1;
    QCheckBox* oss;
    QCheckBox* alsa;
    QCheckBox* xmms;
    QCheckBox* none;
    QButtonGroup* buttonGroup2;
    QLabel* textLabel4;
    QLabel* textLabel3;
    QLineEdit* latency;
    QComboBox* comboBox5;
    QComboBox* comboBox7;
    QLineEdit* x;
    QLabel* textLabel5;
    QLineEdit* y;
    QCheckBox* verbose;
    QButtonGroup* buttonGroup3;
    QLabel* textLabel6;
    QLineEdit* dsp;
    QLineEdit* fragments;
    QLabel* textLabel7;
    QCheckBox* nolatency;
    QComboBox* comboBox3;
    QComboBox* comboBox4;
    QButtonGroup* buttonGroup4;
    QLabel* textLabel9;
    QLineEdit* dev;
    QComboBox* comboBox2;
    QPushButton* pushButton8;
    QPushButton* pushButton7;

protected:
    QGridLayout* PlayerCommandWizardLayout;
    QGridLayout* buttonGroup1Layout;
    QGridLayout* buttonGroup2Layout;
    QHBoxLayout* layout1;
    QGridLayout* buttonGroup3Layout;
    QGridLayout* buttonGroup4Layout;

protected slots:
    virtual void languageChange();

};

#endif // PLAYERCOMMANDWIZARD_H
