/********************************************************************************
** Form generated from reading UI file 'ui-setupwizard.ui'
**
** Created: Thu May 12 17:50:02 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_SETUPWIZARD_H
#define UI_2D_SETUPWIZARD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SetupWizard
{
public:
    QGridLayout *gridLayout;
    QFrame *line;
    QVBoxLayout *vboxLayout;
    QLabel *TextLabel2;
    QLabel *TextLabel3;
    QLabel *TextLabel3_2;
    QDialogButtonBox *buttonBox;
    QLabel *TextLabel5;
    QLabel *TextLabel1;

    void setupUi(QDialog *SetupWizard)
    {
        if (SetupWizard->objectName().isEmpty())
            SetupWizard->setObjectName(QString::fromUtf8("SetupWizard"));
        SetupWizard->resize(686, 258);
        gridLayout = new QGridLayout(SetupWizard);
#ifndef Q_OS_MAC
        gridLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout->setContentsMargins(9, 9, 9, 9);
#endif
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        line = new QFrame(SetupWizard);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 3, 0, 2, 2);

        vboxLayout = new QVBoxLayout();
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
        vboxLayout->setContentsMargins(0, 0, 0, 0);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        TextLabel2 = new QLabel(SetupWizard);
        TextLabel2->setObjectName(QString::fromUtf8("TextLabel2"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        TextLabel2->setFont(font);
        TextLabel2->setAlignment(Qt::AlignCenter);
        TextLabel2->setWordWrap(false);

        vboxLayout->addWidget(TextLabel2);

        TextLabel3 = new QLabel(SetupWizard);
        TextLabel3->setObjectName(QString::fromUtf8("TextLabel3"));
        TextLabel3->setFont(font);
        TextLabel3->setAlignment(Qt::AlignCenter);
        TextLabel3->setWordWrap(false);

        vboxLayout->addWidget(TextLabel3);

        TextLabel3_2 = new QLabel(SetupWizard);
        TextLabel3_2->setObjectName(QString::fromUtf8("TextLabel3_2"));
        TextLabel3_2->setFont(font);
        TextLabel3_2->setAlignment(Qt::AlignCenter);
        TextLabel3_2->setWordWrap(false);

        vboxLayout->addWidget(TextLabel3_2);


        gridLayout->addLayout(vboxLayout, 1, 0, 1, 2);

        buttonBox = new QDialogButtonBox(SetupWizard);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 4, 1, 1, 1);

        TextLabel5 = new QLabel(SetupWizard);
        TextLabel5->setObjectName(QString::fromUtf8("TextLabel5"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TextLabel5->sizePolicy().hasHeightForWidth());
        TextLabel5->setSizePolicy(sizePolicy);
        TextLabel5->setWordWrap(false);

        gridLayout->addWidget(TextLabel5, 0, 0, 1, 2);

        TextLabel1 = new QLabel(SetupWizard);
        TextLabel1->setObjectName(QString::fromUtf8("TextLabel1"));
        TextLabel1->setTextFormat(Qt::AutoText);
        TextLabel1->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        TextLabel1->setWordWrap(true);

        gridLayout->addWidget(TextLabel1, 2, 0, 1, 2);


        retranslateUi(SetupWizard);
        QObject::connect(buttonBox, SIGNAL(accepted()), SetupWizard, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SetupWizard, SLOT(reject()));

        QMetaObject::connectSlotsByName(SetupWizard);
    } // setupUi

    void retranslateUi(QDialog *SetupWizard)
    {
        SetupWizard->setWindowTitle(QApplication::translate("SetupWizard", "BpmDj Brief Setup Notice", 0, QApplication::UnicodeUTF8));
        TextLabel2->setText(QApplication::translate("SetupWizard", "./music", 0, QApplication::UnicodeUTF8));
        TextLabel3->setText(QApplication::translate("SetupWizard", "./index", 0, QApplication::UnicodeUTF8));
        TextLabel3_2->setText(QApplication::translate("SetupWizard", "./fragments", 0, QApplication::UnicodeUTF8));
        TextLabel5->setText(QApplication::translate("SetupWizard", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">BpmDj</span> requires the existence of three directories</p></body></html>", 0, QApplication::UnicodeUTF8));
        TextLabel1->setText(QApplication::translate("SetupWizard", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">The <span style=\" font-weight:600;\">music </span>directory should contain all the .mp3 files you have. You can use symbolic links to refer to other parts of the filesystem. The <span style=\" font-weight:600;\">index</span> directory will contain all meta information bpmdj needs about the .mp3 files. (such as tempo, cue points and so on). The <span style=\" font-weight:600;\">fragments</span> directory will be used as a temporary storage for small audio fragments.</p></body></html>", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SetupWizard: public Ui_SetupWizard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_SETUPWIZARD_H
