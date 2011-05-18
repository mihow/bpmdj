/********************************************************************************
** Form generated from reading UI file 'ui-freq-mapping.ui'
**
** Created: Wed May 18 22:34:36 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_FREQ_2D_MAPPING_H
#define UI_2D_FREQ_2D_MAPPING_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_FrequencyDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *frequency_map;
    QLabel *textLabel5;

    void setupUi(QDialog *FrequencyDialog)
    {
        if (FrequencyDialog->objectName().isEmpty())
            FrequencyDialog->setObjectName(QString::fromUtf8("FrequencyDialog"));
        FrequencyDialog->resize(420, 426);
        gridLayout = new QGridLayout(FrequencyDialog);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        frequency_map = new QLabel(FrequencyDialog);
        frequency_map->setObjectName(QString::fromUtf8("frequency_map"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frequency_map->sizePolicy().hasHeightForWidth());
        frequency_map->setSizePolicy(sizePolicy);
        frequency_map->setScaledContents(true);
        frequency_map->setWordWrap(false);

        gridLayout->addWidget(frequency_map, 0, 0, 1, 1);

        textLabel5 = new QLabel(FrequencyDialog);
        textLabel5->setObjectName(QString::fromUtf8("textLabel5"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(textLabel5->sizePolicy().hasHeightForWidth());
        textLabel5->setSizePolicy(sizePolicy1);
        QFont font;
        font.setPointSize(10);
        textLabel5->setFont(font);
        textLabel5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        textLabel5->setWordWrap(true);

        gridLayout->addWidget(textLabel5, 1, 0, 1, 1);


        retranslateUi(FrequencyDialog);

        QMetaObject::connectSlotsByName(FrequencyDialog);
    } // setupUi

    void retranslateUi(QDialog *FrequencyDialog)
    {
        FrequencyDialog->setWindowTitle(QApplication::translate("FrequencyDialog", "Frequency Shaping", 0, QApplication::UnicodeUTF8));
        textLabel5->setText(QApplication::translate("FrequencyDialog", "This graph presents the frequencyshaping needed to map the monitor to the main song. Horizontally 24 bark frequencies, vertically dB. Every green line is 3 dB. grey bars are missing frequencies in either monitor or main", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class FrequencyDialog: public Ui_FrequencyDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_FREQ_2D_MAPPING_H
