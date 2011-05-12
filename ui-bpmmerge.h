/********************************************************************************
** Form generated from reading UI file 'ui-bpmmerge.ui'
**
** Created: Thu May 12 14:15:37 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_BPMMERGE_H
#define UI_2D_BPMMERGE_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_MergerDialog
{
public:
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *startAtCue;
    QCheckBox *beforeCue;
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout;
    QCheckBox *alwaysAt;
    QLabel *textLabel5;
    QVBoxLayout *vboxLayout1;
    QSpinBox *headPercent;
    QSpinBox *tailPercent;
    QVBoxLayout *vboxLayout2;
    QLabel *label;
    QLabel *label_2;
    QLabel *textLabel1_3;
    QHBoxLayout *horizontalLayout_3;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *normalizeSongs;
    QCheckBox *volumeAccounting;
    QCheckBox *fineScan;
    QHBoxLayout *hboxLayout1;
    QVBoxLayout *vboxLayout3;
    QLabel *textLabel2;
    QLabel *textLabel1_2;
    QLabel *textLabel3;
    QVBoxLayout *vboxLayout4;
    QSpinBox *mixMeasures;
    QSpinBox *slideMeasures;
    QSpinBox *tempoMeasures;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout;
    QCheckBox *splitBeforeMix;
    QCheckBox *splitMiddleMix;
    QCheckBox *splitAfterMix;
    QCheckBox *splitAfterTempo;
    QCheckBox *number;
    QCheckBox *wavConvert;
    QHBoxLayout *horizontalLayout;
    QCheckBox *verbose;
    QPushButton *pushButton4;
    QPushButton *okButton;

    void setupUi(QDialog *MergerDialog)
    {
        if (MergerDialog->objectName().isEmpty())
            MergerDialog->setObjectName(QString::fromUtf8("MergerDialog"));
        MergerDialog->resize(564, 429);
        verticalLayout_4 = new QVBoxLayout(MergerDialog);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        groupBox = new QGroupBox(MergerDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_3 = new QVBoxLayout(groupBox);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        startAtCue = new QCheckBox(groupBox);
        QButtonGroup *buttonGroup = new QButtonGroup(MergerDialog);
        buttonGroup->setObjectName(QString::fromUtf8("buttonGroup"));
        buttonGroup->addButton(startAtCue);
        startAtCue->setObjectName(QString::fromUtf8("startAtCue"));

        verticalLayout_3->addWidget(startAtCue);

        beforeCue = new QCheckBox(groupBox);
        buttonGroup->addButton(beforeCue);
        beforeCue->setObjectName(QString::fromUtf8("beforeCue"));

        verticalLayout_3->addWidget(beforeCue);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        vboxLayout = new QVBoxLayout();
        vboxLayout->setSpacing(6);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        alwaysAt = new QCheckBox(groupBox);
        alwaysAt->setObjectName(QString::fromUtf8("alwaysAt"));
        alwaysAt->setChecked(true);

        vboxLayout->addWidget(alwaysAt);

        textLabel5 = new QLabel(groupBox);
        textLabel5->setObjectName(QString::fromUtf8("textLabel5"));
        textLabel5->setWordWrap(false);

        vboxLayout->addWidget(textLabel5);


        hboxLayout->addLayout(vboxLayout);

        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setSpacing(6);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        headPercent = new QSpinBox(groupBox);
        headPercent->setObjectName(QString::fromUtf8("headPercent"));
        headPercent->setMaximum(100);
        headPercent->setValue(25);

        vboxLayout1->addWidget(headPercent);

        tailPercent = new QSpinBox(groupBox);
        tailPercent->setObjectName(QString::fromUtf8("tailPercent"));
        tailPercent->setMaximum(100);
        tailPercent->setValue(75);

        vboxLayout1->addWidget(tailPercent);


        hboxLayout->addLayout(vboxLayout1);

        vboxLayout2 = new QVBoxLayout();
        vboxLayout2->setSpacing(6);
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);

        vboxLayout2->addWidget(label);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);

        vboxLayout2->addWidget(label_2);


        hboxLayout->addLayout(vboxLayout2);


        verticalLayout_3->addLayout(hboxLayout);


        horizontalLayout_2->addWidget(groupBox);

        textLabel1_3 = new QLabel(MergerDialog);
        textLabel1_3->setObjectName(QString::fromUtf8("textLabel1_3"));
        textLabel1_3->setWordWrap(true);

        horizontalLayout_2->addWidget(textLabel1_3);


        verticalLayout_4->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        groupBox_2 = new QGroupBox(MergerDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout_2 = new QVBoxLayout(groupBox_2);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        normalizeSongs = new QCheckBox(groupBox_2);
        normalizeSongs->setObjectName(QString::fromUtf8("normalizeSongs"));
        normalizeSongs->setEnabled(true);
        normalizeSongs->setChecked(false);

        verticalLayout_2->addWidget(normalizeSongs);

        volumeAccounting = new QCheckBox(groupBox_2);
        volumeAccounting->setObjectName(QString::fromUtf8("volumeAccounting"));
        volumeAccounting->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(volumeAccounting->sizePolicy().hasHeightForWidth());
        volumeAccounting->setSizePolicy(sizePolicy1);

        verticalLayout_2->addWidget(volumeAccounting);

        fineScan = new QCheckBox(groupBox_2);
        fineScan->setObjectName(QString::fromUtf8("fineScan"));

        verticalLayout_2->addWidget(fineScan);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(6);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        vboxLayout3 = new QVBoxLayout();
        vboxLayout3->setSpacing(6);
        vboxLayout3->setObjectName(QString::fromUtf8("vboxLayout3"));
        textLabel2 = new QLabel(groupBox_2);
        textLabel2->setObjectName(QString::fromUtf8("textLabel2"));
        textLabel2->setWordWrap(false);

        vboxLayout3->addWidget(textLabel2);

        textLabel1_2 = new QLabel(groupBox_2);
        textLabel1_2->setObjectName(QString::fromUtf8("textLabel1_2"));
        textLabel1_2->setWordWrap(false);

        vboxLayout3->addWidget(textLabel1_2);

        textLabel3 = new QLabel(groupBox_2);
        textLabel3->setObjectName(QString::fromUtf8("textLabel3"));
        textLabel3->setWordWrap(false);

        vboxLayout3->addWidget(textLabel3);


        hboxLayout1->addLayout(vboxLayout3);

        vboxLayout4 = new QVBoxLayout();
        vboxLayout4->setSpacing(6);
        vboxLayout4->setObjectName(QString::fromUtf8("vboxLayout4"));
        mixMeasures = new QSpinBox(groupBox_2);
        mixMeasures->setObjectName(QString::fromUtf8("mixMeasures"));
        mixMeasures->setMinimum(1);
        mixMeasures->setMaximum(64);
        mixMeasures->setValue(8);

        vboxLayout4->addWidget(mixMeasures);

        slideMeasures = new QSpinBox(groupBox_2);
        slideMeasures->setObjectName(QString::fromUtf8("slideMeasures"));
        slideMeasures->setMinimum(1);
        slideMeasures->setValue(16);

        vboxLayout4->addWidget(slideMeasures);

        tempoMeasures = new QSpinBox(groupBox_2);
        tempoMeasures->setObjectName(QString::fromUtf8("tempoMeasures"));
        tempoMeasures->setMinimum(0);
        tempoMeasures->setMaximum(64);
        tempoMeasures->setValue(16);

        vboxLayout4->addWidget(tempoMeasures);


        hboxLayout1->addLayout(vboxLayout4);


        verticalLayout_2->addLayout(hboxLayout1);


        horizontalLayout_3->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(MergerDialog);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        verticalLayout = new QVBoxLayout(groupBox_3);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        splitBeforeMix = new QCheckBox(groupBox_3);
        splitBeforeMix->setObjectName(QString::fromUtf8("splitBeforeMix"));
        splitBeforeMix->setEnabled(true);

        verticalLayout->addWidget(splitBeforeMix);

        splitMiddleMix = new QCheckBox(groupBox_3);
        splitMiddleMix->setObjectName(QString::fromUtf8("splitMiddleMix"));
        splitMiddleMix->setEnabled(true);
        splitMiddleMix->setChecked(true);

        verticalLayout->addWidget(splitMiddleMix);

        splitAfterMix = new QCheckBox(groupBox_3);
        splitAfterMix->setObjectName(QString::fromUtf8("splitAfterMix"));
        splitAfterMix->setEnabled(true);

        verticalLayout->addWidget(splitAfterMix);

        splitAfterTempo = new QCheckBox(groupBox_3);
        splitAfterTempo->setObjectName(QString::fromUtf8("splitAfterTempo"));
        splitAfterTempo->setEnabled(true);

        verticalLayout->addWidget(splitAfterTempo);

        number = new QCheckBox(groupBox_3);
        number->setObjectName(QString::fromUtf8("number"));
        number->setEnabled(true);
        number->setChecked(true);

        verticalLayout->addWidget(number);

        wavConvert = new QCheckBox(groupBox_3);
        wavConvert->setObjectName(QString::fromUtf8("wavConvert"));
        wavConvert->setEnabled(true);
        wavConvert->setChecked(true);

        verticalLayout->addWidget(wavConvert);


        horizontalLayout_3->addWidget(groupBox_3);


        verticalLayout_4->addLayout(horizontalLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verbose = new QCheckBox(MergerDialog);
        verbose->setObjectName(QString::fromUtf8("verbose"));
        QSizePolicy sizePolicy2(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(verbose->sizePolicy().hasHeightForWidth());
        verbose->setSizePolicy(sizePolicy2);
        verbose->setChecked(true);

        horizontalLayout->addWidget(verbose);

        pushButton4 = new QPushButton(MergerDialog);
        pushButton4->setObjectName(QString::fromUtf8("pushButton4"));

        horizontalLayout->addWidget(pushButton4);

        okButton = new QPushButton(MergerDialog);
        okButton->setObjectName(QString::fromUtf8("okButton"));

        horizontalLayout->addWidget(okButton);


        verticalLayout_4->addLayout(horizontalLayout);

        QWidget::setTabOrder(tailPercent, alwaysAt);
        QWidget::setTabOrder(alwaysAt, headPercent);
        QWidget::setTabOrder(headPercent, mixMeasures);
        QWidget::setTabOrder(mixMeasures, tempoMeasures);
        QWidget::setTabOrder(tempoMeasures, splitBeforeMix);
        QWidget::setTabOrder(splitBeforeMix, splitMiddleMix);
        QWidget::setTabOrder(splitMiddleMix, splitAfterMix);
        QWidget::setTabOrder(splitAfterMix, splitAfterTempo);
        QWidget::setTabOrder(splitAfterTempo, number);
        QWidget::setTabOrder(number, okButton);
        QWidget::setTabOrder(okButton, pushButton4);

        retranslateUi(MergerDialog);
        QObject::connect(okButton, SIGNAL(clicked()), MergerDialog, SLOT(accept()));
        QObject::connect(pushButton4, SIGNAL(clicked()), MergerDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(MergerDialog);
    } // setupUi

    void retranslateUi(QDialog *MergerDialog)
    {
        MergerDialog->setWindowTitle(QApplication::translate("MergerDialog", "BpmDj Offline Auto Mixer", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MergerDialog", "Each song should", 0, QApplication::UnicodeUTF8));
        startAtCue->setText(QApplication::translate("MergerDialog", "Starts at last used cue", 0, QApplication::UnicodeUTF8));
        beforeCue->setText(QApplication::translate("MergerDialog", "Start before last used cue", 0, QApplication::UnicodeUTF8));
        alwaysAt->setText(QApplication::translate("MergerDialog", "Start at ", 0, QApplication::UnicodeUTF8));
        textLabel5->setText(QApplication::translate("MergerDialog", "Stop at", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MergerDialog", "%", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MergerDialog", "%", 0, QApplication::UnicodeUTF8));
        textLabel1_3->setText(QApplication::translate("MergerDialog", "The automixer will mix any two songs that have the same rhythm. When you press OK, a script process_mix.sh will be written in the current directory. Use that to create the mix.", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("MergerDialog", "Processing", 0, QApplication::UnicodeUTF8));
        normalizeSongs->setText(QApplication::translate("MergerDialog", "Normalize Songs", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        volumeAccounting->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        volumeAccounting->setText(QApplication::translate("MergerDialog", "Volume Accounting", 0, QApplication::UnicodeUTF8));
        fineScan->setText(QApplication::translate("MergerDialog", "Fine Scan Tempo", 0, QApplication::UnicodeUTF8));
        textLabel2->setText(QApplication::translate("MergerDialog", "Measures to Mix", 0, QApplication::UnicodeUTF8));
        textLabel1_2->setText(QApplication::translate("MergerDialog", "Measures to slide", 0, QApplication::UnicodeUTF8));
        textLabel3->setText(QApplication::translate("MergerDialog", "Measures for Tempo Switch", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("MergerDialog", "Output", 0, QApplication::UnicodeUTF8));
        splitBeforeMix->setText(QApplication::translate("MergerDialog", "Split Before Mix", 0, QApplication::UnicodeUTF8));
        splitMiddleMix->setText(QApplication::translate("MergerDialog", "Split Middle of Mix", 0, QApplication::UnicodeUTF8));
        splitAfterMix->setText(QApplication::translate("MergerDialog", "Split After Mix/Before Tempo Switch", 0, QApplication::UnicodeUTF8));
        splitAfterTempo->setText(QApplication::translate("MergerDialog", "Split After Tempo Switch", 0, QApplication::UnicodeUTF8));
        number->setText(QApplication::translate("MergerDialog", "Number Them", 0, QApplication::UnicodeUTF8));
        wavConvert->setText(QApplication::translate("MergerDialog", "Convert to .WAV (using sox)", 0, QApplication::UnicodeUTF8));
        verbose->setText(QApplication::translate("MergerDialog", "Verbose", 0, QApplication::UnicodeUTF8));
        pushButton4->setText(QApplication::translate("MergerDialog", "Cancel", 0, QApplication::UnicodeUTF8));
        okButton->setText(QApplication::translate("MergerDialog", "Ok", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MergerDialog: public Ui_MergerDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_BPMMERGE_H
