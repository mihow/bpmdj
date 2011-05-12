/********************************************************************************
** Form generated from reading UI file 'ui-installremotes.ui'
**
** Created: Thu May 12 14:15:37 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_INSTALLREMOTES_H
#define UI_2D_INSTALLREMOTES_H

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
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextBrowser>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_InstallRemotes
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QTreeWidget *hosts;
    QLabel *textLabel1;
    QTextBrowser *output;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *spacerItem;
    QPushButton *okbutton;

    void setupUi(QDialog *InstallRemotes)
    {
        if (InstallRemotes->objectName().isEmpty())
            InstallRemotes->setObjectName(QString::fromUtf8("InstallRemotes"));
        InstallRemotes->resize(541, 603);
        gridLayout = new QGridLayout(InstallRemotes);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        hosts = new QTreeWidget(InstallRemotes);
        hosts->setObjectName(QString::fromUtf8("hosts"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(hosts->sizePolicy().hasHeightForWidth());
        hosts->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(hosts);

        textLabel1 = new QLabel(InstallRemotes);
        textLabel1->setObjectName(QString::fromUtf8("textLabel1"));
        textLabel1->setWordWrap(false);

        verticalLayout->addWidget(textLabel1);

        output = new QTextBrowser(InstallRemotes);
        output->setObjectName(QString::fromUtf8("output"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(output->sizePolicy().hasHeightForWidth());
        output->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(output);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(spacerItem);

        okbutton = new QPushButton(InstallRemotes);
        okbutton->setObjectName(QString::fromUtf8("okbutton"));
        okbutton->setEnabled(false);

        horizontalLayout->addWidget(okbutton);


        verticalLayout->addLayout(horizontalLayout);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(InstallRemotes);
        QObject::connect(okbutton, SIGNAL(clicked()), InstallRemotes, SLOT(accept()));

        QMetaObject::connectSlotsByName(InstallRemotes);
    } // setupUi

    void retranslateUi(QDialog *InstallRemotes)
    {
        InstallRemotes->setWindowTitle(QApplication::translate("InstallRemotes", "Installing BpmPlay at Remote Host", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = hosts->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("InstallRemotes", "Host", 0, QApplication::UnicodeUTF8));
        textLabel1->setText(QApplication::translate("InstallRemotes", "Ouput", 0, QApplication::UnicodeUTF8));
        okbutton->setText(QApplication::translate("InstallRemotes", "Ok", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class InstallRemotes: public Ui_InstallRemotes {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_INSTALLREMOTES_H
