/********************************************************************************
** Form generated from reading UI file 'ui-capacity.ui'
**
** Created: Thu May 12 14:15:37 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_CAPACITY_H
#define UI_2D_CAPACITY_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CapacityWidget
{
public:
    QGridLayout *gridLayout;
    QCheckBox *mplayer1pre6;
    QCheckBox *mplayer1pre7;
    QCheckBox *ogg123;
    QCheckBox *mpg321;
    QCheckBox *mpg123059r;

    void setupUi(QWidget *CapacityWidget)
    {
        if (CapacityWidget->objectName().isEmpty())
            CapacityWidget->setObjectName(QString::fromUtf8("CapacityWidget"));
        CapacityWidget->resize(268, 115);
        gridLayout = new QGridLayout(CapacityWidget);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        mplayer1pre6 = new QCheckBox(CapacityWidget);
        mplayer1pre6->setObjectName(QString::fromUtf8("mplayer1pre6"));
        mplayer1pre6->setAutoFillBackground(true);

        gridLayout->addWidget(mplayer1pre6, 3, 0, 1, 1);

        mplayer1pre7 = new QCheckBox(CapacityWidget);
        mplayer1pre7->setObjectName(QString::fromUtf8("mplayer1pre7"));
        mplayer1pre7->setAutoFillBackground(true);

        gridLayout->addWidget(mplayer1pre7, 4, 0, 1, 1);

        ogg123 = new QCheckBox(CapacityWidget);
        ogg123->setObjectName(QString::fromUtf8("ogg123"));
        ogg123->setAutoFillBackground(true);

        gridLayout->addWidget(ogg123, 2, 0, 1, 1);

        mpg321 = new QCheckBox(CapacityWidget);
        mpg321->setObjectName(QString::fromUtf8("mpg321"));
        mpg321->setAutoFillBackground(true);

        gridLayout->addWidget(mpg321, 1, 0, 1, 1);

        mpg123059r = new QCheckBox(CapacityWidget);
        mpg123059r->setObjectName(QString::fromUtf8("mpg123059r"));
        mpg123059r->setAutoFillBackground(true);

        gridLayout->addWidget(mpg123059r, 0, 0, 1, 1);

        QWidget::setTabOrder(mpg123059r, mpg321);
        QWidget::setTabOrder(mpg321, ogg123);
        QWidget::setTabOrder(ogg123, mplayer1pre6);
        QWidget::setTabOrder(mplayer1pre6, mplayer1pre7);

        retranslateUi(CapacityWidget);

        QMetaObject::connectSlotsByName(CapacityWidget);
    } // setupUi

    void retranslateUi(QWidget *CapacityWidget)
    {
        CapacityWidget->setWindowTitle(QApplication::translate("CapacityWidget", "Form3", 0, QApplication::UnicodeUTF8));
        mplayer1pre6->setText(QApplication::translate("CapacityWidget", "mplayer 1.0pre6-3.3.5", 0, QApplication::UnicodeUTF8));
        mplayer1pre7->setText(QApplication::translate("CapacityWidget", "mplayer 1.0pre7-3.3.5 or 1.0rc1-4.1.2", 0, QApplication::UnicodeUTF8));
        ogg123->setText(QApplication::translate("CapacityWidget", "ogg123", 0, QApplication::UnicodeUTF8));
        mpg321->setText(QApplication::translate("CapacityWidget", "mpg321", 0, QApplication::UnicodeUTF8));
        mpg123059r->setText(QApplication::translate("CapacityWidget", "mpg123 0.59r/0.60/1.4.3/1.10.0", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CapacityWidget: public Ui_CapacityWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_CAPACITY_H
