/********************************************************************************
** Form generated from reading UI file 'ui-clustering.ui'
**
** Created: Wed May 18 22:19:23 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_CLUSTERING_H
#define UI_2D_CLUSTERING_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include "metric-widget.h"

QT_BEGIN_NAMESPACE

class Ui_ClusterDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *textLabel1;
    MetricWidget *metrics;
    QFrame *line4;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *spacer13;
    QPushButton *pushButton27;
    QPushButton *pushButton28;

    void setupUi(QDialog *ClusterDialog)
    {
        if (ClusterDialog->objectName().isEmpty())
            ClusterDialog->setObjectName(QString::fromUtf8("ClusterDialog"));
        ClusterDialog->resize(382, 449);
        gridLayout = new QGridLayout(ClusterDialog);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        textLabel1 = new QLabel(ClusterDialog);
        textLabel1->setObjectName(QString::fromUtf8("textLabel1"));
        textLabel1->setAlignment(Qt::AlignVCenter);
        textLabel1->setWordWrap(true);

        gridLayout->addWidget(textLabel1, 0, 0, 1, 1);

        metrics = new MetricWidget(ClusterDialog);
        metrics->setObjectName(QString::fromUtf8("metrics"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(metrics->sizePolicy().hasHeightForWidth());
        metrics->setSizePolicy(sizePolicy);

        gridLayout->addWidget(metrics, 1, 0, 1, 1);

        line4 = new QFrame(ClusterDialog);
        line4->setObjectName(QString::fromUtf8("line4"));
        line4->setFrameShape(QFrame::HLine);
        line4->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line4, 2, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        spacer13 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(spacer13);

        pushButton27 = new QPushButton(ClusterDialog);
        pushButton27->setObjectName(QString::fromUtf8("pushButton27"));

        horizontalLayout->addWidget(pushButton27);

        pushButton28 = new QPushButton(ClusterDialog);
        pushButton28->setObjectName(QString::fromUtf8("pushButton28"));

        horizontalLayout->addWidget(pushButton28);


        gridLayout->addLayout(horizontalLayout, 3, 0, 1, 1);


        retranslateUi(ClusterDialog);
        QObject::connect(pushButton27, SIGNAL(clicked()), ClusterDialog, SLOT(accept()));
        QObject::connect(pushButton28, SIGNAL(clicked()), ClusterDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ClusterDialog);
    } // setupUi

    void retranslateUi(QDialog *ClusterDialog)
    {
        ClusterDialog->setWindowTitle(QApplication::translate("ClusterDialog", "Cluster Metrics", 0, QApplication::UnicodeUTF8));
        textLabel1->setText(QApplication::translate("ClusterDialog", "The clustering only works on elements that have been properly analyzed", 0, QApplication::UnicodeUTF8));
        pushButton27->setText(QApplication::translate("ClusterDialog", "Start Clustering", 0, QApplication::UnicodeUTF8));
        pushButton28->setText(QApplication::translate("ClusterDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ClusterDialog: public Ui_ClusterDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_CLUSTERING_H
