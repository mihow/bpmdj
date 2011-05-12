/********************************************************************************
** Form generated from reading UI file 'ui-album.ui'
**
** Created: Thu May 12 14:15:37 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_ALBUM_H
#define UI_2D_ALBUM_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_AlbumBox
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *hboxLayout;
    QLabel *TextLabel1_2;
    QComboBox *album;
    QPushButton *PushButton2;
    QPushButton *PushButton17;
    QSpinBox *number;

    void setupUi(QDialog *AlbumBox)
    {
        if (AlbumBox->objectName().isEmpty())
            AlbumBox->setObjectName(QString::fromUtf8("AlbumBox"));
        AlbumBox->resize(411, 106);
        AlbumBox->setFocusPolicy(Qt::NoFocus);
        AlbumBox->setSizeGripEnabled(false);
        gridLayout = new QGridLayout(AlbumBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setHorizontalSpacing(6);
        gridLayout->setVerticalSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        TextLabel1_2 = new QLabel(AlbumBox);
        TextLabel1_2->setObjectName(QString::fromUtf8("TextLabel1_2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TextLabel1_2->sizePolicy().hasHeightForWidth());
        TextLabel1_2->setSizePolicy(sizePolicy);
        TextLabel1_2->setWordWrap(false);

        hboxLayout->addWidget(TextLabel1_2);

        album = new QComboBox(AlbumBox);
        album->setObjectName(QString::fromUtf8("album"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(album->sizePolicy().hasHeightForWidth());
        album->setSizePolicy(sizePolicy1);
        album->setEditable(true);

        hboxLayout->addWidget(album);


        gridLayout->addLayout(hboxLayout, 0, 0, 1, 3);

        PushButton2 = new QPushButton(AlbumBox);
        PushButton2->setObjectName(QString::fromUtf8("PushButton2"));

        gridLayout->addWidget(PushButton2, 1, 2, 1, 1);

        PushButton17 = new QPushButton(AlbumBox);
        PushButton17->setObjectName(QString::fromUtf8("PushButton17"));

        gridLayout->addWidget(PushButton17, 1, 1, 1, 1);

        number = new QSpinBox(AlbumBox);
        number->setObjectName(QString::fromUtf8("number"));
        number->setMinimum(-1);
        number->setValue(-1);

        gridLayout->addWidget(number, 1, 0, 1, 1);

        QWidget::setTabOrder(album, number);
        QWidget::setTabOrder(number, PushButton17);
        QWidget::setTabOrder(PushButton17, PushButton2);

        retranslateUi(AlbumBox);
        QObject::connect(PushButton17, SIGNAL(clicked()), AlbumBox, SLOT(accept()));
        QObject::connect(PushButton2, SIGNAL(clicked()), AlbumBox, SLOT(reject()));

        QMetaObject::connectSlotsByName(AlbumBox);
    } // setupUi

    void retranslateUi(QDialog *AlbumBox)
    {
        AlbumBox->setWindowTitle(QApplication::translate("AlbumBox", "Enter Album", 0, QApplication::UnicodeUTF8));
        TextLabel1_2->setText(QApplication::translate("AlbumBox", "Album", 0, QApplication::UnicodeUTF8));
        PushButton2->setText(QApplication::translate("AlbumBox", "Cancel", 0, QApplication::UnicodeUTF8));
        PushButton17->setText(QApplication::translate("AlbumBox", "Okay !", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class AlbumBox: public Ui_AlbumBox {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_ALBUM_H
