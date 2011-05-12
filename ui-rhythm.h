/********************************************************************************
** Form generated from reading UI file 'ui-rhythm.ui'
**
** Created: Thu May 12 17:50:02 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_RHYTHM_H
#define UI_2D_RHYTHM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RhythmAnalyzer
{
public:
    QGridLayout *gridLayout;
    QLabel *composition_freq;
    QLabel *composition;
    QLabel *textLabel1_2_2;
    QSpinBox *beats;
    QProgressBar *Progress;
    QLabel *rhythm;
    QLabel *textLabel1;
    QLabel *textLabel1_2_2_2;
    QLabel *projection;
    QPushButton *calculate;
    QLabel *textLabel1_2;
    QLabel *status_bar;

    void setupUi(QWidget *RhythmAnalyzer)
    {
        if (RhythmAnalyzer->objectName().isEmpty())
            RhythmAnalyzer->setObjectName(QString::fromUtf8("RhythmAnalyzer"));
        RhythmAnalyzer->resize(463, 242);
        gridLayout = new QGridLayout(RhythmAnalyzer);
        gridLayout->setSpacing(2);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        composition_freq = new QLabel(RhythmAnalyzer);
        composition_freq->setObjectName(QString::fromUtf8("composition_freq"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(composition_freq->sizePolicy().hasHeightForWidth());
        composition_freq->setSizePolicy(sizePolicy);
        composition_freq->setFrameShape(QFrame::Panel);
        composition_freq->setScaledContents(true);
        composition_freq->setWordWrap(false);

        gridLayout->addWidget(composition_freq, 6, 0, 1, 5);

        composition = new QLabel(RhythmAnalyzer);
        composition->setObjectName(QString::fromUtf8("composition"));
        sizePolicy.setHeightForWidth(composition->sizePolicy().hasHeightForWidth());
        composition->setSizePolicy(sizePolicy);
        composition->setFrameShape(QFrame::Panel);
        composition->setScaledContents(true);
        composition->setWordWrap(false);

        gridLayout->addWidget(composition, 4, 0, 1, 5);

        textLabel1_2_2 = new QLabel(RhythmAnalyzer);
        textLabel1_2_2->setObjectName(QString::fromUtf8("textLabel1_2_2"));
        textLabel1_2_2->setWordWrap(false);

        gridLayout->addWidget(textLabel1_2_2, 3, 0, 1, 3);

        beats = new QSpinBox(RhythmAnalyzer);
        beats->setObjectName(QString::fromUtf8("beats"));
        beats->setMinimum(1);
        beats->setMaximum(32);
        beats->setValue(4);

        gridLayout->addWidget(beats, 8, 2, 1, 1);

        Progress = new QProgressBar(RhythmAnalyzer);
        Progress->setObjectName(QString::fromUtf8("Progress"));

        gridLayout->addWidget(Progress, 8, 3, 1, 1);

        rhythm = new QLabel(RhythmAnalyzer);
        rhythm->setObjectName(QString::fromUtf8("rhythm"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(rhythm->sizePolicy().hasHeightForWidth());
        rhythm->setSizePolicy(sizePolicy1);
        rhythm->setFrameShape(QFrame::Panel);
        rhythm->setScaledContents(true);
        rhythm->setWordWrap(false);

        gridLayout->addWidget(rhythm, 1, 0, 1, 5);

        textLabel1 = new QLabel(RhythmAnalyzer);
        textLabel1->setObjectName(QString::fromUtf8("textLabel1"));
        textLabel1->setWordWrap(false);

        gridLayout->addWidget(textLabel1, 8, 0, 1, 2);

        textLabel1_2_2_2 = new QLabel(RhythmAnalyzer);
        textLabel1_2_2_2->setObjectName(QString::fromUtf8("textLabel1_2_2_2"));
        textLabel1_2_2_2->setWordWrap(false);

        gridLayout->addWidget(textLabel1_2_2_2, 5, 0, 1, 5);

        projection = new QLabel(RhythmAnalyzer);
        projection->setObjectName(QString::fromUtf8("projection"));
        projection->setMinimumSize(QSize(0, 10));
        projection->setFrameShape(QFrame::Panel);
        projection->setScaledContents(true);
        projection->setWordWrap(false);

        gridLayout->addWidget(projection, 2, 0, 1, 5);

        calculate = new QPushButton(RhythmAnalyzer);
        calculate->setObjectName(QString::fromUtf8("calculate"));

        gridLayout->addWidget(calculate, 8, 4, 1, 1);

        textLabel1_2 = new QLabel(RhythmAnalyzer);
        textLabel1_2->setObjectName(QString::fromUtf8("textLabel1_2"));
        textLabel1_2->setWordWrap(false);

        gridLayout->addWidget(textLabel1_2, 0, 0, 1, 3);

        status_bar = new QLabel(RhythmAnalyzer);
        status_bar->setObjectName(QString::fromUtf8("status_bar"));
        status_bar->setWordWrap(false);

        gridLayout->addWidget(status_bar, 7, 0, 1, 5);


        retranslateUi(RhythmAnalyzer);

        QMetaObject::connectSlotsByName(RhythmAnalyzer);
    } // setupUi

    void retranslateUi(QWidget *RhythmAnalyzer)
    {
        RhythmAnalyzer->setWindowTitle(QApplication::translate("RhythmAnalyzer", "Form", 0, QApplication::UnicodeUTF8));
        textLabel1_2_2->setText(QApplication::translate("RhythmAnalyzer", "Composition Map", 0, QApplication::UnicodeUTF8));
        textLabel1->setText(QApplication::translate("RhythmAnalyzer", "Beats", 0, QApplication::UnicodeUTF8));
        textLabel1_2_2_2->setText(QApplication::translate("RhythmAnalyzer", "Probability of composition change after x measures at freq y", 0, QApplication::UnicodeUTF8));
        calculate->setText(QApplication::translate("RhythmAnalyzer", "Calculate", 0, QApplication::UnicodeUTF8));
        textLabel1_2->setText(QApplication::translate("RhythmAnalyzer", "1 Measure looks like", 0, QApplication::UnicodeUTF8));
        status_bar->setText(QApplication::translate("RhythmAnalyzer", "...", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RhythmAnalyzer: public Ui_RhythmAnalyzer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_RHYTHM_H
