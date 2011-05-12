/********************************************************************************
** Form generated from reading UI file 'ui-stats.ui'
**
** Created: Thu May 12 15:19:16 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_STATS_H
#define UI_2D_STATS_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QTabWidget>
#include <QtGui/QTextBrowser>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Statistics
{
public:
    QGridLayout *gridLayout;
    QLabel *bodysize;
    QPushButton *calculate;
    QLabel *status;
    QTabWidget *tabWidget2;
    QWidget *tab;
    QGridLayout *gridLayout1;
    QLabel *spectrum;
    QTextBrowser *spectrum_data;
    QLabel *textLabel5;

    void setupUi(QDialog *Statistics)
    {
        if (Statistics->objectName().isEmpty())
            Statistics->setObjectName(QString::fromUtf8("Statistics"));
        Statistics->resize(613, 441);
        gridLayout = new QGridLayout(Statistics);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        bodysize = new QLabel(Statistics);
        bodysize->setObjectName(QString::fromUtf8("bodysize"));
        bodysize->setWordWrap(false);

        gridLayout->addWidget(bodysize, 1, 0, 1, 1);

        calculate = new QPushButton(Statistics);
        calculate->setObjectName(QString::fromUtf8("calculate"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(calculate->sizePolicy().hasHeightForWidth());
        calculate->setSizePolicy(sizePolicy);

        gridLayout->addWidget(calculate, 1, 1, 2, 1);

        status = new QLabel(Statistics);
        status->setObjectName(QString::fromUtf8("status"));
        status->setWordWrap(false);

        gridLayout->addWidget(status, 2, 0, 1, 1);

        tabWidget2 = new QTabWidget(Statistics);
        tabWidget2->setObjectName(QString::fromUtf8("tabWidget2"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout1 = new QGridLayout(tab);
        gridLayout1->setSpacing(6);
        gridLayout1->setContentsMargins(11, 11, 11, 11);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        spectrum = new QLabel(tab);
        spectrum->setObjectName(QString::fromUtf8("spectrum"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(spectrum->sizePolicy().hasHeightForWidth());
        spectrum->setSizePolicy(sizePolicy1);
        spectrum->setScaledContents(true);
        spectrum->setWordWrap(false);

        gridLayout1->addWidget(spectrum, 0, 0, 1, 1);

        spectrum_data = new QTextBrowser(tab);
        spectrum_data->setObjectName(QString::fromUtf8("spectrum_data"));
        QSizePolicy sizePolicy2(QSizePolicy::Maximum, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(spectrum_data->sizePolicy().hasHeightForWidth());
        spectrum_data->setSizePolicy(sizePolicy2);

        gridLayout1->addWidget(spectrum_data, 0, 1, 2, 1);

        textLabel5 = new QLabel(tab);
        textLabel5->setObjectName(QString::fromUtf8("textLabel5"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(textLabel5->sizePolicy().hasHeightForWidth());
        textLabel5->setSizePolicy(sizePolicy3);
        QFont font;
        font.setPointSize(10);
        textLabel5->setFont(font);
        textLabel5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        textLabel5->setWordWrap(true);

        gridLayout1->addWidget(textLabel5, 1, 0, 1, 1);

        tabWidget2->addTab(tab, QString());

        gridLayout->addWidget(tabWidget2, 0, 0, 1, 2);


        retranslateUi(Statistics);

        tabWidget2->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(Statistics);
    } // setupUi

    void retranslateUi(QDialog *Statistics)
    {
        Statistics->setWindowTitle(QApplication::translate("Statistics", "BpmDj Statistics", 0, QApplication::UnicodeUTF8));
        bodysize->setText(QString());
        calculate->setText(QApplication::translate("Statistics", "Calculate", 0, QApplication::UnicodeUTF8));
        status->setText(QString());
        textLabel5->setText(QApplication::translate("Statistics", "This graph shows the mean energy/frequency band. Every green line is 3 dB. The length of a grey smear is the standarddeviation on that band.", 0, QApplication::UnicodeUTF8));
        tabWidget2->setTabText(tabWidget2->indexOf(tab), QApplication::translate("Statistics", "Spectrum", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Statistics: public Ui_Statistics {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_STATS_H
