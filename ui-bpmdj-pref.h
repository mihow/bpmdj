/********************************************************************************
** Form generated from reading UI file 'ui-bpmdj-pref.ui'
**
** Created: Thu May 12 17:50:02 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_BPMDJ_2D_PREF_H
#define UI_2D_BPMDJ_2D_PREF_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "metric-widget.h"

QT_BEGIN_NAMESPACE

class Ui_BpmDjPreferences
{
public:
    QGridLayout *gridLayout;
    QTabWidget *subtabs;
    QWidget *playerTab;
    QGridLayout *gridLayout1;
    QLabel *TextLabel3;
    QFrame *playerStack;
    QVBoxLayout *vboxLayout;
    QLabel *label_2;
    QVBoxLayout *vboxLayout1;
    QSpacerItem *spacerItem;
    QPushButton *playerInstallButton;
    QWidget *analyzerTab;
    QGridLayout *gridLayout2;
    QLabel *TextLabel3_2;
    QFrame *analyzerStack;
    QVBoxLayout *vboxLayout2;
    QVBoxLayout *vboxLayout3;
    QPushButton *analyzerInstallButton;
    QSpacerItem *spacerItem1;
    QWidget *tab_4;
    QGridLayout *gridLayout_2;
    QLabel *textLabel1_2;
    QPushButton *colorTempo44;
    QCheckBox *showTempo54;
    QPushButton *colorTempo54;
    QCheckBox *showTempo64;
    QPushButton *colorTempo64;
    QCheckBox *showTempo74;
    QPushButton *colorTempo74;
    QCheckBox *showTempo84;
    QPushButton *colorTempo84;
    QLabel *textLabel2_3;
    QSpinBox *tempoSpin;
    QCheckBox *showUnknownTempo;
    QGroupBox *bpmalg;
    QGridLayout *gridLayout3;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QRadioButton *resamplingScan;
    QRadioButton *enveloppeSpectrum;
    QRadioButton *experimentalAlg;
    QVBoxLayout *verticalLayout_2;
    QRadioButton *ultraLongFFT;
    QRadioButton *fullAutoCorrelation;
    QRadioButton *weightedEnvCor;
    QFrame *line3;
    QLabel *TextLabel1_3;
    QLineEdit *From;
    QLabel *TextLabel2_3;
    QLineEdit *To;
    QSpacerItem *spacerItem2;
    QWidget *tab_5;
    QGridLayout *gridLayout4;
    QLabel *label;
    QLabel *textLabel1_5_3;
    QLabel *textLabel1_5_3_2;
    QSpinBox *authorDecay;
    QPushButton *colorPlayedAuthor;
    QLabel *textLabel1_5_2;
    QPushButton *colorUnavailableSong;
    QPushButton *colorUncheckedSong;
    QPushButton *colorPlayedSong;
    QPushButton *colorAlltimePlaycount;
    QLabel *textLabel1;
    QLabel *textLabel1_5_4;
    QLabel *textLabel1_5;
    QLabel *textLabel2_4;
    QLabel *TextLabel8_2_4;
    QLabel *TextLabel8_2;
    QLabel *TextLabel8_2_2;
    QPushButton *greenTimeColor;
    QLabel *TextLabel8;
    QLineEdit *yellowTime;
    QPushButton *redTimeColor;
    QCheckBox *color_main_window;
    QLabel *TextLabel85;
    QLineEdit *redTime;
    QLabel *TextLabel84;
    QLabel *TextLabel83;
    QPushButton *orangeTimeColor;
    QLineEdit *orangeTime;
    QPushButton *yellowTimeColor;
    QPushButton *colordColorCol;
    QSpacerItem *spacerItem3;
    QWidget *tab_6;
    QGridLayout *gridLayout5;
    QSpinBox *max_songs;
    MetricWidget *dColorMetric;
    QLabel *textLabel2_2;
    QWidget *tab_7;
    QGridLayout *gridLayout6;
    QSpinBox *clusterDepth;
    QSpacerItem *spacerItem4;
    QLabel *textLabel1_6;
    QLabel *textLabel2_5;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *BpmDjPreferences)
    {
        if (BpmDjPreferences->objectName().isEmpty())
            BpmDjPreferences->setObjectName(QString::fromUtf8("BpmDjPreferences"));
        BpmDjPreferences->resize(506, 535);
        gridLayout = new QGridLayout(BpmDjPreferences);
#ifndef Q_OS_MAC
        gridLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout->setContentsMargins(9, 9, 9, 9);
#endif
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        subtabs = new QTabWidget(BpmDjPreferences);
        subtabs->setObjectName(QString::fromUtf8("subtabs"));
        playerTab = new QWidget();
        playerTab->setObjectName(QString::fromUtf8("playerTab"));
        gridLayout1 = new QGridLayout(playerTab);
#ifndef Q_OS_MAC
        gridLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout1->setContentsMargins(9, 9, 9, 9);
#endif
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        TextLabel3 = new QLabel(playerTab);
        TextLabel3->setObjectName(QString::fromUtf8("TextLabel3"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TextLabel3->sizePolicy().hasHeightForWidth());
        TextLabel3->setSizePolicy(sizePolicy);
        TextLabel3->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        TextLabel3->setWordWrap(true);

        gridLayout1->addWidget(TextLabel3, 0, 0, 1, 1);

        playerStack = new QFrame(playerTab);
        playerStack->setObjectName(QString::fromUtf8("playerStack"));
        playerStack->setFrameShape(QFrame::StyledPanel);
        playerStack->setFrameShadow(QFrame::Raised);
        vboxLayout = new QVBoxLayout(playerStack);
        vboxLayout->setSpacing(2);
        vboxLayout->setContentsMargins(2, 2, 2, 2);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        label_2 = new QLabel(playerStack);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);
        label_2->setWordWrap(true);

        vboxLayout->addWidget(label_2);

        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setSpacing(2);
        vboxLayout1->setContentsMargins(0, 0, 0, 0);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));

        vboxLayout->addLayout(vboxLayout1);


        gridLayout1->addWidget(playerStack, 1, 0, 1, 2);

        spacerItem = new QSpacerItem(437, 27, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout1->addItem(spacerItem, 2, 0, 1, 1);

        playerInstallButton = new QPushButton(playerTab);
        playerInstallButton->setObjectName(QString::fromUtf8("playerInstallButton"));
        playerInstallButton->setEnabled(true);

        gridLayout1->addWidget(playerInstallButton, 2, 1, 1, 1);

        subtabs->addTab(playerTab, QString());
        analyzerTab = new QWidget();
        analyzerTab->setObjectName(QString::fromUtf8("analyzerTab"));
        gridLayout2 = new QGridLayout(analyzerTab);
#ifndef Q_OS_MAC
        gridLayout2->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout2->setContentsMargins(9, 9, 9, 9);
#endif
        gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
        TextLabel3_2 = new QLabel(analyzerTab);
        TextLabel3_2->setObjectName(QString::fromUtf8("TextLabel3_2"));
        sizePolicy.setHeightForWidth(TextLabel3_2->sizePolicy().hasHeightForWidth());
        TextLabel3_2->setSizePolicy(sizePolicy);
        TextLabel3_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        TextLabel3_2->setWordWrap(true);

        gridLayout2->addWidget(TextLabel3_2, 0, 0, 1, 2);

        analyzerStack = new QFrame(analyzerTab);
        analyzerStack->setObjectName(QString::fromUtf8("analyzerStack"));
        analyzerStack->setFrameShape(QFrame::StyledPanel);
        analyzerStack->setFrameShadow(QFrame::Raised);
        vboxLayout2 = new QVBoxLayout(analyzerStack);
        vboxLayout2->setSpacing(2);
        vboxLayout2->setContentsMargins(2, 2, 2, 2);
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        vboxLayout3 = new QVBoxLayout();
        vboxLayout3->setSpacing(2);
#ifndef Q_OS_MAC
        vboxLayout3->setContentsMargins(0, 0, 0, 0);
#endif
        vboxLayout3->setObjectName(QString::fromUtf8("vboxLayout3"));

        vboxLayout2->addLayout(vboxLayout3);


        gridLayout2->addWidget(analyzerStack, 1, 0, 1, 2);

        analyzerInstallButton = new QPushButton(analyzerTab);
        analyzerInstallButton->setObjectName(QString::fromUtf8("analyzerInstallButton"));

        gridLayout2->addWidget(analyzerInstallButton, 2, 1, 1, 1);

        spacerItem1 = new QSpacerItem(261, 27, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout2->addItem(spacerItem1, 2, 0, 1, 1);

        subtabs->addTab(analyzerTab, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        gridLayout_2 = new QGridLayout(tab_4);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        textLabel1_2 = new QLabel(tab_4);
        textLabel1_2->setObjectName(QString::fromUtf8("textLabel1_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(textLabel1_2->sizePolicy().hasHeightForWidth());
        textLabel1_2->setSizePolicy(sizePolicy1);
        textLabel1_2->setWordWrap(false);

        gridLayout_2->addWidget(textLabel1_2, 0, 0, 1, 2);

        colorTempo44 = new QPushButton(tab_4);
        colorTempo44->setObjectName(QString::fromUtf8("colorTempo44"));
        colorTempo44->setAutoFillBackground(true);
        colorTempo44->setFlat(true);

        gridLayout_2->addWidget(colorTempo44, 0, 2, 1, 1);

        showTempo54 = new QCheckBox(tab_4);
        showTempo54->setObjectName(QString::fromUtf8("showTempo54"));

        gridLayout_2->addWidget(showTempo54, 1, 0, 1, 2);

        colorTempo54 = new QPushButton(tab_4);
        colorTempo54->setObjectName(QString::fromUtf8("colorTempo54"));
        colorTempo54->setAutoFillBackground(true);
        colorTempo54->setFlat(true);

        gridLayout_2->addWidget(colorTempo54, 1, 2, 1, 1);

        showTempo64 = new QCheckBox(tab_4);
        showTempo64->setObjectName(QString::fromUtf8("showTempo64"));

        gridLayout_2->addWidget(showTempo64, 2, 0, 1, 2);

        colorTempo64 = new QPushButton(tab_4);
        colorTempo64->setObjectName(QString::fromUtf8("colorTempo64"));
        colorTempo64->setAutoFillBackground(true);
        colorTempo64->setFlat(true);

        gridLayout_2->addWidget(colorTempo64, 2, 2, 1, 1);

        showTempo74 = new QCheckBox(tab_4);
        showTempo74->setObjectName(QString::fromUtf8("showTempo74"));

        gridLayout_2->addWidget(showTempo74, 3, 0, 1, 2);

        colorTempo74 = new QPushButton(tab_4);
        colorTempo74->setObjectName(QString::fromUtf8("colorTempo74"));
        colorTempo74->setAutoFillBackground(true);
        colorTempo74->setFlat(true);

        gridLayout_2->addWidget(colorTempo74, 3, 2, 1, 1);

        showTempo84 = new QCheckBox(tab_4);
        showTempo84->setObjectName(QString::fromUtf8("showTempo84"));

        gridLayout_2->addWidget(showTempo84, 4, 0, 1, 2);

        colorTempo84 = new QPushButton(tab_4);
        colorTempo84->setObjectName(QString::fromUtf8("colorTempo84"));
        colorTempo84->setAutoFillBackground(true);
        colorTempo84->setFlat(true);

        gridLayout_2->addWidget(colorTempo84, 4, 2, 1, 1);

        textLabel2_3 = new QLabel(tab_4);
        textLabel2_3->setObjectName(QString::fromUtf8("textLabel2_3"));
        textLabel2_3->setWordWrap(false);

        gridLayout_2->addWidget(textLabel2_3, 5, 0, 1, 1);

        tempoSpin = new QSpinBox(tab_4);
        tempoSpin->setObjectName(QString::fromUtf8("tempoSpin"));
        tempoSpin->setMaximum(100);
        tempoSpin->setValue(6);

        gridLayout_2->addWidget(tempoSpin, 5, 2, 1, 1);

        showUnknownTempo = new QCheckBox(tab_4);
        showUnknownTempo->setObjectName(QString::fromUtf8("showUnknownTempo"));
        showUnknownTempo->setChecked(true);

        gridLayout_2->addWidget(showUnknownTempo, 6, 0, 1, 2);

        bpmalg = new QGroupBox(tab_4);
        bpmalg->setObjectName(QString::fromUtf8("bpmalg"));
        gridLayout3 = new QGridLayout(bpmalg);
        gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        resamplingScan = new QRadioButton(bpmalg);
        resamplingScan->setObjectName(QString::fromUtf8("resamplingScan"));
        resamplingScan->setChecked(true);

        verticalLayout->addWidget(resamplingScan);

        enveloppeSpectrum = new QRadioButton(bpmalg);
        enveloppeSpectrum->setObjectName(QString::fromUtf8("enveloppeSpectrum"));

        verticalLayout->addWidget(enveloppeSpectrum);

        experimentalAlg = new QRadioButton(bpmalg);
        experimentalAlg->setObjectName(QString::fromUtf8("experimentalAlg"));

        verticalLayout->addWidget(experimentalAlg);


        horizontalLayout->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        ultraLongFFT = new QRadioButton(bpmalg);
        ultraLongFFT->setObjectName(QString::fromUtf8("ultraLongFFT"));
        ultraLongFFT->setChecked(false);

        verticalLayout_2->addWidget(ultraLongFFT);

        fullAutoCorrelation = new QRadioButton(bpmalg);
        fullAutoCorrelation->setObjectName(QString::fromUtf8("fullAutoCorrelation"));

        verticalLayout_2->addWidget(fullAutoCorrelation);

        weightedEnvCor = new QRadioButton(bpmalg);
        weightedEnvCor->setObjectName(QString::fromUtf8("weightedEnvCor"));

        verticalLayout_2->addWidget(weightedEnvCor);


        horizontalLayout->addLayout(verticalLayout_2);


        gridLayout3->addLayout(horizontalLayout, 0, 0, 1, 4);

        line3 = new QFrame(bpmalg);
        line3->setObjectName(QString::fromUtf8("line3"));
        line3->setFrameShape(QFrame::HLine);
        line3->setFrameShadow(QFrame::Sunken);
        line3->setFrameShape(QFrame::HLine);

        gridLayout3->addWidget(line3, 1, 0, 1, 4);

        TextLabel1_3 = new QLabel(bpmalg);
        TextLabel1_3->setObjectName(QString::fromUtf8("TextLabel1_3"));
        TextLabel1_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        TextLabel1_3->setWordWrap(false);

        gridLayout3->addWidget(TextLabel1_3, 2, 0, 1, 1);

        From = new QLineEdit(bpmalg);
        From->setObjectName(QString::fromUtf8("From"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(From->sizePolicy().hasHeightForWidth());
        From->setSizePolicy(sizePolicy2);

        gridLayout3->addWidget(From, 2, 1, 1, 1);

        TextLabel2_3 = new QLabel(bpmalg);
        TextLabel2_3->setObjectName(QString::fromUtf8("TextLabel2_3"));
        TextLabel2_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        TextLabel2_3->setWordWrap(false);

        gridLayout3->addWidget(TextLabel2_3, 2, 2, 1, 1);

        To = new QLineEdit(bpmalg);
        To->setObjectName(QString::fromUtf8("To"));
        sizePolicy2.setHeightForWidth(To->sizePolicy().hasHeightForWidth());
        To->setSizePolicy(sizePolicy2);

        gridLayout3->addWidget(To, 2, 3, 1, 1);


        gridLayout_2->addWidget(bpmalg, 7, 0, 1, 3);

        spacerItem2 = new QSpacerItem(256, 108, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(spacerItem2, 8, 1, 1, 2);

        subtabs->addTab(tab_4, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QString::fromUtf8("tab_5"));
        gridLayout4 = new QGridLayout(tab_5);
#ifndef Q_OS_MAC
        gridLayout4->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout4->setContentsMargins(9, 9, 9, 9);
#endif
        gridLayout4->setObjectName(QString::fromUtf8("gridLayout4"));
        label = new QLabel(tab_5);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout4->addWidget(label, 7, 0, 1, 4);

        textLabel1_5_3 = new QLabel(tab_5);
        textLabel1_5_3->setObjectName(QString::fromUtf8("textLabel1_5_3"));
        textLabel1_5_3->setEnabled(true);
        textLabel1_5_3->setWordWrap(false);

        gridLayout4->addWidget(textLabel1_5_3, 3, 0, 1, 3);

        textLabel1_5_3_2 = new QLabel(tab_5);
        textLabel1_5_3_2->setObjectName(QString::fromUtf8("textLabel1_5_3_2"));
        textLabel1_5_3_2->setEnabled(false);
        textLabel1_5_3_2->setWordWrap(false);

        gridLayout4->addWidget(textLabel1_5_3_2, 4, 0, 1, 3);

        authorDecay = new QSpinBox(tab_5);
        authorDecay->setObjectName(QString::fromUtf8("authorDecay"));
        authorDecay->setMinimum(1);
        authorDecay->setMaximum(100);
        authorDecay->setValue(6);

        gridLayout4->addWidget(authorDecay, 5, 5, 1, 2);

        colorPlayedAuthor = new QPushButton(tab_5);
        colorPlayedAuthor->setObjectName(QString::fromUtf8("colorPlayedAuthor"));
        colorPlayedAuthor->setEnabled(true);
        colorPlayedAuthor->setAutoFillBackground(true);
        colorPlayedAuthor->setFlat(true);

        gridLayout4->addWidget(colorPlayedAuthor, 1, 3, 1, 4);

        textLabel1_5_2 = new QLabel(tab_5);
        textLabel1_5_2->setObjectName(QString::fromUtf8("textLabel1_5_2"));
        textLabel1_5_2->setEnabled(true);
        textLabel1_5_2->setWordWrap(false);

        gridLayout4->addWidget(textLabel1_5_2, 0, 0, 1, 3);

        colorUnavailableSong = new QPushButton(tab_5);
        colorUnavailableSong->setObjectName(QString::fromUtf8("colorUnavailableSong"));
        colorUnavailableSong->setEnabled(true);
        colorUnavailableSong->setAutoFillBackground(true);
        colorUnavailableSong->setFlat(true);

        gridLayout4->addWidget(colorUnavailableSong, 3, 3, 1, 4);

        colorUncheckedSong = new QPushButton(tab_5);
        colorUncheckedSong->setObjectName(QString::fromUtf8("colorUncheckedSong"));
        colorUncheckedSong->setEnabled(false);
        colorUncheckedSong->setAutoFillBackground(true);
        colorUncheckedSong->setFlat(true);

        gridLayout4->addWidget(colorUncheckedSong, 4, 3, 1, 4);

        colorPlayedSong = new QPushButton(tab_5);
        colorPlayedSong->setObjectName(QString::fromUtf8("colorPlayedSong"));
        colorPlayedSong->setEnabled(true);
        colorPlayedSong->setAutoFillBackground(true);
        colorPlayedSong->setFlat(true);

        gridLayout4->addWidget(colorPlayedSong, 0, 3, 1, 4);

        colorAlltimePlaycount = new QPushButton(tab_5);
        colorAlltimePlaycount->setObjectName(QString::fromUtf8("colorAlltimePlaycount"));
        colorAlltimePlaycount->setEnabled(true);
        colorAlltimePlaycount->setAutoFillBackground(true);
        colorAlltimePlaycount->setFlat(true);

        gridLayout4->addWidget(colorAlltimePlaycount, 2, 3, 1, 4);

        textLabel1 = new QLabel(tab_5);
        textLabel1->setObjectName(QString::fromUtf8("textLabel1"));
        textLabel1->setWordWrap(false);

        gridLayout4->addWidget(textLabel1, 5, 0, 1, 5);

        textLabel1_5_4 = new QLabel(tab_5);
        textLabel1_5_4->setObjectName(QString::fromUtf8("textLabel1_5_4"));
        textLabel1_5_4->setEnabled(true);
        textLabel1_5_4->setWordWrap(false);

        gridLayout4->addWidget(textLabel1_5_4, 2, 0, 1, 3);

        textLabel1_5 = new QLabel(tab_5);
        textLabel1_5->setObjectName(QString::fromUtf8("textLabel1_5"));
        textLabel1_5->setEnabled(true);
        textLabel1_5->setWordWrap(false);

        gridLayout4->addWidget(textLabel1_5, 1, 0, 1, 3);

        textLabel2_4 = new QLabel(tab_5);
        textLabel2_4->setObjectName(QString::fromUtf8("textLabel2_4"));
        textLabel2_4->setEnabled(true);
        textLabel2_4->setWordWrap(false);

        gridLayout4->addWidget(textLabel2_4, 6, 0, 1, 2);

        TextLabel8_2_4 = new QLabel(tab_5);
        TextLabel8_2_4->setObjectName(QString::fromUtf8("TextLabel8_2_4"));
        TextLabel8_2_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        TextLabel8_2_4->setWordWrap(false);

        gridLayout4->addWidget(TextLabel8_2_4, 7, 4, 1, 2);

        TextLabel8_2 = new QLabel(tab_5);
        TextLabel8_2->setObjectName(QString::fromUtf8("TextLabel8_2"));
        TextLabel8_2->setWordWrap(false);

        gridLayout4->addWidget(TextLabel8_2, 8, 5, 1, 1);

        TextLabel8_2_2 = new QLabel(tab_5);
        TextLabel8_2_2->setObjectName(QString::fromUtf8("TextLabel8_2_2"));
        TextLabel8_2_2->setWordWrap(false);

        gridLayout4->addWidget(TextLabel8_2_2, 9, 5, 1, 1);

        greenTimeColor = new QPushButton(tab_5);
        greenTimeColor->setObjectName(QString::fromUtf8("greenTimeColor"));
        greenTimeColor->setAutoFillBackground(true);
        greenTimeColor->setFlat(true);

        gridLayout4->addWidget(greenTimeColor, 7, 6, 1, 1);

        TextLabel8 = new QLabel(tab_5);
        TextLabel8->setObjectName(QString::fromUtf8("TextLabel8"));
        TextLabel8->setWordWrap(false);

        gridLayout4->addWidget(TextLabel8, 8, 0, 1, 1);

        yellowTime = new QLineEdit(tab_5);
        yellowTime->setObjectName(QString::fromUtf8("yellowTime"));

        gridLayout4->addWidget(yellowTime, 8, 1, 1, 4);

        redTimeColor = new QPushButton(tab_5);
        redTimeColor->setObjectName(QString::fromUtf8("redTimeColor"));
        redTimeColor->setAutoFillBackground(true);
        redTimeColor->setFlat(true);

        gridLayout4->addWidget(redTimeColor, 10, 6, 1, 1);

        color_main_window = new QCheckBox(tab_5);
        color_main_window->setObjectName(QString::fromUtf8("color_main_window"));

        gridLayout4->addWidget(color_main_window, 11, 0, 1, 7);

        TextLabel85 = new QLabel(tab_5);
        TextLabel85->setObjectName(QString::fromUtf8("TextLabel85"));
        TextLabel85->setWordWrap(false);

        gridLayout4->addWidget(TextLabel85, 10, 5, 1, 1);

        redTime = new QLineEdit(tab_5);
        redTime->setObjectName(QString::fromUtf8("redTime"));

        gridLayout4->addWidget(redTime, 10, 1, 1, 4);

        TextLabel84 = new QLabel(tab_5);
        TextLabel84->setObjectName(QString::fromUtf8("TextLabel84"));
        TextLabel84->setWordWrap(false);

        gridLayout4->addWidget(TextLabel84, 10, 0, 1, 1);

        TextLabel83 = new QLabel(tab_5);
        TextLabel83->setObjectName(QString::fromUtf8("TextLabel83"));
        TextLabel83->setWordWrap(false);

        gridLayout4->addWidget(TextLabel83, 9, 0, 1, 1);

        orangeTimeColor = new QPushButton(tab_5);
        orangeTimeColor->setObjectName(QString::fromUtf8("orangeTimeColor"));
        orangeTimeColor->setAutoFillBackground(true);
        orangeTimeColor->setFlat(true);

        gridLayout4->addWidget(orangeTimeColor, 9, 6, 1, 1);

        orangeTime = new QLineEdit(tab_5);
        orangeTime->setObjectName(QString::fromUtf8("orangeTime"));

        gridLayout4->addWidget(orangeTime, 9, 1, 1, 4);

        yellowTimeColor = new QPushButton(tab_5);
        yellowTimeColor->setObjectName(QString::fromUtf8("yellowTimeColor"));
        yellowTimeColor->setAutoFillBackground(true);
        yellowTimeColor->setFlat(true);

        gridLayout4->addWidget(yellowTimeColor, 8, 6, 1, 1);

        colordColorCol = new QPushButton(tab_5);
        colordColorCol->setObjectName(QString::fromUtf8("colordColorCol"));
        colordColorCol->setEnabled(true);
        colordColorCol->setFlat(true);

        gridLayout4->addWidget(colordColorCol, 6, 2, 1, 5);

        spacerItem3 = new QSpacerItem(159, 306, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout4->addItem(spacerItem3, 12, 5, 1, 2);

        subtabs->addTab(tab_5, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QString::fromUtf8("tab_6"));
        gridLayout5 = new QGridLayout(tab_6);
#ifndef Q_OS_MAC
        gridLayout5->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout5->setContentsMargins(9, 9, 9, 9);
#endif
        gridLayout5->setObjectName(QString::fromUtf8("gridLayout5"));
        max_songs = new QSpinBox(tab_6);
        max_songs->setObjectName(QString::fromUtf8("max_songs"));
        max_songs->setEnabled(true);
        max_songs->setMaximum(1000000);
        max_songs->setValue(100);

        gridLayout5->addWidget(max_songs, 0, 3, 1, 1);

        dColorMetric = new MetricWidget(tab_6);
        dColorMetric->setObjectName(QString::fromUtf8("dColorMetric"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(dColorMetric->sizePolicy().hasHeightForWidth());
        dColorMetric->setSizePolicy(sizePolicy3);

        gridLayout5->addWidget(dColorMetric, 1, 0, 1, 4);

        textLabel2_2 = new QLabel(tab_6);
        textLabel2_2->setObjectName(QString::fromUtf8("textLabel2_2"));
        textLabel2_2->setWordWrap(false);

        gridLayout5->addWidget(textLabel2_2, 0, 2, 1, 1);

        subtabs->addTab(tab_6, QString());
        tab_7 = new QWidget();
        tab_7->setObjectName(QString::fromUtf8("tab_7"));
        gridLayout6 = new QGridLayout(tab_7);
#ifndef Q_OS_MAC
        gridLayout6->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout6->setContentsMargins(9, 9, 9, 9);
#endif
        gridLayout6->setObjectName(QString::fromUtf8("gridLayout6"));
        clusterDepth = new QSpinBox(tab_7);
        clusterDepth->setObjectName(QString::fromUtf8("clusterDepth"));
        clusterDepth->setValue(3);

        gridLayout6->addWidget(clusterDepth, 0, 1, 1, 1);

        spacerItem4 = new QSpacerItem(265, 448, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout6->addItem(spacerItem4, 2, 0, 1, 1);

        textLabel1_6 = new QLabel(tab_7);
        textLabel1_6->setObjectName(QString::fromUtf8("textLabel1_6"));
        textLabel1_6->setWordWrap(false);

        gridLayout6->addWidget(textLabel1_6, 0, 0, 1, 1);

        textLabel2_5 = new QLabel(tab_7);
        textLabel2_5->setObjectName(QString::fromUtf8("textLabel2_5"));
        textLabel2_5->setWordWrap(false);

        gridLayout6->addWidget(textLabel2_5, 1, 0, 1, 2);

        subtabs->addTab(tab_7, QString());

        gridLayout->addWidget(subtabs, 0, 0, 1, 1);

        buttonBox = new QDialogButtonBox(BpmDjPreferences);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 1, 0, 1, 1);


        retranslateUi(BpmDjPreferences);
        QObject::connect(buttonBox, SIGNAL(accepted()), BpmDjPreferences, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), BpmDjPreferences, SLOT(reject()));

        subtabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(BpmDjPreferences);
    } // setupUi

    void retranslateUi(QDialog *BpmDjPreferences)
    {
        BpmDjPreferences->setWindowTitle(QApplication::translate("BpmDjPreferences", "Song Selector Preferences", 0, QApplication::UnicodeUTF8));
        TextLabel3->setText(QApplication::translate("BpmDjPreferences", "<b>Player commands", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("BpmDjPreferences", "Choose what kind of player you want (tip: use standard) and give it a name (that name is the file shere it will save the players configuration). If the player is on a remote host enter the user@host in the third field.", 0, QApplication::UnicodeUTF8));
        playerInstallButton->setText(QApplication::translate("BpmDjPreferences", "Install", 0, QApplication::UnicodeUTF8));
        subtabs->setTabText(subtabs->indexOf(playerTab), QApplication::translate("BpmDjPreferences", "Players", 0, QApplication::UnicodeUTF8));
        TextLabel3_2->setText(QApplication::translate("BpmDjPreferences", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Analyzer commands</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        analyzerInstallButton->setText(QApplication::translate("BpmDjPreferences", "Install", 0, QApplication::UnicodeUTF8));
        subtabs->setTabText(subtabs->indexOf(analyzerTab), QApplication::translate("BpmDjPreferences", "Analyzers", 0, QApplication::UnicodeUTF8));
        textLabel1_2->setText(QApplication::translate("BpmDjPreferences", "Color matching tempo", 0, QApplication::UnicodeUTF8));
        colorTempo44->setText(QString());
        showTempo54->setText(QApplication::translate("BpmDjPreferences", "show 5/4 harmonics", 0, QApplication::UnicodeUTF8));
        colorTempo54->setText(QString());
        showTempo64->setText(QApplication::translate("BpmDjPreferences", "show 6/4 harmonics", 0, QApplication::UnicodeUTF8));
        colorTempo64->setText(QString());
        showTempo74->setText(QApplication::translate("BpmDjPreferences", "show 7/4 harmonics", 0, QApplication::UnicodeUTF8));
        colorTempo74->setText(QString());
        showTempo84->setText(QApplication::translate("BpmDjPreferences", "show 8/4 harmonics", 0, QApplication::UnicodeUTF8));
        colorTempo84->setText(QString());
        textLabel2_3->setText(QApplication::translate("BpmDjPreferences", "Tempo range", 0, QApplication::UnicodeUTF8));
        showUnknownTempo->setText(QApplication::translate("BpmDjPreferences", "unknown tempo matches perfect", 0, QApplication::UnicodeUTF8));
        bpmalg->setTitle(QApplication::translate("BpmDjPreferences", "Analysis Algorithm", 0, QApplication::UnicodeUTF8));
        resamplingScan->setText(QApplication::translate("BpmDjPreferences", "Rayshoot, Resampling", 0, QApplication::UnicodeUTF8));
        enveloppeSpectrum->setText(QApplication::translate("BpmDjPreferences", "Enveloppe Spectrum", 0, QApplication::UnicodeUTF8));
        experimentalAlg->setText(QApplication::translate("BpmDjPreferences", "Experimental", 0, QApplication::UnicodeUTF8));
        ultraLongFFT->setText(QApplication::translate("BpmDjPreferences", "Rayshoot, FFT Guidance", 0, QApplication::UnicodeUTF8));
        fullAutoCorrelation->setText(QApplication::translate("BpmDjPreferences", "Full Autocorrelation", 0, QApplication::UnicodeUTF8));
        weightedEnvCor->setText(QApplication::translate("BpmDjPreferences", "Weighted", 0, QApplication::UnicodeUTF8));
        TextLabel1_3->setText(QApplication::translate("BpmDjPreferences", "From", 0, QApplication::UnicodeUTF8));
        TextLabel2_3->setText(QApplication::translate("BpmDjPreferences", "To", 0, QApplication::UnicodeUTF8));
        subtabs->setTabText(subtabs->indexOf(tab_4), QApplication::translate("BpmDjPreferences", "Tempo", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("BpmDjPreferences", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Timer colors</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        textLabel1_5_3->setText(QApplication::translate("BpmDjPreferences", "Color of unavailable songs", 0, QApplication::UnicodeUTF8));
        textLabel1_5_3_2->setText(QApplication::translate("BpmDjPreferences", "Color of unchecked songs", 0, QApplication::UnicodeUTF8));
        colorPlayedAuthor->setText(QString());
        textLabel1_5_2->setText(QApplication::translate("BpmDjPreferences", "Color of already played songs", 0, QApplication::UnicodeUTF8));
        colorUnavailableSong->setText(QString());
        colorUncheckedSong->setText(QString());
        colorPlayedSong->setText(QString());
        colorAlltimePlaycount->setText(QString());
        textLabel1->setText(QApplication::translate("BpmDjPreferences", "Author Decay (#songs)", 0, QApplication::UnicodeUTF8));
        textLabel1_5_4->setText(QApplication::translate("BpmDjPreferences", "Alltime playcount color", 0, QApplication::UnicodeUTF8));
        textLabel1_5->setText(QApplication::translate("BpmDjPreferences", "Color of already played authors", 0, QApplication::UnicodeUTF8));
        textLabel2_4->setText(QApplication::translate("BpmDjPreferences", "Color 'dColor' col", 0, QApplication::UnicodeUTF8));
        TextLabel8_2_4->setText(QApplication::translate("BpmDjPreferences", "Normal", 0, QApplication::UnicodeUTF8));
        TextLabel8_2->setText(QApplication::translate("BpmDjPreferences", "seconds, become", 0, QApplication::UnicodeUTF8));
        TextLabel8_2_2->setText(QApplication::translate("BpmDjPreferences", "seconds, become", 0, QApplication::UnicodeUTF8));
        greenTimeColor->setText(QString());
        TextLabel8->setText(QApplication::translate("BpmDjPreferences", "After", 0, QApplication::UnicodeUTF8));
        yellowTime->setText(QApplication::translate("BpmDjPreferences", "120", 0, QApplication::UnicodeUTF8));
        redTimeColor->setText(QString());
        color_main_window->setText(QApplication::translate("BpmDjPreferences", "color entire main window", 0, QApplication::UnicodeUTF8));
        TextLabel85->setText(QApplication::translate("BpmDjPreferences", "seconds, become", 0, QApplication::UnicodeUTF8));
        redTime->setText(QApplication::translate("BpmDjPreferences", "180", 0, QApplication::UnicodeUTF8));
        TextLabel84->setText(QApplication::translate("BpmDjPreferences", "After", 0, QApplication::UnicodeUTF8));
        TextLabel83->setText(QApplication::translate("BpmDjPreferences", "After", 0, QApplication::UnicodeUTF8));
        orangeTimeColor->setText(QString());
        orangeTime->setText(QApplication::translate("BpmDjPreferences", "150", 0, QApplication::UnicodeUTF8));
        yellowTimeColor->setText(QString());
        colordColorCol->setText(QString());
        subtabs->setTabText(subtabs->indexOf(tab_5), QApplication::translate("BpmDjPreferences", "Colors", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        max_songs->setToolTip(QApplication::translate("BpmDjPreferences", "Set to 0 to switch off", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        max_songs->setSpecialValueText(QApplication::translate("BpmDjPreferences", "Unlimited", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        textLabel2_2->setToolTip(QApplication::translate("BpmDjPreferences", "Set to zero for unlimited", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        textLabel2_2->setText(QApplication::translate("BpmDjPreferences", "Maximum # songs", 0, QApplication::UnicodeUTF8));
        subtabs->setTabText(subtabs->indexOf(tab_6), QApplication::translate("BpmDjPreferences", "Metrics", 0, QApplication::UnicodeUTF8));
        textLabel1_6->setText(QApplication::translate("BpmDjPreferences", "After cluster analysis, color based on depth.", 0, QApplication::UnicodeUTF8));
        textLabel2_5->setText(QApplication::translate("BpmDjPreferences", "The number of different colors is 2^depth.", 0, QApplication::UnicodeUTF8));
        subtabs->setTabText(subtabs->indexOf(tab_7), QApplication::translate("BpmDjPreferences", "Cluster", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class BpmDjPreferences: public Ui_BpmDjPreferences {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_BPMDJ_2D_PREF_H
