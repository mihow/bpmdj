/********************************************************************************
** Form generated from reading UI file 'ui-logs.ui'
**
** Created: Mon May 16 18:44:31 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_LOGS_H
#define UI_2D_LOGS_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_LogViewer
{
public:
    QGridLayout *gridLayout;
    QListWidget *list;
    QTextBrowser *text;
    QSpacerItem *spacerItem;
    QPushButton *pushButton49;

    void setupUi(QDialog *LogViewer)
    {
        if (LogViewer->objectName().isEmpty())
            LogViewer->setObjectName(QString::fromUtf8("LogViewer"));
        LogViewer->resize(674, 444);
        LogViewer->setModal(true);
        gridLayout = new QGridLayout(LogViewer);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        list = new QListWidget(LogViewer);
        list->setObjectName(QString::fromUtf8("list"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(list->sizePolicy().hasHeightForWidth());
        list->setSizePolicy(sizePolicy);

        gridLayout->addWidget(list, 0, 0, 1, 1);

        text = new QTextBrowser(LogViewer);
        text->setObjectName(QString::fromUtf8("text"));

        gridLayout->addWidget(text, 0, 1, 1, 2);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(spacerItem, 1, 0, 1, 2);

        pushButton49 = new QPushButton(LogViewer);
        pushButton49->setObjectName(QString::fromUtf8("pushButton49"));

        gridLayout->addWidget(pushButton49, 1, 2, 1, 1);


        retranslateUi(LogViewer);
        QObject::connect(pushButton49, SIGNAL(clicked()), LogViewer, SLOT(accept()));

        QMetaObject::connectSlotsByName(LogViewer);
    } // setupUi

    void retranslateUi(QDialog *LogViewer)
    {
        LogViewer->setWindowTitle(QApplication::translate("LogViewer", "Log Viewer", 0, QApplication::UnicodeUTF8));
        pushButton49->setText(QApplication::translate("LogViewer", "Ok", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class LogViewer: public Ui_LogViewer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_LOGS_H
