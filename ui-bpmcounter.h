/********************************************************************************
** Form generated from reading UI file 'ui-bpmcounter.ui'
**
** Created: Thu May 12 17:50:02 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_BPMCOUNTER_H
#define UI_2D_BPMCOUNTER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CountDialog
{
public:
    QGridLayout *gridLayout;
    QGroupBox *algorithm;
    QGridLayout *gridLayout1;
    QRadioButton *weightedEnvCor;
    QRadioButton *fullAutoCorrelation;
    QRadioButton *enveloppeSpectrum;
    QRadioButton *ultraLongFFT;
    QRadioButton *resamplingScan;
    QRadioButton *experimentalAlg;
    QSpacerItem *spacerItem;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem1;
    QLabel *TextLabel1_2;
    QSpinBox *SkipBox;
    QLCDNumber *TapLcd;
    QPushButton *tapButton;
    QPushButton *resetButton;
    QPushButton *BusyToggle;
    QPushButton *StartStop;
    QHBoxLayout *hboxLayout1;
    QProgressBar *ReadingBar;
    QProgressBar *CountingBar;
    QVBoxLayout *vboxLayout;
    QLabel *BpmPix;
    QHBoxLayout *hboxLayout2;
    QLineEdit *FromBpmEdit;
    QSpacerItem *spacerItem2;
    QLabel *X1;
    QSpacerItem *spacerItem3;
    QLabel *X2;
    QSpacerItem *spacerItem4;
    QLabel *X3;
    QSpacerItem *spacerItem5;
    QLineEdit *ToBpmEdit;
    QLabel *StatusLabel;

    void setupUi(QWidget *CountDialog)
    {
        if (CountDialog->objectName().isEmpty())
            CountDialog->setObjectName(QString::fromUtf8("CountDialog"));
        CountDialog->resize(524, 324);
        gridLayout = new QGridLayout(CountDialog);
        gridLayout->setSpacing(5);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        algorithm = new QGroupBox(CountDialog);
        algorithm->setObjectName(QString::fromUtf8("algorithm"));
        gridLayout1 = new QGridLayout(algorithm);
        gridLayout1->setSpacing(0);
        gridLayout1->setContentsMargins(0, 0, 0, 0);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        weightedEnvCor = new QRadioButton(algorithm);
        weightedEnvCor->setObjectName(QString::fromUtf8("weightedEnvCor"));
        weightedEnvCor->setChecked(false);

        gridLayout1->addWidget(weightedEnvCor, 4, 0, 1, 1);

        fullAutoCorrelation = new QRadioButton(algorithm);
        fullAutoCorrelation->setObjectName(QString::fromUtf8("fullAutoCorrelation"));

        gridLayout1->addWidget(fullAutoCorrelation, 3, 0, 1, 1);

        enveloppeSpectrum = new QRadioButton(algorithm);
        enveloppeSpectrum->setObjectName(QString::fromUtf8("enveloppeSpectrum"));

        gridLayout1->addWidget(enveloppeSpectrum, 2, 0, 1, 1);

        ultraLongFFT = new QRadioButton(algorithm);
        ultraLongFFT->setObjectName(QString::fromUtf8("ultraLongFFT"));
        ultraLongFFT->setChecked(false);

        gridLayout1->addWidget(ultraLongFFT, 1, 0, 1, 1);

        resamplingScan = new QRadioButton(algorithm);
        resamplingScan->setObjectName(QString::fromUtf8("resamplingScan"));
        resamplingScan->setChecked(true);

        gridLayout1->addWidget(resamplingScan, 0, 0, 1, 1);

        experimentalAlg = new QRadioButton(algorithm);
        experimentalAlg->setObjectName(QString::fromUtf8("experimentalAlg"));

        gridLayout1->addWidget(experimentalAlg, 5, 0, 1, 1);


        gridLayout->addWidget(algorithm, 1, 0, 1, 1);

        spacerItem = new QSpacerItem(20, 51, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacerItem, 0, 0, 1, 1);

        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        spacerItem1 = new QSpacerItem(91, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem1);

        TextLabel1_2 = new QLabel(CountDialog);
        TextLabel1_2->setObjectName(QString::fromUtf8("TextLabel1_2"));
        TextLabel1_2->setWordWrap(false);

        hboxLayout->addWidget(TextLabel1_2);

        SkipBox = new QSpinBox(CountDialog);
        SkipBox->setObjectName(QString::fromUtf8("SkipBox"));
        SkipBox->setMinimum(1);
        SkipBox->setMaximum(32);
        SkipBox->setValue(4);

        hboxLayout->addWidget(SkipBox);

        TapLcd = new QLCDNumber(CountDialog);
        TapLcd->setObjectName(QString::fromUtf8("TapLcd"));
        TapLcd->setNumDigits(4);

        hboxLayout->addWidget(TapLcd);

        tapButton = new QPushButton(CountDialog);
        tapButton->setObjectName(QString::fromUtf8("tapButton"));

        hboxLayout->addWidget(tapButton);

        resetButton = new QPushButton(CountDialog);
        resetButton->setObjectName(QString::fromUtf8("resetButton"));

        hboxLayout->addWidget(resetButton);

        BusyToggle = new QPushButton(CountDialog);
        BusyToggle->setObjectName(QString::fromUtf8("BusyToggle"));
        BusyToggle->setEnabled(false);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(BusyToggle->sizePolicy().hasHeightForWidth());
        BusyToggle->setSizePolicy(sizePolicy);
        BusyToggle->setFocusPolicy(Qt::NoFocus);
        BusyToggle->setCheckable(true);
        BusyToggle->setFlat(true);

        hboxLayout->addWidget(BusyToggle);

        StartStop = new QPushButton(CountDialog);
        StartStop->setObjectName(QString::fromUtf8("StartStop"));
        StartStop->setEnabled(true);

        hboxLayout->addWidget(StartStop);


        gridLayout->addLayout(hboxLayout, 3, 0, 1, 2);

        hboxLayout1 = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout1->setSpacing(6);
#endif
        hboxLayout1->setContentsMargins(0, 0, 0, 0);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        ReadingBar = new QProgressBar(CountDialog);
        ReadingBar->setObjectName(QString::fromUtf8("ReadingBar"));

        hboxLayout1->addWidget(ReadingBar);

        CountingBar = new QProgressBar(CountDialog);
        CountingBar->setObjectName(QString::fromUtf8("CountingBar"));

        hboxLayout1->addWidget(CountingBar);


        gridLayout->addLayout(hboxLayout1, 2, 0, 1, 2);

        vboxLayout = new QVBoxLayout();
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
        vboxLayout->setContentsMargins(0, 0, 0, 0);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        BpmPix = new QLabel(CountDialog);
        BpmPix->setObjectName(QString::fromUtf8("BpmPix"));
        BpmPix->setMinimumSize(QSize(0, 200));
        BpmPix->setCursor(QCursor(Qt::ArrowCursor));
        BpmPix->setFrameShape(QFrame::Panel);
        BpmPix->setFrameShadow(QFrame::Sunken);
        BpmPix->setScaledContents(true);
        BpmPix->setWordWrap(false);

        vboxLayout->addWidget(BpmPix);

        hboxLayout2 = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout2->setSpacing(6);
#endif
        hboxLayout2->setContentsMargins(0, 0, 0, 0);
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        FromBpmEdit = new QLineEdit(CountDialog);
        FromBpmEdit->setObjectName(QString::fromUtf8("FromBpmEdit"));
        FromBpmEdit->setEnabled(true);
        FromBpmEdit->setMinimumSize(QSize(50, 0));
        FromBpmEdit->setFrame(false);

        hboxLayout2->addWidget(FromBpmEdit);

        spacerItem2 = new QSpacerItem(160, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout2->addItem(spacerItem2);

        X1 = new QLabel(CountDialog);
        X1->setObjectName(QString::fromUtf8("X1"));
        X1->setAlignment(Qt::AlignCenter);
        X1->setWordWrap(false);

        hboxLayout2->addWidget(X1);

        spacerItem3 = new QSpacerItem(201, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout2->addItem(spacerItem3);

        X2 = new QLabel(CountDialog);
        X2->setObjectName(QString::fromUtf8("X2"));
        X2->setAlignment(Qt::AlignCenter);
        X2->setWordWrap(false);

        hboxLayout2->addWidget(X2);

        spacerItem4 = new QSpacerItem(200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout2->addItem(spacerItem4);

        X3 = new QLabel(CountDialog);
        X3->setObjectName(QString::fromUtf8("X3"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(X3->sizePolicy().hasHeightForWidth());
        X3->setSizePolicy(sizePolicy1);
        X3->setAlignment(Qt::AlignCenter);
        X3->setWordWrap(false);

        hboxLayout2->addWidget(X3);

        spacerItem5 = new QSpacerItem(201, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout2->addItem(spacerItem5);

        ToBpmEdit = new QLineEdit(CountDialog);
        ToBpmEdit->setObjectName(QString::fromUtf8("ToBpmEdit"));
        ToBpmEdit->setEnabled(true);
        QSizePolicy sizePolicy2(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(ToBpmEdit->sizePolicy().hasHeightForWidth());
        ToBpmEdit->setSizePolicy(sizePolicy2);
        ToBpmEdit->setFrame(false);
        ToBpmEdit->setAlignment(Qt::AlignRight);

        hboxLayout2->addWidget(ToBpmEdit);


        vboxLayout->addLayout(hboxLayout2);


        gridLayout->addLayout(vboxLayout, 0, 1, 2, 1);

        StatusLabel = new QLabel(CountDialog);
        StatusLabel->setObjectName(QString::fromUtf8("StatusLabel"));
        StatusLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(StatusLabel, 4, 0, 1, 2);


        retranslateUi(CountDialog);
        QObject::connect(StartStop, SIGNAL(clicked()), CountDialog, SLOT(startStopButton()));
        QObject::connect(BusyToggle, SIGNAL(toggled(bool)), FromBpmEdit, SLOT(setDisabled(bool)));
        QObject::connect(BusyToggle, SIGNAL(toggled(bool)), ToBpmEdit, SLOT(setDisabled(bool)));
        QObject::connect(BusyToggle, SIGNAL(toggled(bool)), tapButton, SLOT(setDisabled(bool)));
        QObject::connect(BusyToggle, SIGNAL(toggled(bool)), resetButton, SLOT(setDisabled(bool)));
        QObject::connect(tapButton, SIGNAL(pressed()), CountDialog, SLOT(tap()));
        QObject::connect(resetButton, SIGNAL(pressed()), CountDialog, SLOT(reset()));
        QObject::connect(BusyToggle, SIGNAL(toggled(bool)), algorithm, SLOT(setDisabled(bool)));

        QMetaObject::connectSlotsByName(CountDialog);
    } // setupUi

    void retranslateUi(QWidget *CountDialog)
    {
        CountDialog->setWindowTitle(QApplication::translate("CountDialog", "Form", 0, QApplication::UnicodeUTF8));
        algorithm->setTitle(QApplication::translate("CountDialog", "Algorithm", 0, QApplication::UnicodeUTF8));
        weightedEnvCor->setText(QApplication::translate("CountDialog", "Weighted", 0, QApplication::UnicodeUTF8));
        fullAutoCorrelation->setText(QApplication::translate("CountDialog", "Full Autocorrelation", 0, QApplication::UnicodeUTF8));
        enveloppeSpectrum->setText(QApplication::translate("CountDialog", "Enveloppe Spectrum", 0, QApplication::UnicodeUTF8));
        ultraLongFFT->setText(QApplication::translate("CountDialog", "Rayshoot, FFT Guidance", 0, QApplication::UnicodeUTF8));
        resamplingScan->setText(QApplication::translate("CountDialog", "Rayshoot, Resampling", 0, QApplication::UnicodeUTF8));
        experimentalAlg->setText(QApplication::translate("CountDialog", "Experimental Alg", 0, QApplication::UnicodeUTF8));
        TextLabel1_2->setText(QApplication::translate("CountDialog", "Beats/tap", 0, QApplication::UnicodeUTF8));
        tapButton->setText(QApplication::translate("CountDialog", "&Tap", 0, QApplication::UnicodeUTF8));
        tapButton->setShortcut(QApplication::translate("CountDialog", "Alt+T", 0, QApplication::UnicodeUTF8));
        resetButton->setText(QApplication::translate("CountDialog", "&Reset", 0, QApplication::UnicodeUTF8));
        resetButton->setShortcut(QApplication::translate("CountDialog", "Alt+R", 0, QApplication::UnicodeUTF8));
        BusyToggle->setText(QString());
        BusyToggle->setShortcut(QString());
        StartStop->setText(QApplication::translate("CountDialog", "&Start", 0, QApplication::UnicodeUTF8));
        StartStop->setShortcut(QApplication::translate("CountDialog", "Alt+S", 0, QApplication::UnicodeUTF8));
        FromBpmEdit->setText(QApplication::translate("CountDialog", "80", 0, QApplication::UnicodeUTF8));
        X1->setText(QApplication::translate("CountDialog", ".", 0, QApplication::UnicodeUTF8));
        X2->setText(QApplication::translate("CountDialog", ".", 0, QApplication::UnicodeUTF8));
        X3->setText(QApplication::translate("CountDialog", ".", 0, QApplication::UnicodeUTF8));
        ToBpmEdit->setText(QApplication::translate("CountDialog", "160", 0, QApplication::UnicodeUTF8));
        StatusLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class CountDialog: public Ui_CountDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_BPMCOUNTER_H
