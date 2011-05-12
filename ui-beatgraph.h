/********************************************************************************
** Form generated from reading UI file 'ui-beatgraph.ui'
**
** Created: Thu May 12 17:50:01 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_BEATGRAPH_H
#define UI_2D_BEATGRAPH_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>
#include "beatgraph-label.h"

QT_BEGIN_NAMESPACE

class Ui_BeatGraph
{
public:
    QLabel *switcherLabel;
    QGridLayout *gridLayout;
    BeatGraphLabel *pattern;
    QSlider *periodDelta;
    QSlider *periodDelta10;
    QHBoxLayout *horizontalLayout;
    QLabel *zlabel;
    QLabel *xlabel;
    QLabel *clabel;
    QLabel *vlabel;
    QLabel *nowlabel;
    QLCDNumber *currentTempoLcd;
    QSpacerItem *spacerItem;
    QRadioButton *bg_gray;
    QRadioButton *bg_f1;
    QRadioButton *bg_haar;
    QPushButton *pushButton4;

    void setupUi(QWidget *BeatGraph)
    {
        if (BeatGraph->objectName().isEmpty())
            BeatGraph->setObjectName(QString::fromUtf8("BeatGraph"));
        BeatGraph->resize(592, 358);
        switcherLabel = new QLabel(BeatGraph);
        switcherLabel->setObjectName(QString::fromUtf8("switcherLabel"));
        switcherLabel->setGeometry(QRect(100, 318, 16, 31));
        QFont font;
        font.setPointSize(8);
        switcherLabel->setFont(font);
        switcherLabel->setAutoFillBackground(true);
        switcherLabel->setWordWrap(false);
        gridLayout = new QGridLayout(BeatGraph);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pattern = new BeatGraphLabel(BeatGraph);
        pattern->setObjectName(QString::fromUtf8("pattern"));

        gridLayout->addWidget(pattern, 0, 0, 1, 1);

        periodDelta = new QSlider(BeatGraph);
        periodDelta->setObjectName(QString::fromUtf8("periodDelta"));
        periodDelta->setMinimum(-100);
        periodDelta->setMaximum(100);
        periodDelta->setOrientation(Qt::Vertical);
        periodDelta->setTickInterval(100);

        gridLayout->addWidget(periodDelta, 0, 1, 1, 1);

        periodDelta10 = new QSlider(BeatGraph);
        periodDelta10->setObjectName(QString::fromUtf8("periodDelta10"));
        periodDelta10->setMinimum(-1000);
        periodDelta10->setMaximum(1000);
        periodDelta10->setSingleStep(10);
        periodDelta10->setPageStep(100);
        periodDelta10->setOrientation(Qt::Vertical);
        periodDelta10->setTickInterval(100);

        gridLayout->addWidget(periodDelta10, 0, 2, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        zlabel = new QLabel(BeatGraph);
        zlabel->setObjectName(QString::fromUtf8("zlabel"));
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush1(QColor(255, 0, 0, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        zlabel->setPalette(palette);
        zlabel->setFont(font);
        zlabel->setAutoFillBackground(true);
        zlabel->setWordWrap(false);

        horizontalLayout->addWidget(zlabel);

        xlabel = new QLabel(BeatGraph);
        xlabel->setObjectName(QString::fromUtf8("xlabel"));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush2(QColor(0, 255, 0, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        xlabel->setPalette(palette1);
        xlabel->setFont(font);
        xlabel->setAutoFillBackground(true);
        xlabel->setWordWrap(false);

        horizontalLayout->addWidget(xlabel);

        clabel = new QLabel(BeatGraph);
        clabel->setObjectName(QString::fromUtf8("clabel"));
        QPalette palette2;
        palette2.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush3(QColor(0, 0, 255, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Window, brush3);
        palette2.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Window, brush3);
        palette2.setBrush(QPalette::Disabled, QPalette::Base, brush3);
        palette2.setBrush(QPalette::Disabled, QPalette::Window, brush3);
        clabel->setPalette(palette2);
        clabel->setFont(font);
        clabel->setAutoFillBackground(true);
        clabel->setWordWrap(false);

        horizontalLayout->addWidget(clabel);

        vlabel = new QLabel(BeatGraph);
        vlabel->setObjectName(QString::fromUtf8("vlabel"));
        QPalette palette3;
        palette3.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush4(QColor(255, 255, 0, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette3.setBrush(QPalette::Active, QPalette::Window, brush4);
        palette3.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette3.setBrush(QPalette::Inactive, QPalette::Window, brush4);
        palette3.setBrush(QPalette::Disabled, QPalette::Base, brush4);
        palette3.setBrush(QPalette::Disabled, QPalette::Window, brush4);
        vlabel->setPalette(palette3);
        vlabel->setFont(font);
        vlabel->setAutoFillBackground(true);
        vlabel->setWordWrap(false);

        horizontalLayout->addWidget(vlabel);

        nowlabel = new QLabel(BeatGraph);
        nowlabel->setObjectName(QString::fromUtf8("nowlabel"));
        QPalette palette4;
        palette4.setBrush(QPalette::Active, QPalette::Base, brush);
        palette4.setBrush(QPalette::Active, QPalette::Window, brush);
        palette4.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette4.setBrush(QPalette::Inactive, QPalette::Window, brush);
        palette4.setBrush(QPalette::Disabled, QPalette::Base, brush);
        palette4.setBrush(QPalette::Disabled, QPalette::Window, brush);
        nowlabel->setPalette(palette4);
        nowlabel->setAutoFillBackground(true);
        nowlabel->setWordWrap(false);

        horizontalLayout->addWidget(nowlabel);

        currentTempoLcd = new QLCDNumber(BeatGraph);
        currentTempoLcd->setObjectName(QString::fromUtf8("currentTempoLcd"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(currentTempoLcd->sizePolicy().hasHeightForWidth());
        currentTempoLcd->setSizePolicy(sizePolicy);
        QPalette palette5;
        palette5.setBrush(QPalette::Active, QPalette::WindowText, brush4);
        palette5.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush5(QColor(0, 0, 0, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette5.setBrush(QPalette::Active, QPalette::Window, brush5);
        palette5.setBrush(QPalette::Inactive, QPalette::WindowText, brush4);
        palette5.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette5.setBrush(QPalette::Inactive, QPalette::Window, brush5);
        QBrush brush6(QColor(121, 125, 121, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette5.setBrush(QPalette::Disabled, QPalette::WindowText, brush6);
        palette5.setBrush(QPalette::Disabled, QPalette::Base, brush5);
        palette5.setBrush(QPalette::Disabled, QPalette::Window, brush5);
        currentTempoLcd->setPalette(palette5);
        currentTempoLcd->setAutoFillBackground(true);
        currentTempoLcd->setNumDigits(6);
        currentTempoLcd->setSegmentStyle(QLCDNumber::Flat);

        horizontalLayout->addWidget(currentTempoLcd);

        spacerItem = new QSpacerItem(18, 28, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(spacerItem);

        bg_gray = new QRadioButton(BeatGraph);
        bg_gray->setObjectName(QString::fromUtf8("bg_gray"));
        bg_gray->setChecked(true);

        horizontalLayout->addWidget(bg_gray);

        bg_f1 = new QRadioButton(BeatGraph);
        bg_f1->setObjectName(QString::fromUtf8("bg_f1"));

        horizontalLayout->addWidget(bg_f1);

        bg_haar = new QRadioButton(BeatGraph);
        bg_haar->setObjectName(QString::fromUtf8("bg_haar"));

        horizontalLayout->addWidget(bg_haar);

        pushButton4 = new QPushButton(BeatGraph);
        pushButton4->setObjectName(QString::fromUtf8("pushButton4"));

        horizontalLayout->addWidget(pushButton4);


        gridLayout->addLayout(horizontalLayout, 1, 0, 1, 3);

        QWidget::setTabOrder(periodDelta10, periodDelta);
        QWidget::setTabOrder(periodDelta, pushButton4);

        retranslateUi(BeatGraph);
        QObject::connect(pushButton4, SIGNAL(clicked()), BeatGraph, SLOT(setTempo()));
        QObject::connect(periodDelta, SIGNAL(valueChanged(int)), BeatGraph, SLOT(slantChanged()));
        QObject::connect(periodDelta10, SIGNAL(valueChanged(int)), BeatGraph, SLOT(slantChanged()));
        QObject::connect(bg_f1, SIGNAL(toggled(bool)), BeatGraph, SLOT(changeVisualisation()));
        QObject::connect(bg_gray, SIGNAL(toggled(bool)), BeatGraph, SLOT(changeVisualisation()));
        QObject::connect(bg_haar, SIGNAL(toggled(bool)), BeatGraph, SLOT(changeVisualisation()));

        QMetaObject::connectSlotsByName(BeatGraph);
    } // setupUi

    void retranslateUi(QWidget *BeatGraph)
    {
        BeatGraph->setWindowTitle(QApplication::translate("BeatGraph", "BeatGraphWidget", 0, QApplication::UnicodeUTF8));
        switcherLabel->setText(QString());
        zlabel->setText(QApplication::translate("BeatGraph", "Z ", 0, QApplication::UnicodeUTF8));
        xlabel->setText(QApplication::translate("BeatGraph", "X ", 0, QApplication::UnicodeUTF8));
        clabel->setText(QApplication::translate("BeatGraph", "C ", 0, QApplication::UnicodeUTF8));
        vlabel->setText(QApplication::translate("BeatGraph", "V ", 0, QApplication::UnicodeUTF8));
        nowlabel->setText(QApplication::translate("BeatGraph", "Now", 0, QApplication::UnicodeUTF8));
        bg_gray->setText(QApplication::translate("BeatGraph", "Gray", 0, QApplication::UnicodeUTF8));
        bg_f1->setText(QApplication::translate("BeatGraph", "Primary Bands", 0, QApplication::UnicodeUTF8));
        bg_haar->setText(QApplication::translate("BeatGraph", "Wavelet", 0, QApplication::UnicodeUTF8));
        pushButton4->setText(QApplication::translate("BeatGraph", "Set Tempo", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class BeatGraph: public Ui_BeatGraph {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_BEATGRAPH_H
