/********************************************************************************
** Form generated from reading UI file 'ui-player.ui'
**
** Created: Thu May 12 17:50:02 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_PLAYER_H
#define UI_2D_PLAYER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "beatgraph-analyzer.h"
#include "bpm-analyzer.h"
#include "capacity-widget.h"
#include "rhythm-analyzer.h"
#include "song-information.h"
#include "spectrum-analyzer.h"

QT_BEGIN_NAMESPACE

class Ui_SongPlayer
{
public:
    QGridLayout *gridLayout;
    QTabWidget *tab;
    QWidget *playerTab;
    QGridLayout *gridLayout_6;
    QVBoxLayout *verticalLayout_10;
    QHBoxLayout *horizontalLayout_8;
    QVBoxLayout *verticalLayout_11;
    QLabel *label;
    QLCDNumber *CurrentTimeLCD;
    QVBoxLayout *verticalLayout_12;
    QLabel *label_2;
    QLCDNumber *TotalTimeLCD;
    QVBoxLayout *verticalLayout_13;
    QLabel *label_3;
    QLCDNumber *CurrentTempoLCD;
    QVBoxLayout *verticalLayout_14;
    QLabel *label_4;
    QLCDNumber *NormalTempoLCD;
    QVBoxLayout *verticalLayout_15;
    QLabel *label_5;
    QLCDNumber *TempoLd;
    QSlider *tempoSlider;
    QHBoxLayout *horizontalLayout_7;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_1;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_5;
    QPushButton *KeyMinus;
    QPushButton *ButtonMinusHalfB;
    QPushButton *ButtonMinus1M;
    QPushButton *ButtonMinus4M;
    QPushButton *ButtonMinus8M;
    QVBoxLayout *verticalLayout_6;
    QPushButton *KeyPlus;
    QPushButton *PushButton19;
    QPushButton *ButtonPlus1M;
    QPushButton *ButtonPlus4M;
    QPushButton *ButtonPlus8M;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QPushButton *PushButton11;
    QPushButton *PushButton12;
    QPushButton *PushButton13;
    QPushButton *PushButton14;
    QVBoxLayout *verticalLayout_2;
    QPushButton *PushButton15;
    QPushButton *PushButton16;
    QPushButton *PushButton17;
    QPushButton *PushButton18;
    QVBoxLayout *verticalLayout_3;
    QPushButton *PushButton34;
    QPushButton *PushButton33;
    QPushButton *PushButton35;
    QPushButton *PushButton36;
    QPushButton *PushButton10;
    QGroupBox *groupBox_7;
    QGridLayout *gridLayout_5;
    QVBoxLayout *verticalLayout_9;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *verticalLayout_7;
    QPushButton *PushButton26;
    QPushButton *PushButton28;
    QPushButton *PushButton30;
    QPushButton *PushButton42;
    QVBoxLayout *verticalLayout_8;
    QPushButton *PushButton27;
    QPushButton *PushButton29;
    QPushButton *PushButton31;
    QPushButton *PushButton39;
    QPushButton *PushButton41;
    QHBoxLayout *horizontalLayout_6;
    QSpinBox *tempoSwitchTime;
    QPushButton *PushButton21;
    QPushButton *switcherButton;
    QPushButton *PushButton20;
    QPushButton *PushButton24;
    QPushButton *startStopButton;
    QPushButton *AboutButton;
    QWidget *scramblerTab;
    QGridLayout *gridLayout_3;
    QVBoxLayout *verticalLayout_20;
    QVBoxLayout *vboxLayout;
    QLabel *mapin;
    QLabel *mapout;
    QLabel *mapspeed;
    QLabel *mapvolume;
    QHBoxLayout *horizontalLayout_10;
    QPushButton *pushButton49;
    QPushButton *pushButton50_2;
    QPushButton *pushButton51;
    QHBoxLayout *horizontalLayout_11;
    QVBoxLayout *verticalLayout_17;
    QPushButton *pushButton50;
    QPushButton *pushButton77;
    QVBoxLayout *verticalLayout_18;
    QSpinBox *mapout_length;
    QComboBox *mapping_scale;
    QHBoxLayout *horizontalLayout_4;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_20;
    QHBoxLayout *horizontalLayout_9;
    QRadioButton *atend_restart;
    QRadioButton *atend_continue;
    QRadioButton *atend_stop;
    QHBoxLayout *horizontalLayout_12;
    QCheckBox *atend_loop;
    QRadioButton *atend_c;
    QRadioButton *atend_x;
    QRadioButton *atend_v;
    QRadioButton *atend_z;
    QVBoxLayout *verticalLayout_19;
    QPushButton *pushButton48;
    QPushButton *pushButton47;
    QWidget *beatgraphTab;
    QGridLayout *gridLayout1;
    BeatGraphAnalyzer *beatGraphAnalyzer;
    QWidget *bpmTab;
    QGridLayout *gridLayout2;
    BpmAnalyzerDialog *bpmCounter;
    QWidget *spectrumTab;
    QGridLayout *gridLayout3;
    SpectrumAnalyzer *spectrum;
    QWidget *rhythmTab;
    QGridLayout *gridLayout4;
    RhythmAnalyzer *widget;
    QWidget *songTab;
    QGridLayout *gridLayout5;
    SongInformation *songInfo;
    QWidget *setupTab;
    QGridLayout *gridLayout6;
    QSpacerItem *spacerItem;
    QPushButton *pushButton90;
    QTabWidget *tabWidget;
    QWidget *decodingtab;
    QGridLayout *gridLayout7;
    QCheckBox *rms_toggle;
    QLabel *textLabel2_6;
    QLineEdit *rms;
    QLabel *textLabel2;
    QLabel *textLabel1_3;
    CapacityWidget *capacity;
    QLineEdit *raw_directory;
    QWidget *alsatab;
    QGridLayout *gridLayout8;
    QLabel *textLabel9;
    QSpinBox *alsa_latency;
    QSpacerItem *spacerItem1;
    QCheckBox *alsa_verbose;
    QLabel *textLabel3;
    QCheckBox *alsa;
    QLineEdit *alsa_dev;
    QComboBox *comboBox2;
    QWidget *osstab;
    QGridLayout *gridLayout9;
    QSpacerItem *spacerItem2;
    QLabel *textLabel7;
    QCheckBox *oss_verbose;
    QLabel *textLabel6;
    QCheckBox *oss_init_fragments;
    QCheckBox *oss;
    QLabel *textLabel3_3;
    QSpinBox *oss_latency;
    QLineEdit *oss_dsp;
    QSpinBox *oss_fragments;
    QCheckBox *oss_nolatencyaccounting;
    QComboBox *comboBox3;
    QWidget *jacktab;
    QGridLayout *subgridLayout;
    QCheckBox *jack;
    QLabel *outputchannel_label;
    QHBoxLayout *horizontalLayout1;
    QLineEdit *jack_dev;
    QComboBox *jackcombo;
    QLabel *connectchannell_label;
    QHBoxLayout *horizontalLayout;
    QLineEdit *jack_lout;
    QComboBox *jackloutcombo;
    QLabel *connectchannelr_label;
    QHBoxLayout *horizontalLayoutLR;
    QLineEdit *jack_rout;
    QComboBox *jackroutcombo;
    QCheckBox *jack_verbose;
    QSpacerItem *verticalSpacer;

    void setupUi(QDialog *SongPlayer)
    {
        if (SongPlayer->objectName().isEmpty())
            SongPlayer->setObjectName(QString::fromUtf8("SongPlayer"));
        SongPlayer->resize(648, 408);
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SongPlayer->sizePolicy().hasHeightForWidth());
        SongPlayer->setSizePolicy(sizePolicy);
        SongPlayer->setSizeGripEnabled(true);
        gridLayout = new QGridLayout(SongPlayer);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tab = new QTabWidget(SongPlayer);
        tab->setObjectName(QString::fromUtf8("tab"));
        playerTab = new QWidget();
        playerTab->setObjectName(QString::fromUtf8("playerTab"));
        gridLayout_6 = new QGridLayout(playerTab);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        verticalLayout_10 = new QVBoxLayout();
        verticalLayout_10->setSpacing(2);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        verticalLayout_11 = new QVBoxLayout();
        verticalLayout_11->setSpacing(0);
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        label = new QLabel(playerTab);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font;
        font.setPointSize(8);
        label->setFont(font);

        verticalLayout_11->addWidget(label);

        CurrentTimeLCD = new QLCDNumber(playerTab);
        CurrentTimeLCD->setObjectName(QString::fromUtf8("CurrentTimeLCD"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(CurrentTimeLCD->sizePolicy().hasHeightForWidth());
        CurrentTimeLCD->setSizePolicy(sizePolicy1);
        CurrentTimeLCD->setMinimumSize(QSize(0, 40));
        QPalette palette;
        QBrush brush(QColor(255, 255, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(255, 255, 255, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush1);
        QBrush brush2(QColor(0, 0, 0, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Window, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        QBrush brush3(QColor(121, 125, 121, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        CurrentTimeLCD->setPalette(palette);
        CurrentTimeLCD->setAutoFillBackground(true);
        CurrentTimeLCD->setNumDigits(6);
        CurrentTimeLCD->setSegmentStyle(QLCDNumber::Flat);

        verticalLayout_11->addWidget(CurrentTimeLCD);


        horizontalLayout_8->addLayout(verticalLayout_11);

        verticalLayout_12 = new QVBoxLayout();
        verticalLayout_12->setSpacing(0);
        verticalLayout_12->setObjectName(QString::fromUtf8("verticalLayout_12"));
        label_2 = new QLabel(playerTab);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setFont(font);

        verticalLayout_12->addWidget(label_2);

        TotalTimeLCD = new QLCDNumber(playerTab);
        TotalTimeLCD->setObjectName(QString::fromUtf8("TotalTimeLCD"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(TotalTimeLCD->sizePolicy().hasHeightForWidth());
        TotalTimeLCD->setSizePolicy(sizePolicy2);
        TotalTimeLCD->setMinimumSize(QSize(0, 0));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush3);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        TotalTimeLCD->setPalette(palette1);
        TotalTimeLCD->setAutoFillBackground(true);
        TotalTimeLCD->setNumDigits(6);
        TotalTimeLCD->setSegmentStyle(QLCDNumber::Flat);

        verticalLayout_12->addWidget(TotalTimeLCD);


        horizontalLayout_8->addLayout(verticalLayout_12);

        verticalLayout_13 = new QVBoxLayout();
        verticalLayout_13->setSpacing(0);
        verticalLayout_13->setObjectName(QString::fromUtf8("verticalLayout_13"));
        label_3 = new QLabel(playerTab);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setFont(font);

        verticalLayout_13->addWidget(label_3);

        CurrentTempoLCD = new QLCDNumber(playerTab);
        CurrentTempoLCD->setObjectName(QString::fromUtf8("CurrentTempoLCD"));
        sizePolicy1.setHeightForWidth(CurrentTempoLCD->sizePolicy().hasHeightForWidth());
        CurrentTempoLCD->setSizePolicy(sizePolicy1);
        CurrentTempoLCD->setMinimumSize(QSize(0, 30));
        QPalette palette2;
        palette2.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette2.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette2.setBrush(QPalette::Active, QPalette::Window, brush2);
        palette2.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        palette2.setBrush(QPalette::Disabled, QPalette::WindowText, brush3);
        palette2.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette2.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        CurrentTempoLCD->setPalette(palette2);
        CurrentTempoLCD->setAutoFillBackground(true);
        CurrentTempoLCD->setNumDigits(6);
        CurrentTempoLCD->setSegmentStyle(QLCDNumber::Flat);

        verticalLayout_13->addWidget(CurrentTempoLCD);


        horizontalLayout_8->addLayout(verticalLayout_13);

        verticalLayout_14 = new QVBoxLayout();
        verticalLayout_14->setSpacing(0);
        verticalLayout_14->setObjectName(QString::fromUtf8("verticalLayout_14"));
        label_4 = new QLabel(playerTab);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFont(font);

        verticalLayout_14->addWidget(label_4);

        NormalTempoLCD = new QLCDNumber(playerTab);
        NormalTempoLCD->setObjectName(QString::fromUtf8("NormalTempoLCD"));
        sizePolicy1.setHeightForWidth(NormalTempoLCD->sizePolicy().hasHeightForWidth());
        NormalTempoLCD->setSizePolicy(sizePolicy1);
        QPalette palette3;
        palette3.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette3.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette3.setBrush(QPalette::Active, QPalette::Window, brush2);
        palette3.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette3.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        palette3.setBrush(QPalette::Disabled, QPalette::WindowText, brush3);
        palette3.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette3.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        NormalTempoLCD->setPalette(palette3);
        NormalTempoLCD->setAutoFillBackground(true);
        NormalTempoLCD->setNumDigits(6);
        NormalTempoLCD->setSegmentStyle(QLCDNumber::Flat);

        verticalLayout_14->addWidget(NormalTempoLCD);


        horizontalLayout_8->addLayout(verticalLayout_14);

        verticalLayout_15 = new QVBoxLayout();
        verticalLayout_15->setSpacing(0);
        verticalLayout_15->setObjectName(QString::fromUtf8("verticalLayout_15"));
        label_5 = new QLabel(playerTab);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setFont(font);

        verticalLayout_15->addWidget(label_5);

        TempoLd = new QLCDNumber(playerTab);
        TempoLd->setObjectName(QString::fromUtf8("TempoLd"));
        sizePolicy1.setHeightForWidth(TempoLd->sizePolicy().hasHeightForWidth());
        TempoLd->setSizePolicy(sizePolicy1);
        QPalette palette4;
        palette4.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette4.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette4.setBrush(QPalette::Active, QPalette::Window, brush2);
        palette4.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette4.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette4.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        palette4.setBrush(QPalette::Disabled, QPalette::WindowText, brush3);
        palette4.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette4.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        TempoLd->setPalette(palette4);
        TempoLd->setAutoFillBackground(true);
        TempoLd->setSegmentStyle(QLCDNumber::Flat);

        verticalLayout_15->addWidget(TempoLd);


        horizontalLayout_8->addLayout(verticalLayout_15);


        verticalLayout_10->addLayout(horizontalLayout_8);

        tempoSlider = new QSlider(playerTab);
        tempoSlider->setObjectName(QString::fromUtf8("tempoSlider"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(tempoSlider->sizePolicy().hasHeightForWidth());
        tempoSlider->setSizePolicy(sizePolicy3);
        tempoSlider->setMinimumSize(QSize(0, 30));
        tempoSlider->setMinimum(-100);
        tempoSlider->setMaximum(100);
        tempoSlider->setValue(0);
        tempoSlider->setOrientation(Qt::Horizontal);

        verticalLayout_10->addWidget(tempoSlider);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(0);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        groupBox_5 = new QGroupBox(playerTab);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        groupBox_5->setFont(font);
        gridLayout_1 = new QGridLayout(groupBox_5);
        gridLayout_1->setSpacing(6);
        gridLayout_1->setContentsMargins(2, 2, 2, 2);
        gridLayout_1->setObjectName(QString::fromUtf8("gridLayout_1"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        KeyMinus = new QPushButton(groupBox_5);
        KeyMinus->setObjectName(QString::fromUtf8("KeyMinus"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(KeyMinus->sizePolicy().hasHeightForWidth());
        KeyMinus->setSizePolicy(sizePolicy4);

        verticalLayout_5->addWidget(KeyMinus);

        ButtonMinusHalfB = new QPushButton(groupBox_5);
        ButtonMinusHalfB->setObjectName(QString::fromUtf8("ButtonMinusHalfB"));
        sizePolicy4.setHeightForWidth(ButtonMinusHalfB->sizePolicy().hasHeightForWidth());
        ButtonMinusHalfB->setSizePolicy(sizePolicy4);

        verticalLayout_5->addWidget(ButtonMinusHalfB);

        ButtonMinus1M = new QPushButton(groupBox_5);
        ButtonMinus1M->setObjectName(QString::fromUtf8("ButtonMinus1M"));
        sizePolicy4.setHeightForWidth(ButtonMinus1M->sizePolicy().hasHeightForWidth());
        ButtonMinus1M->setSizePolicy(sizePolicy4);

        verticalLayout_5->addWidget(ButtonMinus1M);

        ButtonMinus4M = new QPushButton(groupBox_5);
        ButtonMinus4M->setObjectName(QString::fromUtf8("ButtonMinus4M"));
        sizePolicy4.setHeightForWidth(ButtonMinus4M->sizePolicy().hasHeightForWidth());
        ButtonMinus4M->setSizePolicy(sizePolicy4);

        verticalLayout_5->addWidget(ButtonMinus4M);

        ButtonMinus8M = new QPushButton(groupBox_5);
        ButtonMinus8M->setObjectName(QString::fromUtf8("ButtonMinus8M"));
        sizePolicy4.setHeightForWidth(ButtonMinus8M->sizePolicy().hasHeightForWidth());
        ButtonMinus8M->setSizePolicy(sizePolicy4);

        verticalLayout_5->addWidget(ButtonMinus8M);


        horizontalLayout_3->addLayout(verticalLayout_5);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        KeyPlus = new QPushButton(groupBox_5);
        KeyPlus->setObjectName(QString::fromUtf8("KeyPlus"));
        sizePolicy4.setHeightForWidth(KeyPlus->sizePolicy().hasHeightForWidth());
        KeyPlus->setSizePolicy(sizePolicy4);

        verticalLayout_6->addWidget(KeyPlus);

        PushButton19 = new QPushButton(groupBox_5);
        PushButton19->setObjectName(QString::fromUtf8("PushButton19"));
        sizePolicy4.setHeightForWidth(PushButton19->sizePolicy().hasHeightForWidth());
        PushButton19->setSizePolicy(sizePolicy4);

        verticalLayout_6->addWidget(PushButton19);

        ButtonPlus1M = new QPushButton(groupBox_5);
        ButtonPlus1M->setObjectName(QString::fromUtf8("ButtonPlus1M"));
        sizePolicy4.setHeightForWidth(ButtonPlus1M->sizePolicy().hasHeightForWidth());
        ButtonPlus1M->setSizePolicy(sizePolicy4);

        verticalLayout_6->addWidget(ButtonPlus1M);

        ButtonPlus4M = new QPushButton(groupBox_5);
        ButtonPlus4M->setObjectName(QString::fromUtf8("ButtonPlus4M"));
        sizePolicy4.setHeightForWidth(ButtonPlus4M->sizePolicy().hasHeightForWidth());
        ButtonPlus4M->setSizePolicy(sizePolicy4);

        verticalLayout_6->addWidget(ButtonPlus4M);

        ButtonPlus8M = new QPushButton(groupBox_5);
        ButtonPlus8M->setObjectName(QString::fromUtf8("ButtonPlus8M"));
        sizePolicy4.setHeightForWidth(ButtonPlus8M->sizePolicy().hasHeightForWidth());
        ButtonPlus8M->setSizePolicy(sizePolicy4);

        verticalLayout_6->addWidget(ButtonPlus8M);


        horizontalLayout_3->addLayout(verticalLayout_6);


        gridLayout_1->addLayout(horizontalLayout_3, 0, 0, 1, 1);


        horizontalLayout_7->addWidget(groupBox_5);

        groupBox_6 = new QGroupBox(playerTab);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        groupBox_6->setFont(font);
        gridLayout_2 = new QGridLayout(groupBox_6);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(2, 2, 2, 2);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        PushButton11 = new QPushButton(groupBox_6);
        PushButton11->setObjectName(QString::fromUtf8("PushButton11"));
        sizePolicy4.setHeightForWidth(PushButton11->sizePolicy().hasHeightForWidth());
        PushButton11->setSizePolicy(sizePolicy4);
        PushButton11->setFlat(false);

        verticalLayout->addWidget(PushButton11);

        PushButton12 = new QPushButton(groupBox_6);
        PushButton12->setObjectName(QString::fromUtf8("PushButton12"));
        sizePolicy4.setHeightForWidth(PushButton12->sizePolicy().hasHeightForWidth());
        PushButton12->setSizePolicy(sizePolicy4);

        verticalLayout->addWidget(PushButton12);

        PushButton13 = new QPushButton(groupBox_6);
        PushButton13->setObjectName(QString::fromUtf8("PushButton13"));
        sizePolicy4.setHeightForWidth(PushButton13->sizePolicy().hasHeightForWidth());
        PushButton13->setSizePolicy(sizePolicy4);

        verticalLayout->addWidget(PushButton13);

        PushButton14 = new QPushButton(groupBox_6);
        PushButton14->setObjectName(QString::fromUtf8("PushButton14"));
        sizePolicy4.setHeightForWidth(PushButton14->sizePolicy().hasHeightForWidth());
        PushButton14->setSizePolicy(sizePolicy4);

        verticalLayout->addWidget(PushButton14);


        horizontalLayout_2->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        PushButton15 = new QPushButton(groupBox_6);
        PushButton15->setObjectName(QString::fromUtf8("PushButton15"));
        sizePolicy4.setHeightForWidth(PushButton15->sizePolicy().hasHeightForWidth());
        PushButton15->setSizePolicy(sizePolicy4);

        verticalLayout_2->addWidget(PushButton15);

        PushButton16 = new QPushButton(groupBox_6);
        PushButton16->setObjectName(QString::fromUtf8("PushButton16"));
        sizePolicy4.setHeightForWidth(PushButton16->sizePolicy().hasHeightForWidth());
        PushButton16->setSizePolicy(sizePolicy4);

        verticalLayout_2->addWidget(PushButton16);

        PushButton17 = new QPushButton(groupBox_6);
        PushButton17->setObjectName(QString::fromUtf8("PushButton17"));
        sizePolicy4.setHeightForWidth(PushButton17->sizePolicy().hasHeightForWidth());
        PushButton17->setSizePolicy(sizePolicy4);

        verticalLayout_2->addWidget(PushButton17);

        PushButton18 = new QPushButton(groupBox_6);
        PushButton18->setObjectName(QString::fromUtf8("PushButton18"));
        sizePolicy4.setHeightForWidth(PushButton18->sizePolicy().hasHeightForWidth());
        PushButton18->setSizePolicy(sizePolicy4);

        verticalLayout_2->addWidget(PushButton18);


        horizontalLayout_2->addLayout(verticalLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        PushButton34 = new QPushButton(groupBox_6);
        PushButton34->setObjectName(QString::fromUtf8("PushButton34"));
        sizePolicy4.setHeightForWidth(PushButton34->sizePolicy().hasHeightForWidth());
        PushButton34->setSizePolicy(sizePolicy4);

        verticalLayout_3->addWidget(PushButton34);

        PushButton33 = new QPushButton(groupBox_6);
        PushButton33->setObjectName(QString::fromUtf8("PushButton33"));
        sizePolicy4.setHeightForWidth(PushButton33->sizePolicy().hasHeightForWidth());
        PushButton33->setSizePolicy(sizePolicy4);

        verticalLayout_3->addWidget(PushButton33);

        PushButton35 = new QPushButton(groupBox_6);
        PushButton35->setObjectName(QString::fromUtf8("PushButton35"));
        sizePolicy4.setHeightForWidth(PushButton35->sizePolicy().hasHeightForWidth());
        PushButton35->setSizePolicy(sizePolicy4);

        verticalLayout_3->addWidget(PushButton35);

        PushButton36 = new QPushButton(groupBox_6);
        PushButton36->setObjectName(QString::fromUtf8("PushButton36"));
        sizePolicy4.setHeightForWidth(PushButton36->sizePolicy().hasHeightForWidth());
        PushButton36->setSizePolicy(sizePolicy4);

        verticalLayout_3->addWidget(PushButton36);


        horizontalLayout_2->addLayout(verticalLayout_3);


        verticalLayout_4->addLayout(horizontalLayout_2);

        PushButton10 = new QPushButton(groupBox_6);
        PushButton10->setObjectName(QString::fromUtf8("PushButton10"));
        sizePolicy4.setHeightForWidth(PushButton10->sizePolicy().hasHeightForWidth());
        PushButton10->setSizePolicy(sizePolicy4);

        verticalLayout_4->addWidget(PushButton10);


        gridLayout_2->addLayout(verticalLayout_4, 0, 0, 1, 1);


        horizontalLayout_7->addWidget(groupBox_6);

        groupBox_7 = new QGroupBox(playerTab);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        groupBox_7->setFont(font);
        gridLayout_5 = new QGridLayout(groupBox_7);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(2, 2, 2, 2);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        PushButton26 = new QPushButton(groupBox_7);
        PushButton26->setObjectName(QString::fromUtf8("PushButton26"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Icons/slow-pan.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PushButton26->setIcon(icon);
        PushButton26->setCheckable(true);
        PushButton26->setAutoExclusive(true);

        verticalLayout_7->addWidget(PushButton26);

        PushButton28 = new QPushButton(groupBox_7);
        PushButton28->setObjectName(QString::fromUtf8("PushButton28"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Icons/slow-up-saw.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PushButton28->setIcon(icon1);
        PushButton28->setCheckable(true);
        PushButton28->setAutoExclusive(true);

        verticalLayout_7->addWidget(PushButton28);

        PushButton30 = new QPushButton(groupBox_7);
        PushButton30->setObjectName(QString::fromUtf8("PushButton30"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Icons/slow-down-saw.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PushButton30->setIcon(icon2);
        PushButton30->setCheckable(true);
        PushButton30->setAutoExclusive(true);

        verticalLayout_7->addWidget(PushButton30);

        PushButton42 = new QPushButton(groupBox_7);
        PushButton42->setObjectName(QString::fromUtf8("PushButton42"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Icons/break.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PushButton42->setIcon(icon3);
        PushButton42->setCheckable(true);
        PushButton42->setAutoExclusive(true);

        verticalLayout_7->addWidget(PushButton42);


        horizontalLayout_5->addLayout(verticalLayout_7);

        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        PushButton27 = new QPushButton(groupBox_7);
        PushButton27->setObjectName(QString::fromUtf8("PushButton27"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Icons/fast-pan.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PushButton27->setIcon(icon4);
        PushButton27->setCheckable(true);
        PushButton27->setAutoExclusive(true);

        verticalLayout_8->addWidget(PushButton27);

        PushButton29 = new QPushButton(groupBox_7);
        PushButton29->setObjectName(QString::fromUtf8("PushButton29"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/Icons/fast-up-saw.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PushButton29->setIcon(icon5);
        PushButton29->setCheckable(true);
        PushButton29->setAutoExclusive(true);

        verticalLayout_8->addWidget(PushButton29);

        PushButton31 = new QPushButton(groupBox_7);
        PushButton31->setObjectName(QString::fromUtf8("PushButton31"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/Icons/fast-down-saw.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PushButton31->setIcon(icon6);
        PushButton31->setCheckable(true);
        PushButton31->setAutoExclusive(true);

        verticalLayout_8->addWidget(PushButton31);

        PushButton39 = new QPushButton(groupBox_7);
        PushButton39->setObjectName(QString::fromUtf8("PushButton39"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/Icons/normal.xpm"), QSize(), QIcon::Normal, QIcon::Off);
        PushButton39->setIcon(icon7);
        PushButton39->setCheckable(true);
        PushButton39->setAutoExclusive(true);

        verticalLayout_8->addWidget(PushButton39);


        horizontalLayout_5->addLayout(verticalLayout_8);


        verticalLayout_9->addLayout(horizontalLayout_5);

        PushButton41 = new QPushButton(groupBox_7);
        PushButton41->setObjectName(QString::fromUtf8("PushButton41"));

        verticalLayout_9->addWidget(PushButton41);


        gridLayout_5->addLayout(verticalLayout_9, 0, 0, 1, 1);


        horizontalLayout_7->addWidget(groupBox_7);


        verticalLayout_10->addLayout(horizontalLayout_7);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        tempoSwitchTime = new QSpinBox(playerTab);
        tempoSwitchTime->setObjectName(QString::fromUtf8("tempoSwitchTime"));
        tempoSwitchTime->setMinimum(1);
        tempoSwitchTime->setMaximum(180);
        tempoSwitchTime->setSingleStep(10);
        tempoSwitchTime->setValue(10);

        horizontalLayout_6->addWidget(tempoSwitchTime);

        PushButton21 = new QPushButton(playerTab);
        PushButton21->setObjectName(QString::fromUtf8("PushButton21"));
        QSizePolicy sizePolicy5(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(PushButton21->sizePolicy().hasHeightForWidth());
        PushButton21->setSizePolicy(sizePolicy5);

        horizontalLayout_6->addWidget(PushButton21);

        switcherButton = new QPushButton(playerTab);
        switcherButton->setObjectName(QString::fromUtf8("switcherButton"));
        sizePolicy5.setHeightForWidth(switcherButton->sizePolicy().hasHeightForWidth());
        switcherButton->setSizePolicy(sizePolicy5);

        horizontalLayout_6->addWidget(switcherButton);

        PushButton20 = new QPushButton(playerTab);
        PushButton20->setObjectName(QString::fromUtf8("PushButton20"));
        sizePolicy5.setHeightForWidth(PushButton20->sizePolicy().hasHeightForWidth());
        PushButton20->setSizePolicy(sizePolicy5);

        horizontalLayout_6->addWidget(PushButton20);

        PushButton24 = new QPushButton(playerTab);
        PushButton24->setObjectName(QString::fromUtf8("PushButton24"));
        sizePolicy4.setHeightForWidth(PushButton24->sizePolicy().hasHeightForWidth());
        PushButton24->setSizePolicy(sizePolicy4);

        horizontalLayout_6->addWidget(PushButton24);

        startStopButton = new QPushButton(playerTab);
        startStopButton->setObjectName(QString::fromUtf8("startStopButton"));
        sizePolicy4.setHeightForWidth(startStopButton->sizePolicy().hasHeightForWidth());
        startStopButton->setSizePolicy(sizePolicy4);

        horizontalLayout_6->addWidget(startStopButton);

        AboutButton = new QPushButton(playerTab);
        AboutButton->setObjectName(QString::fromUtf8("AboutButton"));
        AboutButton->setEnabled(true);
        sizePolicy5.setHeightForWidth(AboutButton->sizePolicy().hasHeightForWidth());
        AboutButton->setSizePolicy(sizePolicy5);

        horizontalLayout_6->addWidget(AboutButton);


        verticalLayout_10->addLayout(horizontalLayout_6);


        gridLayout_6->addLayout(verticalLayout_10, 0, 0, 1, 1);

        tab->addTab(playerTab, QString());
        scramblerTab = new QWidget();
        scramblerTab->setObjectName(QString::fromUtf8("scramblerTab"));
        gridLayout_3 = new QGridLayout(scramblerTab);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        verticalLayout_20 = new QVBoxLayout();
        verticalLayout_20->setSpacing(6);
        verticalLayout_20->setObjectName(QString::fromUtf8("verticalLayout_20"));
        vboxLayout = new QVBoxLayout();
        vboxLayout->setSpacing(3);
        vboxLayout->setContentsMargins(0, 0, 0, 0);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        mapin = new QLabel(scramblerTab);
        mapin->setObjectName(QString::fromUtf8("mapin"));
        QSizePolicy sizePolicy6(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(mapin->sizePolicy().hasHeightForWidth());
        mapin->setSizePolicy(sizePolicy6);
        mapin->setMinimumSize(QSize(0, 28));
        mapin->setScaledContents(true);
        mapin->setWordWrap(false);

        vboxLayout->addWidget(mapin);

        mapout = new QLabel(scramblerTab);
        mapout->setObjectName(QString::fromUtf8("mapout"));
        sizePolicy6.setHeightForWidth(mapout->sizePolicy().hasHeightForWidth());
        mapout->setSizePolicy(sizePolicy6);
        mapout->setMinimumSize(QSize(0, 28));
        mapout->setScaledContents(true);
        mapout->setWordWrap(false);

        vboxLayout->addWidget(mapout);

        mapspeed = new QLabel(scramblerTab);
        mapspeed->setObjectName(QString::fromUtf8("mapspeed"));
        sizePolicy6.setHeightForWidth(mapspeed->sizePolicy().hasHeightForWidth());
        mapspeed->setSizePolicy(sizePolicy6);
        mapspeed->setMinimumSize(QSize(0, 28));
        mapspeed->setScaledContents(true);
        mapspeed->setWordWrap(false);

        vboxLayout->addWidget(mapspeed);

        mapvolume = new QLabel(scramblerTab);
        mapvolume->setObjectName(QString::fromUtf8("mapvolume"));
        sizePolicy6.setHeightForWidth(mapvolume->sizePolicy().hasHeightForWidth());
        mapvolume->setSizePolicy(sizePolicy6);
        mapvolume->setMinimumSize(QSize(0, 28));
        mapvolume->setScaledContents(true);
        mapvolume->setWordWrap(false);

        vboxLayout->addWidget(mapvolume);


        verticalLayout_20->addLayout(vboxLayout);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        pushButton49 = new QPushButton(scramblerTab);
        pushButton49->setObjectName(QString::fromUtf8("pushButton49"));

        horizontalLayout_10->addWidget(pushButton49);

        pushButton50_2 = new QPushButton(scramblerTab);
        pushButton50_2->setObjectName(QString::fromUtf8("pushButton50_2"));

        horizontalLayout_10->addWidget(pushButton50_2);

        pushButton51 = new QPushButton(scramblerTab);
        pushButton51->setObjectName(QString::fromUtf8("pushButton51"));

        horizontalLayout_10->addWidget(pushButton51);


        verticalLayout_20->addLayout(horizontalLayout_10);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        verticalLayout_17 = new QVBoxLayout();
        verticalLayout_17->setSpacing(6);
        verticalLayout_17->setObjectName(QString::fromUtf8("verticalLayout_17"));
        pushButton50 = new QPushButton(scramblerTab);
        pushButton50->setObjectName(QString::fromUtf8("pushButton50"));
        sizePolicy3.setHeightForWidth(pushButton50->sizePolicy().hasHeightForWidth());
        pushButton50->setSizePolicy(sizePolicy3);

        verticalLayout_17->addWidget(pushButton50);

        pushButton77 = new QPushButton(scramblerTab);
        pushButton77->setObjectName(QString::fromUtf8("pushButton77"));

        verticalLayout_17->addWidget(pushButton77);


        horizontalLayout_11->addLayout(verticalLayout_17);

        verticalLayout_18 = new QVBoxLayout();
        verticalLayout_18->setSpacing(6);
        verticalLayout_18->setObjectName(QString::fromUtf8("verticalLayout_18"));
        mapout_length = new QSpinBox(scramblerTab);
        mapout_length->setObjectName(QString::fromUtf8("mapout_length"));
        sizePolicy3.setHeightForWidth(mapout_length->sizePolicy().hasHeightForWidth());
        mapout_length->setSizePolicy(sizePolicy3);
        mapout_length->setFocusPolicy(Qt::NoFocus);
        mapout_length->setMinimum(1);
        mapout_length->setMaximum(256);
        mapout_length->setSingleStep(1);
        mapout_length->setValue(64);

        verticalLayout_18->addWidget(mapout_length);

        mapping_scale = new QComboBox(scramblerTab);
        mapping_scale->setObjectName(QString::fromUtf8("mapping_scale"));
        QSizePolicy sizePolicy7(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(mapping_scale->sizePolicy().hasHeightForWidth());
        mapping_scale->setSizePolicy(sizePolicy7);
        mapping_scale->setFocusPolicy(Qt::WheelFocus);
        mapping_scale->setEditable(false);
        mapping_scale->setDuplicatesEnabled(false);

        verticalLayout_18->addWidget(mapping_scale);


        horizontalLayout_11->addLayout(verticalLayout_18);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        groupBox = new QGroupBox(scramblerTab);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_20 = new QGridLayout(groupBox);
        gridLayout_20->setSpacing(6);
        gridLayout_20->setContentsMargins(2, 2, 2, 2);
        gridLayout_20->setObjectName(QString::fromUtf8("gridLayout_20"));
        gridLayout_20->setHorizontalSpacing(2);
        gridLayout_20->setVerticalSpacing(0);
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        atend_restart = new QRadioButton(groupBox);
        atend_restart->setObjectName(QString::fromUtf8("atend_restart"));
        atend_restart->setChecked(true);

        horizontalLayout_9->addWidget(atend_restart);

        atend_continue = new QRadioButton(groupBox);
        atend_continue->setObjectName(QString::fromUtf8("atend_continue"));
        atend_continue->setChecked(false);

        horizontalLayout_9->addWidget(atend_continue);

        atend_stop = new QRadioButton(groupBox);
        atend_stop->setObjectName(QString::fromUtf8("atend_stop"));

        horizontalLayout_9->addWidget(atend_stop);


        gridLayout_20->addLayout(horizontalLayout_9, 0, 4, 1, 1);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setSpacing(6);
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        atend_loop = new QCheckBox(groupBox);
        atend_loop->setObjectName(QString::fromUtf8("atend_loop"));

        horizontalLayout_12->addWidget(atend_loop);

        atend_c = new QRadioButton(groupBox);
        atend_c->setObjectName(QString::fromUtf8("atend_c"));

        horizontalLayout_12->addWidget(atend_c);

        atend_x = new QRadioButton(groupBox);
        atend_x->setObjectName(QString::fromUtf8("atend_x"));

        horizontalLayout_12->addWidget(atend_x);

        atend_v = new QRadioButton(groupBox);
        atend_v->setObjectName(QString::fromUtf8("atend_v"));

        horizontalLayout_12->addWidget(atend_v);

        atend_z = new QRadioButton(groupBox);
        atend_z->setObjectName(QString::fromUtf8("atend_z"));

        horizontalLayout_12->addWidget(atend_z);


        gridLayout_20->addLayout(horizontalLayout_12, 1, 4, 1, 1);


        horizontalLayout_4->addWidget(groupBox);


        horizontalLayout_11->addLayout(horizontalLayout_4);

        verticalLayout_19 = new QVBoxLayout();
        verticalLayout_19->setSpacing(6);
        verticalLayout_19->setObjectName(QString::fromUtf8("verticalLayout_19"));
        pushButton48 = new QPushButton(scramblerTab);
        pushButton48->setObjectName(QString::fromUtf8("pushButton48"));

        verticalLayout_19->addWidget(pushButton48);

        pushButton47 = new QPushButton(scramblerTab);
        pushButton47->setObjectName(QString::fromUtf8("pushButton47"));

        verticalLayout_19->addWidget(pushButton47);


        horizontalLayout_11->addLayout(verticalLayout_19);


        verticalLayout_20->addLayout(horizontalLayout_11);


        gridLayout_3->addLayout(verticalLayout_20, 0, 0, 1, 1);

        tab->addTab(scramblerTab, QString());
        beatgraphTab = new QWidget();
        beatgraphTab->setObjectName(QString::fromUtf8("beatgraphTab"));
        gridLayout1 = new QGridLayout(beatgraphTab);
        gridLayout1->setSpacing(6);
        gridLayout1->setContentsMargins(0, 0, 0, 0);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        beatGraphAnalyzer = new BeatGraphAnalyzer(beatgraphTab);
        beatGraphAnalyzer->setObjectName(QString::fromUtf8("beatGraphAnalyzer"));

        gridLayout1->addWidget(beatGraphAnalyzer, 0, 0, 1, 1);

        tab->addTab(beatgraphTab, QString());
        bpmTab = new QWidget();
        bpmTab->setObjectName(QString::fromUtf8("bpmTab"));
        gridLayout2 = new QGridLayout(bpmTab);
        gridLayout2->setSpacing(6);
        gridLayout2->setContentsMargins(9, 9, 9, 9);
        gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
        bpmCounter = new BpmAnalyzerDialog(bpmTab);
        bpmCounter->setObjectName(QString::fromUtf8("bpmCounter"));

        gridLayout2->addWidget(bpmCounter, 0, 0, 1, 1);

        tab->addTab(bpmTab, QString());
        spectrumTab = new QWidget();
        spectrumTab->setObjectName(QString::fromUtf8("spectrumTab"));
        gridLayout3 = new QGridLayout(spectrumTab);
        gridLayout3->setSpacing(6);
        gridLayout3->setContentsMargins(9, 9, 9, 9);
        gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
        spectrum = new SpectrumAnalyzer(spectrumTab);
        spectrum->setObjectName(QString::fromUtf8("spectrum"));

        gridLayout3->addWidget(spectrum, 0, 0, 1, 1);

        tab->addTab(spectrumTab, QString());
        rhythmTab = new QWidget();
        rhythmTab->setObjectName(QString::fromUtf8("rhythmTab"));
        gridLayout4 = new QGridLayout(rhythmTab);
        gridLayout4->setSpacing(6);
        gridLayout4->setContentsMargins(9, 9, 9, 9);
        gridLayout4->setObjectName(QString::fromUtf8("gridLayout4"));
        widget = new RhythmAnalyzer(rhythmTab);
        widget->setObjectName(QString::fromUtf8("widget"));

        gridLayout4->addWidget(widget, 0, 0, 1, 1);

        tab->addTab(rhythmTab, QString());
        songTab = new QWidget();
        songTab->setObjectName(QString::fromUtf8("songTab"));
        gridLayout5 = new QGridLayout(songTab);
        gridLayout5->setSpacing(6);
        gridLayout5->setContentsMargins(9, 9, 9, 9);
        gridLayout5->setObjectName(QString::fromUtf8("gridLayout5"));
        songInfo = new SongInformation(songTab);
        songInfo->setObjectName(QString::fromUtf8("songInfo"));

        gridLayout5->addWidget(songInfo, 0, 0, 1, 1);

        tab->addTab(songTab, QString());
        setupTab = new QWidget();
        setupTab->setObjectName(QString::fromUtf8("setupTab"));
        gridLayout6 = new QGridLayout(setupTab);
        gridLayout6->setSpacing(6);
        gridLayout6->setContentsMargins(9, 9, 9, 9);
        gridLayout6->setObjectName(QString::fromUtf8("gridLayout6"));
        spacerItem = new QSpacerItem(360, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout6->addItem(spacerItem, 1, 0, 1, 1);

        pushButton90 = new QPushButton(setupTab);
        pushButton90->setObjectName(QString::fromUtf8("pushButton90"));

        gridLayout6->addWidget(pushButton90, 1, 1, 1, 1);

        tabWidget = new QTabWidget(setupTab);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        decodingtab = new QWidget();
        decodingtab->setObjectName(QString::fromUtf8("decodingtab"));
        gridLayout7 = new QGridLayout(decodingtab);
        gridLayout7->setSpacing(6);
        gridLayout7->setContentsMargins(9, 9, 9, 9);
        gridLayout7->setObjectName(QString::fromUtf8("gridLayout7"));
        rms_toggle = new QCheckBox(decodingtab);
        rms_toggle->setObjectName(QString::fromUtf8("rms_toggle"));

        gridLayout7->addWidget(rms_toggle, 3, 0, 1, 3);

        textLabel2_6 = new QLabel(decodingtab);
        textLabel2_6->setObjectName(QString::fromUtf8("textLabel2_6"));
        QSizePolicy sizePolicy8(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy8.setHorizontalStretch(0);
        sizePolicy8.setVerticalStretch(0);
        sizePolicy8.setHeightForWidth(textLabel2_6->sizePolicy().hasHeightForWidth());
        textLabel2_6->setSizePolicy(sizePolicy8);
        textLabel2_6->setWordWrap(false);

        gridLayout7->addWidget(textLabel2_6, 0, 0, 1, 3);

        rms = new QLineEdit(decodingtab);
        rms->setObjectName(QString::fromUtf8("rms"));

        gridLayout7->addWidget(rms, 4, 1, 1, 2);

        textLabel2 = new QLabel(decodingtab);
        textLabel2->setObjectName(QString::fromUtf8("textLabel2"));
        textLabel2->setWordWrap(false);

        gridLayout7->addWidget(textLabel2, 4, 0, 1, 1);

        textLabel1_3 = new QLabel(decodingtab);
        textLabel1_3->setObjectName(QString::fromUtf8("textLabel1_3"));
        textLabel1_3->setWordWrap(false);

        gridLayout7->addWidget(textLabel1_3, 2, 0, 1, 2);

        capacity = new CapacityWidget(decodingtab);
        capacity->setObjectName(QString::fromUtf8("capacity"));

        gridLayout7->addWidget(capacity, 1, 0, 1, 3);

        raw_directory = new QLineEdit(decodingtab);
        raw_directory->setObjectName(QString::fromUtf8("raw_directory"));

        gridLayout7->addWidget(raw_directory, 2, 2, 1, 1);

        tabWidget->addTab(decodingtab, QString());
        alsatab = new QWidget();
        alsatab->setObjectName(QString::fromUtf8("alsatab"));
        gridLayout8 = new QGridLayout(alsatab);
        gridLayout8->setSpacing(6);
        gridLayout8->setContentsMargins(9, 9, 9, 9);
        gridLayout8->setObjectName(QString::fromUtf8("gridLayout8"));
        textLabel9 = new QLabel(alsatab);
        textLabel9->setObjectName(QString::fromUtf8("textLabel9"));
        textLabel9->setWordWrap(false);

        gridLayout8->addWidget(textLabel9, 1, 0, 1, 1);

        alsa_latency = new QSpinBox(alsatab);
        alsa_latency->setObjectName(QString::fromUtf8("alsa_latency"));
        alsa_latency->setMinimum(50);
        alsa_latency->setMaximum(1000);
        alsa_latency->setSingleStep(10);
        alsa_latency->setValue(150);

        gridLayout8->addWidget(alsa_latency, 2, 1, 1, 2);

        spacerItem1 = new QSpacerItem(20, 149, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout8->addItem(spacerItem1, 4, 1, 1, 1);

        alsa_verbose = new QCheckBox(alsatab);
        alsa_verbose->setObjectName(QString::fromUtf8("alsa_verbose"));

        gridLayout8->addWidget(alsa_verbose, 3, 0, 1, 3);

        textLabel3 = new QLabel(alsatab);
        textLabel3->setObjectName(QString::fromUtf8("textLabel3"));
        textLabel3->setWordWrap(false);

        gridLayout8->addWidget(textLabel3, 2, 0, 1, 1);

        alsa = new QCheckBox(alsatab);
        alsa->setObjectName(QString::fromUtf8("alsa"));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        alsa->setFont(font1);
        alsa->setChecked(true);

        gridLayout8->addWidget(alsa, 0, 0, 1, 3);

        alsa_dev = new QLineEdit(alsatab);
        alsa_dev->setObjectName(QString::fromUtf8("alsa_dev"));

        gridLayout8->addWidget(alsa_dev, 1, 1, 1, 1);

        comboBox2 = new QComboBox(alsatab);
        comboBox2->setObjectName(QString::fromUtf8("comboBox2"));

        gridLayout8->addWidget(comboBox2, 1, 2, 1, 1);

        tabWidget->addTab(alsatab, QString());
        osstab = new QWidget();
        osstab->setObjectName(QString::fromUtf8("osstab"));
        gridLayout9 = new QGridLayout(osstab);
        gridLayout9->setSpacing(6);
        gridLayout9->setContentsMargins(9, 9, 9, 9);
        gridLayout9->setObjectName(QString::fromUtf8("gridLayout9"));
        spacerItem2 = new QSpacerItem(20, 51, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout9->addItem(spacerItem2, 7, 0, 1, 2);

        textLabel7 = new QLabel(osstab);
        textLabel7->setObjectName(QString::fromUtf8("textLabel7"));
        textLabel7->setWordWrap(false);

        gridLayout9->addWidget(textLabel7, 3, 0, 1, 1);

        oss_verbose = new QCheckBox(osstab);
        oss_verbose->setObjectName(QString::fromUtf8("oss_verbose"));

        gridLayout9->addWidget(oss_verbose, 6, 0, 1, 3);

        textLabel6 = new QLabel(osstab);
        textLabel6->setObjectName(QString::fromUtf8("textLabel6"));
        textLabel6->setWordWrap(false);

        gridLayout9->addWidget(textLabel6, 1, 0, 1, 1);

        oss_init_fragments = new QCheckBox(osstab);
        oss_init_fragments->setObjectName(QString::fromUtf8("oss_init_fragments"));

        gridLayout9->addWidget(oss_init_fragments, 2, 0, 1, 3);

        oss = new QCheckBox(osstab);
        oss->setObjectName(QString::fromUtf8("oss"));
        oss->setFont(font1);

        gridLayout9->addWidget(oss, 0, 0, 1, 3);

        textLabel3_3 = new QLabel(osstab);
        textLabel3_3->setObjectName(QString::fromUtf8("textLabel3_3"));
        textLabel3_3->setWordWrap(false);

        gridLayout9->addWidget(textLabel3_3, 4, 0, 1, 1);

        oss_latency = new QSpinBox(osstab);
        oss_latency->setObjectName(QString::fromUtf8("oss_latency"));
        oss_latency->setMinimum(50);
        oss_latency->setMaximum(1000);
        oss_latency->setSingleStep(25);
        oss_latency->setValue(300);

        gridLayout9->addWidget(oss_latency, 4, 1, 1, 2);

        oss_dsp = new QLineEdit(osstab);
        oss_dsp->setObjectName(QString::fromUtf8("oss_dsp"));

        gridLayout9->addWidget(oss_dsp, 1, 1, 1, 1);

        oss_fragments = new QSpinBox(osstab);
        oss_fragments->setObjectName(QString::fromUtf8("oss_fragments"));
        oss_fragments->setMinimum(1);
        oss_fragments->setMaximum(100);
        oss_fragments->setValue(16);

        gridLayout9->addWidget(oss_fragments, 3, 1, 1, 2);

        oss_nolatencyaccounting = new QCheckBox(osstab);
        oss_nolatencyaccounting->setObjectName(QString::fromUtf8("oss_nolatencyaccounting"));

        gridLayout9->addWidget(oss_nolatencyaccounting, 5, 0, 1, 3);

        comboBox3 = new QComboBox(osstab);
        comboBox3->setObjectName(QString::fromUtf8("comboBox3"));

        gridLayout9->addWidget(comboBox3, 1, 2, 1, 1);

        tabWidget->addTab(osstab, QString());
        jacktab = new QWidget();
        jacktab->setObjectName(QString::fromUtf8("jacktab"));
        subgridLayout = new QGridLayout(jacktab);
        subgridLayout->setSpacing(6);
        subgridLayout->setContentsMargins(11, 11, 11, 11);
        subgridLayout->setObjectName(QString::fromUtf8("subgridLayout"));
        jack = new QCheckBox(jacktab);
        jack->setObjectName(QString::fromUtf8("jack"));
        jack->setEnabled(true);
        jack->setFont(font1);

        subgridLayout->addWidget(jack, 0, 0, 1, 3);

        outputchannel_label = new QLabel(jacktab);
        outputchannel_label->setObjectName(QString::fromUtf8("outputchannel_label"));
        outputchannel_label->setEnabled(true);
        outputchannel_label->setWordWrap(false);

        subgridLayout->addWidget(outputchannel_label, 1, 0, 1, 1);

        horizontalLayout1 = new QHBoxLayout();
        horizontalLayout1->setSpacing(6);
        horizontalLayout1->setObjectName(QString::fromUtf8("horizontalLayout1"));
        jack_dev = new QLineEdit(jacktab);
        jack_dev->setObjectName(QString::fromUtf8("jack_dev"));
        jack_dev->setEnabled(true);

        horizontalLayout1->addWidget(jack_dev);

        jackcombo = new QComboBox(jacktab);
        jackcombo->setObjectName(QString::fromUtf8("jackcombo"));

        horizontalLayout1->addWidget(jackcombo);


        subgridLayout->addLayout(horizontalLayout1, 1, 1, 1, 2);

        connectchannell_label = new QLabel(jacktab);
        connectchannell_label->setObjectName(QString::fromUtf8("connectchannell_label"));
        connectchannell_label->setEnabled(true);
        connectchannell_label->setWordWrap(false);

        subgridLayout->addWidget(connectchannell_label, 2, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        jack_lout = new QLineEdit(jacktab);
        jack_lout->setObjectName(QString::fromUtf8("jack_lout"));
        jack_lout->setEnabled(true);

        horizontalLayout->addWidget(jack_lout);

        jackloutcombo = new QComboBox(jacktab);
        jackloutcombo->setObjectName(QString::fromUtf8("jackloutcombo"));

        horizontalLayout->addWidget(jackloutcombo);


        subgridLayout->addLayout(horizontalLayout, 2, 1, 1, 2);

        connectchannelr_label = new QLabel(jacktab);
        connectchannelr_label->setObjectName(QString::fromUtf8("connectchannelr_label"));
        connectchannelr_label->setEnabled(true);
        connectchannelr_label->setWordWrap(false);

        subgridLayout->addWidget(connectchannelr_label, 3, 0, 1, 1);

        horizontalLayoutLR = new QHBoxLayout();
        horizontalLayoutLR->setSpacing(6);
        horizontalLayoutLR->setObjectName(QString::fromUtf8("horizontalLayoutLR"));
        jack_rout = new QLineEdit(jacktab);
        jack_rout->setObjectName(QString::fromUtf8("jack_rout"));
        jack_rout->setEnabled(true);

        horizontalLayoutLR->addWidget(jack_rout);

        jackroutcombo = new QComboBox(jacktab);
        jackroutcombo->setObjectName(QString::fromUtf8("jackroutcombo"));

        horizontalLayoutLR->addWidget(jackroutcombo);


        subgridLayout->addLayout(horizontalLayoutLR, 3, 1, 1, 2);

        jack_verbose = new QCheckBox(jacktab);
        jack_verbose->setObjectName(QString::fromUtf8("jack_verbose"));
        jack_verbose->setEnabled(true);

        subgridLayout->addWidget(jack_verbose, 4, 0, 1, 3);

        verticalSpacer = new QSpacerItem(20, 124, QSizePolicy::Minimum, QSizePolicy::Expanding);

        subgridLayout->addItem(verticalSpacer, 5, 2, 1, 1);

        tabWidget->addTab(jacktab, QString());

        gridLayout6->addWidget(tabWidget, 0, 0, 1, 2);

        tab->addTab(setupTab, QString());

        gridLayout->addWidget(tab, 0, 0, 1, 1);

        QWidget::setTabOrder(startStopButton, ButtonMinusHalfB);
        QWidget::setTabOrder(ButtonMinusHalfB, PushButton19);
        QWidget::setTabOrder(PushButton19, ButtonPlus8M);
        QWidget::setTabOrder(ButtonPlus8M, ButtonMinus8M);
        QWidget::setTabOrder(ButtonMinus8M, ButtonMinus4M);
        QWidget::setTabOrder(ButtonMinus4M, ButtonMinus1M);
        QWidget::setTabOrder(ButtonMinus1M, KeyMinus);
        QWidget::setTabOrder(KeyMinus, ButtonPlus4M);
        QWidget::setTabOrder(ButtonPlus4M, KeyPlus);
        QWidget::setTabOrder(KeyPlus, ButtonPlus1M);
        QWidget::setTabOrder(ButtonPlus1M, PushButton24);
        QWidget::setTabOrder(PushButton24, PushButton34);
        QWidget::setTabOrder(PushButton34, PushButton35);
        QWidget::setTabOrder(PushButton35, PushButton12);
        QWidget::setTabOrder(PushButton12, PushButton13);
        QWidget::setTabOrder(PushButton13, PushButton14);
        QWidget::setTabOrder(PushButton14, PushButton11);
        QWidget::setTabOrder(PushButton11, PushButton18);
        QWidget::setTabOrder(PushButton18, PushButton16);
        QWidget::setTabOrder(PushButton16, PushButton15);
        QWidget::setTabOrder(PushButton15, switcherButton);
        QWidget::setTabOrder(switcherButton, PushButton20);
        QWidget::setTabOrder(PushButton20, pushButton48);
        QWidget::setTabOrder(pushButton48, pushButton47);
        QWidget::setTabOrder(pushButton47, mapping_scale);
        QWidget::setTabOrder(mapping_scale, pushButton50);

        retranslateUi(SongPlayer);
        QObject::connect(AboutButton, SIGNAL(clicked()), SongPlayer, SLOT(openAbout()));
        QObject::connect(ButtonMinus1M, SIGNAL(pressed()), SongPlayer, SLOT(nudgeMinus1M()));
        QObject::connect(ButtonMinus4M, SIGNAL(pressed()), SongPlayer, SLOT(nudgeMinus4M()));
        QObject::connect(ButtonMinus8M, SIGNAL(pressed()), SongPlayer, SLOT(nudgeMinus8M()));
        QObject::connect(ButtonMinusHalfB, SIGNAL(pressed()), SongPlayer, SLOT(nudgeMinusHalfB()));
        QObject::connect(ButtonPlus1M, SIGNAL(pressed()), SongPlayer, SLOT(nudgePlus1M()));
        QObject::connect(ButtonPlus4M, SIGNAL(pressed()), SongPlayer, SLOT(nudgePlus4M()));
        QObject::connect(ButtonPlus8M, SIGNAL(pressed()), SongPlayer, SLOT(nudgePlus8M()));
        QObject::connect(KeyMinus, SIGNAL(clicked()), SongPlayer, SLOT(nudgeMinus()));
        QObject::connect(KeyPlus, SIGNAL(clicked()), SongPlayer, SLOT(nudgePlus()));
        QObject::connect(PushButton10, SIGNAL(pressed()), SongPlayer, SLOT(setCue()));
        QObject::connect(PushButton11, SIGNAL(pressed()), SongPlayer, SLOT(retrieveZ()));
        QObject::connect(PushButton12, SIGNAL(pressed()), SongPlayer, SLOT(retrieveX()));
        QObject::connect(PushButton13, SIGNAL(pressed()), SongPlayer, SLOT(retrieveC()));
        QObject::connect(PushButton14, SIGNAL(pressed()), SongPlayer, SLOT(retrieveV()));
        QObject::connect(PushButton15, SIGNAL(clicked()), SongPlayer, SLOT(storeZ()));
        QObject::connect(PushButton16, SIGNAL(clicked()), SongPlayer, SLOT(storeX()));
        QObject::connect(PushButton17, SIGNAL(clicked()), SongPlayer, SLOT(storeC()));
        QObject::connect(PushButton18, SIGNAL(clicked()), SongPlayer, SLOT(storeV()));
        QObject::connect(PushButton19, SIGNAL(clicked()), SongPlayer, SLOT(nudgePlusB()));
        QObject::connect(PushButton20, SIGNAL(clicked()), SongPlayer, SLOT(targetTempo()));
        QObject::connect(PushButton21, SIGNAL(clicked()), SongPlayer, SLOT(normalTempo()));
        QObject::connect(PushButton24, SIGNAL(clicked()), SongPlayer, SLOT(restart()));
        QObject::connect(PushButton26, SIGNAL(pressed()), SongPlayer, SLOT(slowPan()));
        QObject::connect(PushButton27, SIGNAL(pressed()), SongPlayer, SLOT(fastPan()));
        QObject::connect(PushButton28, SIGNAL(pressed()), SongPlayer, SLOT(slowSaw()));
        QObject::connect(PushButton29, SIGNAL(pressed()), SongPlayer, SLOT(fastSaw()));
        QObject::connect(PushButton30, SIGNAL(pressed()), SongPlayer, SLOT(slowRevSaw()));
        QObject::connect(PushButton31, SIGNAL(pressed()), SongPlayer, SLOT(fastRevSaw()));
        QObject::connect(PushButton33, SIGNAL(clicked()), SongPlayer, SLOT(nudgeCueForward()));
        QObject::connect(PushButton34, SIGNAL(clicked()), SongPlayer, SLOT(nudgeCueBack()));
        QObject::connect(PushButton35, SIGNAL(released()), SongPlayer, SLOT(nudgeCueBack8M()));
        QObject::connect(PushButton36, SIGNAL(clicked()), SongPlayer, SLOT(nudgeCueForward8M()));
        QObject::connect(PushButton39, SIGNAL(pressed()), SongPlayer, SLOT(normalLfo()));
        QObject::connect(PushButton41, SIGNAL(pressed()), SongPlayer, SLOT(metronome()));
        QObject::connect(PushButton42, SIGNAL(pressed()), SongPlayer, SLOT(breakLfo()));
        QObject::connect(alsa, SIGNAL(pressed()), SongPlayer, SLOT(setAlsa()));
        QObject::connect(atend_loop, SIGNAL(clicked()), SongPlayer, SLOT(mapLoopChange()));
        QObject::connect(comboBox2, SIGNAL(activated(QString)), alsa_dev, SLOT(setText(QString)));
        QObject::connect(comboBox3, SIGNAL(activated(QString)), oss_dsp, SLOT(setText(QString)));
        QObject::connect(mapout_length, SIGNAL(valueChanged(int)), SongPlayer, SLOT(mapLengthChanged(int)));
        QObject::connect(mapping_scale, SIGNAL(activated(int)), SongPlayer, SLOT(mapScaleChanged(int)));
        QObject::connect(oss, SIGNAL(pressed()), SongPlayer, SLOT(setOss()));
        QObject::connect(pushButton47, SIGNAL(clicked()), SongPlayer, SLOT(saveMap()));
        QObject::connect(pushButton48, SIGNAL(clicked()), SongPlayer, SLOT(loadMap()));
        QObject::connect(pushButton49, SIGNAL(clicked()), SongPlayer, SLOT(nudgeMinus()));
        QObject::connect(pushButton50, SIGNAL(pressed()), SongPlayer, SLOT(mapStart()));
        QObject::connect(pushButton50_2, SIGNAL(clicked()), SongPlayer, SLOT(nudgePlus()));
        QObject::connect(pushButton51, SIGNAL(clicked()), SongPlayer, SLOT(mediumSwitch()));
        QObject::connect(pushButton77, SIGNAL(pressed()), SongPlayer, SLOT(mapStop()));
        QObject::connect(pushButton90, SIGNAL(clicked()), SongPlayer, SLOT(restartCore()));
        QObject::connect(startStopButton, SIGNAL(pressed()), SongPlayer, SLOT(start_stop()));
        QObject::connect(switcherButton, SIGNAL(clicked()), SongPlayer, SLOT(mediumSwitch()));
        QObject::connect(tempoSlider, SIGNAL(valueChanged(int)), SongPlayer, SLOT(tempoChanged()));
        QObject::connect(jack, SIGNAL(clicked()), SongPlayer, SLOT(setJack()));
        QObject::connect(beatGraphAnalyzer, SIGNAL(normalTempo()), SongPlayer, SLOT(normalTempo()));
        QObject::connect(beatGraphAnalyzer, SIGNAL(targetTempo()), SongPlayer, SLOT(targetTempo()));
        QObject::connect(jackcombo, SIGNAL(activated(QString)), jack_dev, SLOT(setText(QString)));
        QObject::connect(jackloutcombo, SIGNAL(activated(QString)), jack_lout, SLOT(setText(QString)));
        QObject::connect(jackroutcombo, SIGNAL(activated(QString)), jack_rout, SLOT(setText(QString)));

        tab->setCurrentIndex(0);
        mapping_scale->setCurrentIndex(2);
        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SongPlayer);
    } // setupUi

    void retranslateUi(QDialog *SongPlayer)
    {
        SongPlayer->setWindowTitle(QApplication::translate("SongPlayer", "BpmPlayer", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SongPlayer", "Time", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        CurrentTimeLCD->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        label_2->setText(QApplication::translate("SongPlayer", "Total", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        TotalTimeLCD->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        label_3->setText(QApplication::translate("SongPlayer", "Tempo Now", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("SongPlayer", "Normal Tempo", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("SongPlayer", "Playing Speed", 0, QApplication::UnicodeUTF8));
        groupBox_5->setTitle(QApplication::translate("SongPlayer", "Nudging", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        KeyMinus->setToolTip(QApplication::translate("SongPlayer", "Key: '-'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        KeyMinus->setText(QApplication::translate("SongPlayer", "-", 0, QApplication::UnicodeUTF8));
        KeyMinus->setShortcut(QApplication::translate("SongPlayer", "-", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        ButtonMinusHalfB->setToolTip(QApplication::translate("SongPlayer", "Key: '*'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        ButtonMinusHalfB->setText(QApplication::translate("SongPlayer", "-1/2B", 0, QApplication::UnicodeUTF8));
        ButtonMinusHalfB->setShortcut(QApplication::translate("SongPlayer", "*", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        ButtonMinus1M->setToolTip(QApplication::translate("SongPlayer", "Key: '1'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        ButtonMinus1M->setText(QApplication::translate("SongPlayer", "-1M", 0, QApplication::UnicodeUTF8));
        ButtonMinus1M->setShortcut(QApplication::translate("SongPlayer", "1", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        ButtonMinus4M->setToolTip(QApplication::translate("SongPlayer", "Key: '4'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        ButtonMinus4M->setText(QApplication::translate("SongPlayer", "-4M", 0, QApplication::UnicodeUTF8));
        ButtonMinus4M->setShortcut(QApplication::translate("SongPlayer", "4", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        ButtonMinus8M->setToolTip(QApplication::translate("SongPlayer", "Key: '6'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        ButtonMinus8M->setText(QApplication::translate("SongPlayer", "-8M", 0, QApplication::UnicodeUTF8));
        ButtonMinus8M->setShortcut(QApplication::translate("SongPlayer", "7", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        KeyPlus->setToolTip(QApplication::translate("SongPlayer", "Key: '+' ", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        KeyPlus->setText(QApplication::translate("SongPlayer", "+", 0, QApplication::UnicodeUTF8));
        KeyPlus->setShortcut(QApplication::translate("SongPlayer", "+", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton19->setToolTip(QApplication::translate("SongPlayer", "Key: '9'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton19->setText(QApplication::translate("SongPlayer", "+1B", 0, QApplication::UnicodeUTF8));
        PushButton19->setShortcut(QApplication::translate("SongPlayer", "9", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        ButtonPlus1M->setToolTip(QApplication::translate("SongPlayer", "Key: '2'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        ButtonPlus1M->setText(QApplication::translate("SongPlayer", "+1M", 0, QApplication::UnicodeUTF8));
        ButtonPlus1M->setShortcut(QApplication::translate("SongPlayer", "2", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        ButtonPlus4M->setToolTip(QApplication::translate("SongPlayer", "Key: '5'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        ButtonPlus4M->setText(QApplication::translate("SongPlayer", "+4M", 0, QApplication::UnicodeUTF8));
        ButtonPlus4M->setShortcut(QApplication::translate("SongPlayer", "5", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        ButtonPlus8M->setToolTip(QApplication::translate("SongPlayer", "Key: '8'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        ButtonPlus8M->setText(QApplication::translate("SongPlayer", "+8M", 0, QApplication::UnicodeUTF8));
        ButtonPlus8M->setShortcut(QApplication::translate("SongPlayer", "8", 0, QApplication::UnicodeUTF8));
        groupBox_6->setTitle(QApplication::translate("SongPlayer", "Cues", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton11->setToolTip(QApplication::translate("SongPlayer", "Key 'z'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton11->setText(QApplication::translate("SongPlayer", "Jump to Z", 0, QApplication::UnicodeUTF8));
        PushButton11->setShortcut(QApplication::translate("SongPlayer", "Z", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton12->setToolTip(QApplication::translate("SongPlayer", "Key: 'X'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton12->setText(QApplication::translate("SongPlayer", "Jump to X", 0, QApplication::UnicodeUTF8));
        PushButton12->setShortcut(QApplication::translate("SongPlayer", "X", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton13->setToolTip(QApplication::translate("SongPlayer", "Key: 'C'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton13->setText(QApplication::translate("SongPlayer", "Jump to C", 0, QApplication::UnicodeUTF8));
        PushButton13->setShortcut(QApplication::translate("SongPlayer", "C", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton14->setToolTip(QApplication::translate("SongPlayer", "Key: 'V'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton14->setText(QApplication::translate("SongPlayer", "Jump to V", 0, QApplication::UnicodeUTF8));
        PushButton14->setShortcut(QApplication::translate("SongPlayer", "V", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton15->setToolTip(QApplication::translate("SongPlayer", "Key: 'ALT+Z'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton15->setText(QApplication::translate("SongPlayer", "Store Z", 0, QApplication::UnicodeUTF8));
        PushButton15->setShortcut(QApplication::translate("SongPlayer", "Alt+Z", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton16->setToolTip(QApplication::translate("SongPlayer", "Key: 'ALT-X'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton16->setText(QApplication::translate("SongPlayer", "Store X", 0, QApplication::UnicodeUTF8));
        PushButton16->setShortcut(QApplication::translate("SongPlayer", "Alt+X", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton17->setToolTip(QApplication::translate("SongPlayer", "Key: 'ALT-C'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton17->setText(QApplication::translate("SongPlayer", "Store C", 0, QApplication::UnicodeUTF8));
        PushButton17->setShortcut(QApplication::translate("SongPlayer", "Alt+C", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton18->setToolTip(QApplication::translate("SongPlayer", "Key: 'ALT-V'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton18->setText(QApplication::translate("SongPlayer", "Store V", 0, QApplication::UnicodeUTF8));
        PushButton18->setShortcut(QApplication::translate("SongPlayer", "Alt+V", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton34->setToolTip(QApplication::translate("SongPlayer", "Key: '('", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton34->setText(QApplication::translate("SongPlayer", "- 1/32", 0, QApplication::UnicodeUTF8));
        PushButton34->setShortcut(QApplication::translate("SongPlayer", "(", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton33->setToolTip(QApplication::translate("SongPlayer", "Key: ')'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton33->setText(QApplication::translate("SongPlayer", "+1/32", 0, QApplication::UnicodeUTF8));
        PushButton33->setShortcut(QApplication::translate("SongPlayer", ")", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton35->setToolTip(QApplication::translate("SongPlayer", "Key: '{'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton35->setText(QApplication::translate("SongPlayer", "-8M", 0, QApplication::UnicodeUTF8));
        PushButton35->setShortcut(QApplication::translate("SongPlayer", "{", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton36->setToolTip(QApplication::translate("SongPlayer", "Key: '}'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton36->setText(QApplication::translate("SongPlayer", "+8M", 0, QApplication::UnicodeUTF8));
        PushButton36->setShortcut(QApplication::translate("SongPlayer", "}", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton10->setToolTip(QApplication::translate("SongPlayer", "Key: '/'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton10->setText(QApplication::translate("SongPlayer", "Set Cue", 0, QApplication::UnicodeUTF8));
        PushButton10->setShortcut(QApplication::translate("SongPlayer", "/", 0, QApplication::UnicodeUTF8));
        groupBox_7->setTitle(QApplication::translate("SongPlayer", "LFO", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton26->setToolTip(QApplication::translate("SongPlayer", "Key: 'P': pan", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton26->setText(QString());
        PushButton26->setShortcut(QApplication::translate("SongPlayer", "P", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton28->setToolTip(QApplication::translate("SongPlayer", "Key:'S': saw", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton28->setText(QString());
        PushButton28->setShortcut(QApplication::translate("SongPlayer", "S", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton30->setToolTip(QApplication::translate("SongPlayer", "Key: 'R': reverse saw", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton30->setText(QString());
        PushButton30->setShortcut(QApplication::translate("SongPlayer", "R", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton42->setToolTip(QApplication::translate("SongPlayer", "Key: 'B': break", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton42->setText(QString());
        PushButton42->setShortcut(QApplication::translate("SongPlayer", "B", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton27->setToolTip(QApplication::translate("SongPlayer", "Key: 'ALT-P': pan", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton27->setText(QString());
        PushButton27->setShortcut(QApplication::translate("SongPlayer", "Alt+P", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton29->setToolTip(QApplication::translate("SongPlayer", "Key: 'ALT-S': saw", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton29->setText(QString());
        PushButton29->setShortcut(QApplication::translate("SongPlayer", "Alt+S", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton31->setToolTip(QApplication::translate("SongPlayer", "Key: 'ALT-R': reverse saw", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton31->setText(QString());
        PushButton31->setShortcut(QApplication::translate("SongPlayer", "Alt+R", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton39->setToolTip(QApplication::translate("SongPlayer", "Key: 'N', no LFO", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton39->setText(QString());
        PushButton39->setShortcut(QApplication::translate("SongPlayer", "N", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton41->setToolTip(QApplication::translate("SongPlayer", "Key: 'm'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton41->setText(QApplication::translate("SongPlayer", "Metronome", 0, QApplication::UnicodeUTF8));
        PushButton41->setShortcut(QApplication::translate("SongPlayer", "M", 0, QApplication::UnicodeUTF8));
        PushButton21->setText(QApplication::translate("SongPlayer", "Normal", 0, QApplication::UnicodeUTF8));
        PushButton21->setShortcut(QApplication::translate("SongPlayer", ".", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        switcherButton->setToolTip(QApplication::translate("SongPlayer", "Key: '0'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        switcherButton->setText(QApplication::translate("SongPlayer", "Fade", 0, QApplication::UnicodeUTF8));
        switcherButton->setShortcut(QApplication::translate("SongPlayer", "0", 0, QApplication::UnicodeUTF8));
        PushButton20->setText(QApplication::translate("SongPlayer", "Target", 0, QApplication::UnicodeUTF8));
        PushButton20->setShortcut(QString());
#ifndef QT_NO_TOOLTIP
        PushButton24->setToolTip(QApplication::translate("SongPlayer", "Key: ']'", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton24->setText(QApplication::translate("SongPlayer", "Restart", 0, QApplication::UnicodeUTF8));
        PushButton24->setShortcut(QApplication::translate("SongPlayer", "]", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        startStopButton->setToolTip(QApplication::translate("SongPlayer", "Key: ' '", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        startStopButton->setText(QApplication::translate("SongPlayer", "Stop", 0, QApplication::UnicodeUTF8));
        startStopButton->setShortcut(QString());
        AboutButton->setText(QApplication::translate("SongPlayer", "About", 0, QApplication::UnicodeUTF8));
        tab->setTabText(tab->indexOf(playerTab), QApplication::translate("SongPlayer", "Pla&yer", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        mapin->setToolTip(QApplication::translate("SongPlayer", "This is the input map. Every color represents a segment of the audio stream. The length of one segment is determined by the notelength.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        mapout->setToolTip(QApplication::translate("SongPlayer", "The is the ouput sequency. The colors are the same as in the input track above. The order can be modified by selecting sgements in the input track and then placing them here.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButton49->setText(QApplication::translate("SongPlayer", "-", 0, QApplication::UnicodeUTF8));
        pushButton49->setShortcut(QApplication::translate("SongPlayer", "-", 0, QApplication::UnicodeUTF8));
        pushButton50_2->setText(QApplication::translate("SongPlayer", "+", 0, QApplication::UnicodeUTF8));
        pushButton50_2->setShortcut(QApplication::translate("SongPlayer", "+", 0, QApplication::UnicodeUTF8));
        pushButton51->setText(QApplication::translate("SongPlayer", "Fade", 0, QApplication::UnicodeUTF8));
        pushButton51->setShortcut(QApplication::translate("SongPlayer", "0", 0, QApplication::UnicodeUTF8));
        pushButton50->setText(QApplication::translate("SongPlayer", "&Go seq", 0, QApplication::UnicodeUTF8));
        pushButton50->setShortcut(QApplication::translate("SongPlayer", "G", 0, QApplication::UnicodeUTF8));
        pushButton77->setText(QApplication::translate("SongPlayer", "Stop seq", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        mapout_length->setToolTip(QApplication::translate("SongPlayer", "The number of output segments in the output track", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        mapping_scale->clear();
        mapping_scale->insertItems(0, QStringList()
         << QApplication::translate("SongPlayer", "1/64th", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "1/32th", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "1/16th", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "1/8th", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "1/4th", 0, QApplication::UnicodeUTF8)
        );
        groupBox->setTitle(QString());
        atend_restart->setText(QApplication::translate("SongPlayer", "restart", 0, QApplication::UnicodeUTF8));
        atend_continue->setText(QApplication::translate("SongPlayer", "continue", 0, QApplication::UnicodeUTF8));
        atend_stop->setText(QApplication::translate("SongPlayer", "stop", 0, QApplication::UnicodeUTF8));
        atend_loop->setText(QApplication::translate("SongPlayer", "loop", 0, QApplication::UnicodeUTF8));
        atend_loop->setShortcut(QApplication::translate("SongPlayer", "Space", 0, QApplication::UnicodeUTF8));
        atend_c->setText(QApplication::translate("SongPlayer", "C", 0, QApplication::UnicodeUTF8));
        atend_x->setText(QApplication::translate("SongPlayer", "X", 0, QApplication::UnicodeUTF8));
        atend_v->setText(QApplication::translate("SongPlayer", "V", 0, QApplication::UnicodeUTF8));
        atend_z->setText(QApplication::translate("SongPlayer", "Z", 0, QApplication::UnicodeUTF8));
        pushButton48->setText(QApplication::translate("SongPlayer", "&Load", 0, QApplication::UnicodeUTF8));
        pushButton47->setText(QApplication::translate("SongPlayer", "Save", 0, QApplication::UnicodeUTF8));
        pushButton47->setShortcut(QString());
        tab->setTabText(tab->indexOf(scramblerTab), QApplication::translate("SongPlayer", "Scra&mbler", 0, QApplication::UnicodeUTF8));
        tab->setTabText(tab->indexOf(beatgraphTab), QApplication::translate("SongPlayer", "Be&atgraph", 0, QApplication::UnicodeUTF8));
        tab->setTabText(tab->indexOf(bpmTab), QApplication::translate("SongPlayer", "&Bpm", 0, QApplication::UnicodeUTF8));
        tab->setTabText(tab->indexOf(spectrumTab), QApplication::translate("SongPlayer", "Spectrum", 0, QApplication::UnicodeUTF8));
        tab->setTabText(tab->indexOf(rhythmTab), QApplication::translate("SongPlayer", "Rhytm", 0, QApplication::UnicodeUTF8));
        tab->setTabText(tab->indexOf(songTab), QApplication::translate("SongPlayer", "Song &Info", 0, QApplication::UnicodeUTF8));
        pushButton90->setText(QApplication::translate("SongPlayer", "Apply", 0, QApplication::UnicodeUTF8));
        rms_toggle->setText(QApplication::translate("SongPlayer", "normalize RMS power", 0, QApplication::UnicodeUTF8));
        textLabel2_6->setText(QApplication::translate("SongPlayer", "disable use of the following decoder programs on this configuration", 0, QApplication::UnicodeUTF8));
        rms->setText(QApplication::translate("SongPlayer", "0.12", 0, QApplication::UnicodeUTF8));
        textLabel2->setText(QApplication::translate("SongPlayer", "Target RMS power", 0, QApplication::UnicodeUTF8));
        textLabel1_3->setText(QApplication::translate("SongPlayer", "Directory to store .raw files", 0, QApplication::UnicodeUTF8));
        raw_directory->setText(QApplication::translate("SongPlayer", "./", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(decodingtab), QApplication::translate("SongPlayer", "Decoding", 0, QApplication::UnicodeUTF8));
        textLabel9->setText(QApplication::translate("SongPlayer", "ALSA device to use", 0, QApplication::UnicodeUTF8));
        alsa_verbose->setText(QApplication::translate("SongPlayer", "verbose", 0, QApplication::UnicodeUTF8));
        textLabel3->setText(QApplication::translate("SongPlayer", "Latency (ms)", 0, QApplication::UnicodeUTF8));
        alsa->setText(QApplication::translate("SongPlayer", "use ALSA driver", 0, QApplication::UnicodeUTF8));
        alsa_dev->setText(QApplication::translate("SongPlayer", "hw:0", 0, QApplication::UnicodeUTF8));
        comboBox2->clear();
        comboBox2->insertItems(0, QStringList()
         << QApplication::translate("SongPlayer", "hw:0,0", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "hw:1,0", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "plughw:0", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "plughw:1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "default", 0, QApplication::UnicodeUTF8)
        );
        tabWidget->setTabText(tabWidget->indexOf(alsatab), QApplication::translate("SongPlayer", "Alsa", 0, QApplication::UnicodeUTF8));
        textLabel7->setText(QApplication::translate("SongPlayer", "Fragment count", 0, QApplication::UnicodeUTF8));
        oss_verbose->setText(QApplication::translate("SongPlayer", "verbose", 0, QApplication::UnicodeUTF8));
        textLabel6->setText(QApplication::translate("SongPlayer", "DSP device to use", 0, QApplication::UnicodeUTF8));
        oss_init_fragments->setText(QApplication::translate("SongPlayer", "Set buffer properties", 0, QApplication::UnicodeUTF8));
        oss->setText(QApplication::translate("SongPlayer", "use OSS sound driver", 0, QApplication::UnicodeUTF8));
        textLabel3_3->setText(QApplication::translate("SongPlayer", "Latency (ms)", 0, QApplication::UnicodeUTF8));
        oss_dsp->setText(QApplication::translate("SongPlayer", "/dev/dsp", 0, QApplication::UnicodeUTF8));
        oss_nolatencyaccounting->setText(QApplication::translate("SongPlayer", "No latency accounting", 0, QApplication::UnicodeUTF8));
        comboBox3->clear();
        comboBox3->insertItems(0, QStringList()
         << QApplication::translate("SongPlayer", "/dev/dsp", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "/dev/dsp1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "/dev/dsp2", 0, QApplication::UnicodeUTF8)
        );
        tabWidget->setTabText(tabWidget->indexOf(osstab), QApplication::translate("SongPlayer", "Oss", 0, QApplication::UnicodeUTF8));
        jack->setText(QApplication::translate("SongPlayer", "use Jack device", 0, QApplication::UnicodeUTF8));
        outputchannel_label->setText(QApplication::translate("SongPlayer", "Outputchannel", 0, QApplication::UnicodeUTF8));
        jack_dev->setText(QString());
        jackcombo->clear();
        jackcombo->insertItems(0, QStringList()
         << QApplication::translate("SongPlayer", "bpmplay0", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "bpmplay1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "bpmplay2", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "bpmplay3", 0, QApplication::UnicodeUTF8)
        );
        connectchannell_label->setText(QApplication::translate("SongPlayer", "Left channel connection", 0, QApplication::UnicodeUTF8));
        jack_lout->setText(QString());
        jackloutcombo->clear();
        jackloutcombo->insertItems(0, QStringList()
         << QApplication::translate("SongPlayer", "system:playback_1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "system:playback_2", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "system:playback_3", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "system:playback_4", 0, QApplication::UnicodeUTF8)
        );
        connectchannelr_label->setText(QApplication::translate("SongPlayer", "Right channel connection", 0, QApplication::UnicodeUTF8));
        jack_rout->setText(QString());
        jackroutcombo->clear();
        jackroutcombo->insertItems(0, QStringList()
         << QApplication::translate("SongPlayer", "system:playback_1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "system:playback_2", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "system:playback_3", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SongPlayer", "system:playback_4", 0, QApplication::UnicodeUTF8)
        );
        jack_verbose->setText(QApplication::translate("SongPlayer", "Verbose", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(jacktab), QApplication::translate("SongPlayer", "Jack", 0, QApplication::UnicodeUTF8));
        tab->setTabText(tab->indexOf(setupTab), QApplication::translate("SongPlayer", "Setup", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SongPlayer: public Ui_SongPlayer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_PLAYER_H
