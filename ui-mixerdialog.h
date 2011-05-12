/********************************************************************************
** Form generated from reading UI file 'ui-mixerdialog.ui'
**
** Created: Thu May 12 17:50:02 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_MIXERDIALOG_H
#define UI_2D_MIXERDIALOG_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_MixerDialog
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout;
    QLabel *textLabel1;
    QHBoxLayout *hboxLayout1;
    QVBoxLayout *vboxLayout1;
    QLabel *Ch1MainText;
    QSlider *Ch1Main;
    QVBoxLayout *vboxLayout2;
    QLabel *Ch2MainText;
    QSlider *Ch2Main;
    QFrame *line2;
    QVBoxLayout *vboxLayout3;
    QLabel *textLabel2;
    QHBoxLayout *hboxLayout2;
    QVBoxLayout *vboxLayout4;
    QLabel *Ch1MonitorText;
    QSlider *Ch1Monitor;
    QVBoxLayout *vboxLayout5;
    QLabel *Ch2MonitorText;
    QSlider *Ch2Monitor;
    QFrame *line2_2;
    QVBoxLayout *vboxLayout6;
    QFrame *line3;
    QHBoxLayout *hboxLayout3;
    QLabel *textLabel1_3;
    QLabel *textLabel1_3_4;
    QLabel *textLabel1_3_3;
    QLabel *textLabel1_3_2;
    QHBoxLayout *hboxLayout4;
    QComboBox *modebox;
    QPushButton *pushButton2;
    QHBoxLayout *hboxLayout5;
    QPushButton *syncbutton;
    QPushButton *balanced;
    QHBoxLayout *hboxLayout6;
    QVBoxLayout *vboxLayout7;
    QLabel *textLabel1_2;
    QLabel *textLabel3_3;
    QLabel *textLabel3_3_2;
    QLabel *textLabel4_3;
    QVBoxLayout *vboxLayout8;
    QLabel *latency_audio;
    QLabel *latency_buffer;
    QLabel *latency_difference;
    QLabel *synchronisation;
    QSpacerItem *spacer6;

    void setupUi(QDialog *MixerDialog)
    {
        if (MixerDialog->objectName().isEmpty())
            MixerDialog->setObjectName(QString::fromUtf8("MixerDialog"));
        MixerDialog->resize(466, 241);
        gridLayout = new QGridLayout(MixerDialog);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        vboxLayout = new QVBoxLayout();
        vboxLayout->setSpacing(6);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        textLabel1 = new QLabel(MixerDialog);
        textLabel1->setObjectName(QString::fromUtf8("textLabel1"));
        textLabel1->setWordWrap(false);

        vboxLayout->addWidget(textLabel1);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(6);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setSpacing(0);
        vboxLayout1->setContentsMargins(0, 0, 0, 0);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        Ch1MainText = new QLabel(MixerDialog);
        Ch1MainText->setObjectName(QString::fromUtf8("Ch1MainText"));
        Ch1MainText->setWordWrap(false);

        vboxLayout1->addWidget(Ch1MainText);

        Ch1Main = new QSlider(MixerDialog);
        Ch1Main->setObjectName(QString::fromUtf8("Ch1Main"));
        Ch1Main->setMinimum(-128);
        Ch1Main->setMaximum(0);
        Ch1Main->setValue(-128);
        Ch1Main->setOrientation(Qt::Vertical);

        vboxLayout1->addWidget(Ch1Main);


        hboxLayout1->addLayout(vboxLayout1);

        vboxLayout2 = new QVBoxLayout();
        vboxLayout2->setSpacing(0);
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        Ch2MainText = new QLabel(MixerDialog);
        Ch2MainText->setObjectName(QString::fromUtf8("Ch2MainText"));
        Ch2MainText->setWordWrap(false);

        vboxLayout2->addWidget(Ch2MainText);

        Ch2Main = new QSlider(MixerDialog);
        Ch2Main->setObjectName(QString::fromUtf8("Ch2Main"));
        Ch2Main->setMinimum(-128);
        Ch2Main->setMaximum(0);
        Ch2Main->setValue(-128);
        Ch2Main->setOrientation(Qt::Vertical);

        vboxLayout2->addWidget(Ch2Main);


        hboxLayout1->addLayout(vboxLayout2);


        vboxLayout->addLayout(hboxLayout1);


        hboxLayout->addLayout(vboxLayout);

        line2 = new QFrame(MixerDialog);
        line2->setObjectName(QString::fromUtf8("line2"));
        line2->setFrameShape(QFrame::VLine);
        line2->setFrameShadow(QFrame::Sunken);

        hboxLayout->addWidget(line2);

        vboxLayout3 = new QVBoxLayout();
        vboxLayout3->setSpacing(6);
        vboxLayout3->setObjectName(QString::fromUtf8("vboxLayout3"));
        textLabel2 = new QLabel(MixerDialog);
        textLabel2->setObjectName(QString::fromUtf8("textLabel2"));
        textLabel2->setWordWrap(false);

        vboxLayout3->addWidget(textLabel2);

        hboxLayout2 = new QHBoxLayout();
        hboxLayout2->setSpacing(6);
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        vboxLayout4 = new QVBoxLayout();
        vboxLayout4->setSpacing(0);
        vboxLayout4->setObjectName(QString::fromUtf8("vboxLayout4"));
        Ch1MonitorText = new QLabel(MixerDialog);
        Ch1MonitorText->setObjectName(QString::fromUtf8("Ch1MonitorText"));
        Ch1MonitorText->setWordWrap(false);

        vboxLayout4->addWidget(Ch1MonitorText);

        Ch1Monitor = new QSlider(MixerDialog);
        Ch1Monitor->setObjectName(QString::fromUtf8("Ch1Monitor"));
        Ch1Monitor->setMinimum(-128);
        Ch1Monitor->setMaximum(0);
        Ch1Monitor->setValue(-128);
        Ch1Monitor->setOrientation(Qt::Vertical);

        vboxLayout4->addWidget(Ch1Monitor);


        hboxLayout2->addLayout(vboxLayout4);

        vboxLayout5 = new QVBoxLayout();
        vboxLayout5->setSpacing(0);
        vboxLayout5->setContentsMargins(0, 0, 0, 0);
        vboxLayout5->setObjectName(QString::fromUtf8("vboxLayout5"));
        Ch2MonitorText = new QLabel(MixerDialog);
        Ch2MonitorText->setObjectName(QString::fromUtf8("Ch2MonitorText"));
        Ch2MonitorText->setWordWrap(false);

        vboxLayout5->addWidget(Ch2MonitorText);

        Ch2Monitor = new QSlider(MixerDialog);
        Ch2Monitor->setObjectName(QString::fromUtf8("Ch2Monitor"));
        Ch2Monitor->setMinimum(-128);
        Ch2Monitor->setMaximum(0);
        Ch2Monitor->setValue(-128);
        Ch2Monitor->setOrientation(Qt::Vertical);

        vboxLayout5->addWidget(Ch2Monitor);


        hboxLayout2->addLayout(vboxLayout5);


        vboxLayout3->addLayout(hboxLayout2);


        hboxLayout->addLayout(vboxLayout3);


        gridLayout->addLayout(hboxLayout, 0, 0, 2, 1);

        line2_2 = new QFrame(MixerDialog);
        line2_2->setObjectName(QString::fromUtf8("line2_2"));
        line2_2->setFrameShape(QFrame::VLine);
        line2_2->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line2_2, 0, 1, 2, 2);

        vboxLayout6 = new QVBoxLayout();
        vboxLayout6->setSpacing(0);
        vboxLayout6->setContentsMargins(0, 0, 0, 0);
        vboxLayout6->setObjectName(QString::fromUtf8("vboxLayout6"));
        line3 = new QFrame(MixerDialog);
        line3->setObjectName(QString::fromUtf8("line3"));
        line3->setFrameShape(QFrame::HLine);
        line3->setFrameShadow(QFrame::Sunken);

        vboxLayout6->addWidget(line3);

        hboxLayout3 = new QHBoxLayout();
        hboxLayout3->setSpacing(0);
        hboxLayout3->setContentsMargins(0, 0, 0, 0);
        hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
        textLabel1_3 = new QLabel(MixerDialog);
        textLabel1_3->setObjectName(QString::fromUtf8("textLabel1_3"));
        textLabel1_3->setAlignment(Qt::AlignCenter);
        textLabel1_3->setWordWrap(false);

        hboxLayout3->addWidget(textLabel1_3);

        textLabel1_3_4 = new QLabel(MixerDialog);
        textLabel1_3_4->setObjectName(QString::fromUtf8("textLabel1_3_4"));
        textLabel1_3_4->setAlignment(Qt::AlignCenter);
        textLabel1_3_4->setWordWrap(false);

        hboxLayout3->addWidget(textLabel1_3_4);

        textLabel1_3_3 = new QLabel(MixerDialog);
        textLabel1_3_3->setObjectName(QString::fromUtf8("textLabel1_3_3"));
        textLabel1_3_3->setAlignment(Qt::AlignCenter);
        textLabel1_3_3->setWordWrap(false);

        hboxLayout3->addWidget(textLabel1_3_3);

        textLabel1_3_2 = new QLabel(MixerDialog);
        textLabel1_3_2->setObjectName(QString::fromUtf8("textLabel1_3_2"));
        textLabel1_3_2->setAlignment(Qt::AlignCenter);
        textLabel1_3_2->setWordWrap(false);

        hboxLayout3->addWidget(textLabel1_3_2);


        vboxLayout6->addLayout(hboxLayout3);


        gridLayout->addLayout(vboxLayout6, 2, 0, 1, 5);

        hboxLayout4 = new QHBoxLayout();
        hboxLayout4->setSpacing(6);
        hboxLayout4->setObjectName(QString::fromUtf8("hboxLayout4"));
        modebox = new QComboBox(MixerDialog);
        modebox->setObjectName(QString::fromUtf8("modebox"));
        modebox->setEnabled(true);

        hboxLayout4->addWidget(modebox);

        pushButton2 = new QPushButton(MixerDialog);
        pushButton2->setObjectName(QString::fromUtf8("pushButton2"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pushButton2->sizePolicy().hasHeightForWidth());
        pushButton2->setSizePolicy(sizePolicy);

        hboxLayout4->addWidget(pushButton2);


        gridLayout->addLayout(hboxLayout4, 3, 0, 1, 5);

        hboxLayout5 = new QHBoxLayout();
        hboxLayout5->setSpacing(6);
        hboxLayout5->setObjectName(QString::fromUtf8("hboxLayout5"));
        syncbutton = new QPushButton(MixerDialog);
        syncbutton->setObjectName(QString::fromUtf8("syncbutton"));
        syncbutton->setEnabled(true);
        syncbutton->setCheckable(true);
        syncbutton->setChecked(true);

        hboxLayout5->addWidget(syncbutton);

        balanced = new QPushButton(MixerDialog);
        balanced->setObjectName(QString::fromUtf8("balanced"));
        balanced->setCheckable(true);

        hboxLayout5->addWidget(balanced);


        gridLayout->addLayout(hboxLayout5, 1, 2, 1, 3);

        hboxLayout6 = new QHBoxLayout();
        hboxLayout6->setSpacing(6);
        hboxLayout6->setObjectName(QString::fromUtf8("hboxLayout6"));
        vboxLayout7 = new QVBoxLayout();
        vboxLayout7->setSpacing(6);
        vboxLayout7->setObjectName(QString::fromUtf8("vboxLayout7"));
        textLabel1_2 = new QLabel(MixerDialog);
        textLabel1_2->setObjectName(QString::fromUtf8("textLabel1_2"));
        textLabel1_2->setWordWrap(false);

        vboxLayout7->addWidget(textLabel1_2);

        textLabel3_3 = new QLabel(MixerDialog);
        textLabel3_3->setObjectName(QString::fromUtf8("textLabel3_3"));
        textLabel3_3->setWordWrap(false);

        vboxLayout7->addWidget(textLabel3_3);

        textLabel3_3_2 = new QLabel(MixerDialog);
        textLabel3_3_2->setObjectName(QString::fromUtf8("textLabel3_3_2"));
        textLabel3_3_2->setWordWrap(false);

        vboxLayout7->addWidget(textLabel3_3_2);

        textLabel4_3 = new QLabel(MixerDialog);
        textLabel4_3->setObjectName(QString::fromUtf8("textLabel4_3"));
        textLabel4_3->setWordWrap(false);

        vboxLayout7->addWidget(textLabel4_3);


        hboxLayout6->addLayout(vboxLayout7);

        vboxLayout8 = new QVBoxLayout();
        vboxLayout8->setSpacing(6);
        vboxLayout8->setObjectName(QString::fromUtf8("vboxLayout8"));
        latency_audio = new QLabel(MixerDialog);
        latency_audio->setObjectName(QString::fromUtf8("latency_audio"));
        latency_audio->setWordWrap(false);

        vboxLayout8->addWidget(latency_audio);

        latency_buffer = new QLabel(MixerDialog);
        latency_buffer->setObjectName(QString::fromUtf8("latency_buffer"));
        latency_buffer->setWordWrap(false);

        vboxLayout8->addWidget(latency_buffer);

        latency_difference = new QLabel(MixerDialog);
        latency_difference->setObjectName(QString::fromUtf8("latency_difference"));
        latency_difference->setWordWrap(false);

        vboxLayout8->addWidget(latency_difference);

        synchronisation = new QLabel(MixerDialog);
        synchronisation->setObjectName(QString::fromUtf8("synchronisation"));
        synchronisation->setWordWrap(false);

        vboxLayout8->addWidget(synchronisation);


        hboxLayout6->addLayout(vboxLayout8);


        gridLayout->addLayout(hboxLayout6, 0, 3, 1, 1);

        spacer6 = new QSpacerItem(80, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(spacer6, 0, 4, 1, 1);

        QWidget::setTabOrder(Ch1Main, Ch2Main);
        QWidget::setTabOrder(Ch2Main, Ch1Monitor);
        QWidget::setTabOrder(Ch1Monitor, Ch2Monitor);

        retranslateUi(MixerDialog);
        QObject::connect(Ch1Main, SIGNAL(valueChanged(int)), MixerDialog, SLOT(volumesChanged()));
        QObject::connect(Ch2Main, SIGNAL(valueChanged(int)), MixerDialog, SLOT(volumesChanged()));
        QObject::connect(Ch1Monitor, SIGNAL(valueChanged(int)), MixerDialog, SLOT(volumesChanged()));
        QObject::connect(Ch2Monitor, SIGNAL(valueChanged(int)), MixerDialog, SLOT(volumesChanged()));
        QObject::connect(pushButton2, SIGNAL(clicked()), MixerDialog, SLOT(accept()));
        QObject::connect(balanced, SIGNAL(stateChanged(int)), MixerDialog, SLOT(volumesChanged()));
        QObject::connect(modebox, SIGNAL(activated(int)), MixerDialog, SLOT(volumesChanged()));
        QObject::connect(syncbutton, SIGNAL(stateChanged(int)), MixerDialog, SLOT(volumesChanged()));

        QMetaObject::connectSlotsByName(MixerDialog);
    } // setupUi

    void retranslateUi(QDialog *MixerDialog)
    {
        MixerDialog->setWindowTitle(QApplication::translate("MixerDialog", "BpmDj Mixer", 0, QApplication::UnicodeUTF8));
        textLabel1->setText(QApplication::translate("MixerDialog", "Main", 0, QApplication::UnicodeUTF8));
        Ch1MainText->setText(QApplication::translate("MixerDialog", "Ch1", 0, QApplication::UnicodeUTF8));
        Ch2MainText->setText(QApplication::translate("MixerDialog", "Ch2", 0, QApplication::UnicodeUTF8));
        textLabel2->setText(QApplication::translate("MixerDialog", "Monitor", 0, QApplication::UnicodeUTF8));
        Ch1MonitorText->setText(QApplication::translate("MixerDialog", "Ch1", 0, QApplication::UnicodeUTF8));
        Ch2MonitorText->setText(QApplication::translate("MixerDialog", "Ch2", 0, QApplication::UnicodeUTF8));
        textLabel1_3->setText(QApplication::translate("MixerDialog", "Disconnected", 0, QApplication::UnicodeUTF8));
        textLabel1_3_4->setText(QApplication::translate("MixerDialog", "Paused", 0, QApplication::UnicodeUTF8));
        textLabel1_3_3->setText(QApplication::translate("MixerDialog", "Syncing", 0, QApplication::UnicodeUTF8));
        textLabel1_3_2->setText(QApplication::translate("MixerDialog", "Playing", 0, QApplication::UnicodeUTF8));
        modebox->clear();
        modebox->insertItems(0, QStringList()
         << QApplication::translate("MixerDialog", "Out L = Main L, Out R = Main R", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MixerDialog", "Out L = Main, Out R = Monitor", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MixerDialog", "Out L = Monitor, Out R = Main", 0, QApplication::UnicodeUTF8)
        );
        pushButton2->setText(QApplication::translate("MixerDialog", "Quit", 0, QApplication::UnicodeUTF8));
        syncbutton->setText(QApplication::translate("MixerDialog", "Synchronise", 0, QApplication::UnicodeUTF8));
        balanced->setText(QApplication::translate("MixerDialog", "Balance", 0, QApplication::UnicodeUTF8));
        textLabel1_2->setText(QApplication::translate("MixerDialog", "Latency audio (ms)", 0, QApplication::UnicodeUTF8));
        textLabel3_3->setText(QApplication::translate("MixerDialog", "Latency mixer (ms)", 0, QApplication::UnicodeUTF8));
        textLabel3_3_2->setText(QApplication::translate("MixerDialog", "Latency difference (ticks)", 0, QApplication::UnicodeUTF8));
        textLabel4_3->setText(QApplication::translate("MixerDialog", "Synchronisation (Mes)", 0, QApplication::UnicodeUTF8));
        latency_audio->setText(QApplication::translate("MixerDialog", "...", 0, QApplication::UnicodeUTF8));
        latency_buffer->setText(QApplication::translate("MixerDialog", "...", 0, QApplication::UnicodeUTF8));
        latency_difference->setText(QApplication::translate("MixerDialog", "...", 0, QApplication::UnicodeUTF8));
        synchronisation->setText(QApplication::translate("MixerDialog", "...", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MixerDialog: public Ui_MixerDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_MIXERDIALOG_H
