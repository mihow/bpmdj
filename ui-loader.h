/********************************************************************************
** Form generated from reading UI file 'ui-loader.ui'
**
** Created: Thu May 12 14:15:37 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_LOADER_H
#define UI_2D_LOADER_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_Loader
{
public:
    QGridLayout *gridLayout;

    void setupUi(QDialog *Loader)
    {
        if (Loader->objectName().isEmpty())
            Loader->setObjectName(QString::fromUtf8("Loader"));
        Loader->setEnabled(true);
        Loader->resize(400, 200);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Loader->sizePolicy().hasHeightForWidth());
        Loader->setSizePolicy(sizePolicy);
        Loader->setMinimumSize(QSize(400, 200));
        Loader->setMaximumSize(QSize(400, 200));
        Loader->setBaseSize(QSize(400, 200));
        QFont font;
        font.setPointSize(10);
        Loader->setFont(font);
        gridLayout = new QGridLayout(Loader);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setHorizontalSpacing(6);
        gridLayout->setVerticalSpacing(6);
        gridLayout->setContentsMargins(9, 9, 9, 9);

        retranslateUi(Loader);

        QMetaObject::connectSlotsByName(Loader);
    } // setupUi

    void retranslateUi(QDialog *Loader)
    {
        Loader->setWindowTitle(QApplication::translate("Loader", "Bpmdj", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Loader: public Ui_Loader {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_LOADER_H
