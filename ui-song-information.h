/********************************************************************************
** Form generated from reading UI file 'ui-song-information.ui'
**
** Created: Mon May 16 18:44:31 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_SONG_2D_INFORMATION_H
#define UI_2D_SONG_2D_INFORMATION_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "capacity-widget.h"

QT_BEGIN_NAMESPACE

class Ui_SongInformation
{
public:
    QGridLayout *gridLayout;
    QSpacerItem *spacerItem;
    QHBoxLayout *hboxLayout;
    QFrame *line4;
    QVBoxLayout *vboxLayout;
    QSpacerItem *spacerItem1;
    QLabel *textLabel3_2;
    CapacityWidget *capacity;
    QHBoxLayout *hboxLayout1;
    QVBoxLayout *vboxLayout1;
    QLabel *TextLabel1;
    QLabel *TextLabel5_2;
    QLabel *TextLabel5_3;
    QVBoxLayout *vboxLayout2;
    QLabel *idxLabel;
    QLabel *md5Label;
    QLabel *location;
    QHBoxLayout *hboxLayout2;
    QVBoxLayout *vboxLayout3;
    QLabel *textLabel1;
    QLabel *textLabel2;
    QLabel *textLabel3;
    QLabel *textLabel4;
    QLabel *TextLabel2_2;
    QVBoxLayout *vboxLayout4;
    QLineEdit *titleEdit;
    QLineEdit *authorEdit;
    QLineEdit *remixEdit;
    QLineEdit *versionEdit;
    QLineEdit *tagEdit;
    QHBoxLayout *hboxLayout3;
    QSpacerItem *spacerItem2;
    QPushButton *save;

    void setupUi(QWidget *SongInformation)
    {
        if (SongInformation->objectName().isEmpty())
            SongInformation->setObjectName(QString::fromUtf8("SongInformation"));
        SongInformation->resize(469, 280);
        gridLayout = new QGridLayout(SongInformation);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        spacerItem = new QSpacerItem(20, 31, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacerItem, 0, 0, 1, 1);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        line4 = new QFrame(SongInformation);
        line4->setObjectName(QString::fromUtf8("line4"));
        line4->setFrameShape(QFrame::VLine);
        line4->setFrameShadow(QFrame::Sunken);
        line4->setFrameShape(QFrame::VLine);

        hboxLayout->addWidget(line4);

        vboxLayout = new QVBoxLayout();
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        spacerItem1 = new QSpacerItem(20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout->addItem(spacerItem1);

        textLabel3_2 = new QLabel(SongInformation);
        textLabel3_2->setObjectName(QString::fromUtf8("textLabel3_2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(textLabel3_2->sizePolicy().hasHeightForWidth());
        textLabel3_2->setSizePolicy(sizePolicy);
        textLabel3_2->setWordWrap(false);

        vboxLayout->addWidget(textLabel3_2);

        capacity = new CapacityWidget(SongInformation);
        capacity->setObjectName(QString::fromUtf8("capacity"));

        vboxLayout->addWidget(capacity);


        hboxLayout->addLayout(vboxLayout);


        gridLayout->addLayout(hboxLayout, 0, 1, 3, 1);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        TextLabel1 = new QLabel(SongInformation);
        TextLabel1->setObjectName(QString::fromUtf8("TextLabel1"));
        QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(TextLabel1->sizePolicy().hasHeightForWidth());
        TextLabel1->setSizePolicy(sizePolicy1);
        TextLabel1->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        TextLabel1->setWordWrap(false);

        vboxLayout1->addWidget(TextLabel1);

        TextLabel5_2 = new QLabel(SongInformation);
        TextLabel5_2->setObjectName(QString::fromUtf8("TextLabel5_2"));
        sizePolicy1.setHeightForWidth(TextLabel5_2->sizePolicy().hasHeightForWidth());
        TextLabel5_2->setSizePolicy(sizePolicy1);
        TextLabel5_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        TextLabel5_2->setWordWrap(false);

        vboxLayout1->addWidget(TextLabel5_2);

        TextLabel5_3 = new QLabel(SongInformation);
        TextLabel5_3->setObjectName(QString::fromUtf8("TextLabel5_3"));
        sizePolicy1.setHeightForWidth(TextLabel5_3->sizePolicy().hasHeightForWidth());
        TextLabel5_3->setSizePolicy(sizePolicy1);
        TextLabel5_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        TextLabel5_3->setWordWrap(false);

        vboxLayout1->addWidget(TextLabel5_3);


        hboxLayout1->addLayout(vboxLayout1);

        vboxLayout2 = new QVBoxLayout();
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        idxLabel = new QLabel(SongInformation);
        idxLabel->setObjectName(QString::fromUtf8("idxLabel"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(idxLabel->sizePolicy().hasHeightForWidth());
        idxLabel->setSizePolicy(sizePolicy2);
        idxLabel->setWordWrap(true);

        vboxLayout2->addWidget(idxLabel);

        md5Label = new QLabel(SongInformation);
        md5Label->setObjectName(QString::fromUtf8("md5Label"));
        sizePolicy2.setHeightForWidth(md5Label->sizePolicy().hasHeightForWidth());
        md5Label->setSizePolicy(sizePolicy2);

        vboxLayout2->addWidget(md5Label);

        location = new QLabel(SongInformation);
        location->setObjectName(QString::fromUtf8("location"));
        location->setWordWrap(true);

        vboxLayout2->addWidget(location);


        hboxLayout1->addLayout(vboxLayout2);


        gridLayout->addLayout(hboxLayout1, 1, 0, 1, 1);

        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setSpacing(6);
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        hboxLayout2->setContentsMargins(0, 0, 0, 0);
        vboxLayout3 = new QVBoxLayout();
        vboxLayout3->setSpacing(2);
        vboxLayout3->setObjectName(QString::fromUtf8("vboxLayout3"));
        vboxLayout3->setContentsMargins(0, 0, 0, 0);
        textLabel1 = new QLabel(SongInformation);
        textLabel1->setObjectName(QString::fromUtf8("textLabel1"));
        textLabel1->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        textLabel1->setWordWrap(false);

        vboxLayout3->addWidget(textLabel1);

        textLabel2 = new QLabel(SongInformation);
        textLabel2->setObjectName(QString::fromUtf8("textLabel2"));
        textLabel2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        textLabel2->setWordWrap(false);

        vboxLayout3->addWidget(textLabel2);

        textLabel3 = new QLabel(SongInformation);
        textLabel3->setObjectName(QString::fromUtf8("textLabel3"));
        textLabel3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        textLabel3->setWordWrap(false);

        vboxLayout3->addWidget(textLabel3);

        textLabel4 = new QLabel(SongInformation);
        textLabel4->setObjectName(QString::fromUtf8("textLabel4"));
        textLabel4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        textLabel4->setWordWrap(false);

        vboxLayout3->addWidget(textLabel4);

        TextLabel2_2 = new QLabel(SongInformation);
        TextLabel2_2->setObjectName(QString::fromUtf8("TextLabel2_2"));
        TextLabel2_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        TextLabel2_2->setWordWrap(false);

        vboxLayout3->addWidget(TextLabel2_2);


        hboxLayout2->addLayout(vboxLayout3);

        vboxLayout4 = new QVBoxLayout();
        vboxLayout4->setSpacing(2);
        vboxLayout4->setObjectName(QString::fromUtf8("vboxLayout4"));
        vboxLayout4->setContentsMargins(0, 0, 0, 0);
        titleEdit = new QLineEdit(SongInformation);
        titleEdit->setObjectName(QString::fromUtf8("titleEdit"));

        vboxLayout4->addWidget(titleEdit);

        authorEdit = new QLineEdit(SongInformation);
        authorEdit->setObjectName(QString::fromUtf8("authorEdit"));

        vboxLayout4->addWidget(authorEdit);

        remixEdit = new QLineEdit(SongInformation);
        remixEdit->setObjectName(QString::fromUtf8("remixEdit"));

        vboxLayout4->addWidget(remixEdit);

        versionEdit = new QLineEdit(SongInformation);
        versionEdit->setObjectName(QString::fromUtf8("versionEdit"));

        vboxLayout4->addWidget(versionEdit);

        tagEdit = new QLineEdit(SongInformation);
        tagEdit->setObjectName(QString::fromUtf8("tagEdit"));

        vboxLayout4->addWidget(tagEdit);


        hboxLayout2->addLayout(vboxLayout4);


        gridLayout->addLayout(hboxLayout2, 2, 0, 1, 1);

        hboxLayout3 = new QHBoxLayout();
        hboxLayout3->setSpacing(6);
        hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
        hboxLayout3->setContentsMargins(0, 0, 0, 0);
        spacerItem2 = new QSpacerItem(450, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout3->addItem(spacerItem2);

        save = new QPushButton(SongInformation);
        save->setObjectName(QString::fromUtf8("save"));

        hboxLayout3->addWidget(save);


        gridLayout->addLayout(hboxLayout3, 3, 0, 1, 2);

        QWidget::setTabOrder(titleEdit, authorEdit);
        QWidget::setTabOrder(authorEdit, remixEdit);
        QWidget::setTabOrder(remixEdit, versionEdit);
        QWidget::setTabOrder(versionEdit, tagEdit);
        QWidget::setTabOrder(tagEdit, save);

        retranslateUi(SongInformation);

        QMetaObject::connectSlotsByName(SongInformation);
    } // setupUi

    void retranslateUi(QWidget *SongInformation)
    {
        SongInformation->setWindowTitle(QApplication::translate("SongInformation", "Form", 0, QApplication::UnicodeUTF8));
        textLabel3_2->setText(QApplication::translate("SongInformation", "Disable", 0, QApplication::UnicodeUTF8));
        TextLabel1->setText(QApplication::translate("SongInformation", "meta =", 0, QApplication::UnicodeUTF8));
        TextLabel5_2->setText(QApplication::translate("SongInformation", "md5sum =", 0, QApplication::UnicodeUTF8));
        TextLabel5_3->setText(QApplication::translate("SongInformation", "Location =", 0, QApplication::UnicodeUTF8));
        idxLabel->setText(QApplication::translate("SongInformation", "<meta data location>", 0, QApplication::UnicodeUTF8));
        md5Label->setText(QApplication::translate("SongInformation", "<file md5sum>", 0, QApplication::UnicodeUTF8));
        location->setText(QApplication::translate("SongInformation", "<file location>", 0, QApplication::UnicodeUTF8));
        textLabel1->setText(QApplication::translate("SongInformation", "title =", 0, QApplication::UnicodeUTF8));
        textLabel2->setText(QApplication::translate("SongInformation", "author =", 0, QApplication::UnicodeUTF8));
        textLabel3->setText(QApplication::translate("SongInformation", "remix =", 0, QApplication::UnicodeUTF8));
        textLabel4->setText(QApplication::translate("SongInformation", "version =", 0, QApplication::UnicodeUTF8));
        TextLabel2_2->setText(QApplication::translate("SongInformation", "tags =", 0, QApplication::UnicodeUTF8));
        save->setText(QApplication::translate("SongInformation", "Save", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SongInformation: public Ui_SongInformation {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_SONG_2D_INFORMATION_H
