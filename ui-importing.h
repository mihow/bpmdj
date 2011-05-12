/********************************************************************************
** Form generated from reading UI file 'ui-importing.ui'
**
** Created: Thu May 12 14:15:37 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_IMPORTING_H
#define UI_2D_IMPORTING_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QTextBrowser>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ImportProgress
{
public:
    QGridLayout *gridLayout;
    QLabel *TextLabel1;
    QLabel *Log;
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout;
    QLabel *TextLabel2;
    QTextBrowser *created;
    QVBoxLayout *vboxLayout1;
    QLabel *label;
    QTextBrowser *ignored;

    void setupUi(QDialog *ImportProgress)
    {
        if (ImportProgress->objectName().isEmpty())
            ImportProgress->setObjectName(QString::fromUtf8("ImportProgress"));
        ImportProgress->resize(559, 299);
        ImportProgress->setSizeGripEnabled(true);
        gridLayout = new QGridLayout(ImportProgress);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        TextLabel1 = new QLabel(ImportProgress);
        TextLabel1->setObjectName(QString::fromUtf8("TextLabel1"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TextLabel1->sizePolicy().hasHeightForWidth());
        TextLabel1->setSizePolicy(sizePolicy);
        TextLabel1->setWordWrap(false);

        gridLayout->addWidget(TextLabel1, 0, 0, 1, 1);

        Log = new QLabel(ImportProgress);
        Log->setObjectName(QString::fromUtf8("Log"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(Log->sizePolicy().hasHeightForWidth());
        Log->setSizePolicy(sizePolicy1);
        Log->setWordWrap(true);

        gridLayout->addWidget(Log, 0, 1, 1, 1);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        vboxLayout = new QVBoxLayout();
        vboxLayout->setSpacing(6);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        TextLabel2 = new QLabel(ImportProgress);
        TextLabel2->setObjectName(QString::fromUtf8("TextLabel2"));
        TextLabel2->setWordWrap(false);

        vboxLayout->addWidget(TextLabel2);

        created = new QTextBrowser(ImportProgress);
        created->setObjectName(QString::fromUtf8("created"));

        vboxLayout->addWidget(created);


        hboxLayout->addLayout(vboxLayout);

        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setSpacing(6);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        label = new QLabel(ImportProgress);
        label->setObjectName(QString::fromUtf8("label"));

        vboxLayout1->addWidget(label);

        ignored = new QTextBrowser(ImportProgress);
        ignored->setObjectName(QString::fromUtf8("ignored"));

        vboxLayout1->addWidget(ignored);


        hboxLayout->addLayout(vboxLayout1);


        gridLayout->addLayout(hboxLayout, 1, 0, 1, 2);


        retranslateUi(ImportProgress);

        QMetaObject::connectSlotsByName(ImportProgress);
    } // setupUi

    void retranslateUi(QDialog *ImportProgress)
    {
        ImportProgress->setWindowTitle(QApplication::translate("ImportProgress", "Importing Songs", 0, QApplication::UnicodeUTF8));
        TextLabel1->setText(QApplication::translate("ImportProgress", "Scanning :", 0, QApplication::UnicodeUTF8));
        Log->setText(QString());
        TextLabel2->setText(QApplication::translate("ImportProgress", "Indexed", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ImportProgress", "Ignored", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ImportProgress: public Ui_ImportProgress {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_IMPORTING_H
