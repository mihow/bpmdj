/********************************************************************************
** Form generated from reading UI file 'ui-renamer.ui'
**
** Created: Mon May 16 18:44:31 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_RENAMER_H
#define UI_2D_RENAMER_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Renamer
{
public:
    QVBoxLayout *verticalLayout_4;
    QVBoxLayout *verticalLayout_3;
    QTreeWidget *NameList;
    QHBoxLayout *horizontalLayout;
    QLabel *TextLabel2;
    QLineEdit *SubString;
    QHBoxLayout *horizontalLayout_2;
    QLabel *TextLabel1;
    QLineEdit *ReplaceString;
    QPushButton *PushButton58;
    QHBoxLayout *horizontalLayout_5;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QPushButton *PushButton5;
    QPushButton *PushButton101;
    QPushButton *PushButton3;
    QPushButton *betwseenBracesIsMixButtonbrace;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_4;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QPushButton *PushButton2;
    QPushButton *PushButton37;
    QPushButton *PushButton6;
    QPushButton *PushButton13;
    QPushButton *pushButton20;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_2;
    QPushButton *PushButton7;
    QPushButton *isMixButton;
    QPushButton *PushButton12;
    QPushButton *pushButton17;
    QPushButton *pushButton18;
    QPushButton *pushButton23;
    QFrame *line1;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *spacer1;
    QPushButton *PushButton34;
    QPushButton *PushButton16;
    QPushButton *PushButton35;

    void setupUi(QDialog *Renamer)
    {
        if (Renamer->objectName().isEmpty())
            Renamer->setObjectName(QString::fromUtf8("Renamer"));
        Renamer->resize(590, 491);
        Renamer->setSizeGripEnabled(true);
        verticalLayout_4 = new QVBoxLayout(Renamer);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        NameList = new QTreeWidget(Renamer);
        NameList->setObjectName(QString::fromUtf8("NameList"));
        NameList->setSelectionMode(QAbstractItemView::ExtendedSelection);
        NameList->setSortingEnabled(true);
        NameList->setAnimated(true);

        verticalLayout_3->addWidget(NameList);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        TextLabel2 = new QLabel(Renamer);
        TextLabel2->setObjectName(QString::fromUtf8("TextLabel2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TextLabel2->sizePolicy().hasHeightForWidth());
        TextLabel2->setSizePolicy(sizePolicy);
        TextLabel2->setFrameShape(QFrame::NoFrame);
        TextLabel2->setFrameShadow(QFrame::Plain);
        TextLabel2->setWordWrap(false);

        horizontalLayout->addWidget(TextLabel2);

        SubString = new QLineEdit(Renamer);
        SubString->setObjectName(QString::fromUtf8("SubString"));
        QFont font;
        font.setFamily(QString::fromUtf8("adobe-courier"));
        SubString->setFont(font);

        horizontalLayout->addWidget(SubString);


        verticalLayout_3->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        TextLabel1 = new QLabel(Renamer);
        TextLabel1->setObjectName(QString::fromUtf8("TextLabel1"));
        sizePolicy.setHeightForWidth(TextLabel1->sizePolicy().hasHeightForWidth());
        TextLabel1->setSizePolicy(sizePolicy);
        TextLabel1->setWordWrap(false);

        horizontalLayout_2->addWidget(TextLabel1);

        ReplaceString = new QLineEdit(Renamer);
        ReplaceString->setObjectName(QString::fromUtf8("ReplaceString"));
        ReplaceString->setFont(font);

        horizontalLayout_2->addWidget(ReplaceString);

        PushButton58 = new QPushButton(Renamer);
        PushButton58->setObjectName(QString::fromUtf8("PushButton58"));
        PushButton58->setEnabled(true);

        horizontalLayout_2->addWidget(PushButton58);


        verticalLayout_3->addLayout(horizontalLayout_2);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        groupBox = new QGroupBox(Renamer);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(2, 2, 2, 2);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        PushButton5 = new QPushButton(groupBox);
        PushButton5->setObjectName(QString::fromUtf8("PushButton5"));

        verticalLayout->addWidget(PushButton5);

        PushButton101 = new QPushButton(groupBox);
        PushButton101->setObjectName(QString::fromUtf8("PushButton101"));
        PushButton101->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(PushButton101->sizePolicy().hasHeightForWidth());
        PushButton101->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(PushButton101);

        PushButton3 = new QPushButton(groupBox);
        PushButton3->setObjectName(QString::fromUtf8("PushButton3"));

        verticalLayout->addWidget(PushButton3);

        betwseenBracesIsMixButtonbrace = new QPushButton(groupBox);
        betwseenBracesIsMixButtonbrace->setObjectName(QString::fromUtf8("betwseenBracesIsMixButtonbrace"));
        betwseenBracesIsMixButtonbrace->setEnabled(true);

        verticalLayout->addWidget(betwseenBracesIsMixButtonbrace);


        horizontalLayout_5->addWidget(groupBox);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        groupBox_2 = new QGroupBox(Renamer);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(2, 2, 2, 2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        PushButton2 = new QPushButton(groupBox_2);
        PushButton2->setObjectName(QString::fromUtf8("PushButton2"));

        gridLayout->addWidget(PushButton2, 0, 0, 1, 1);

        PushButton37 = new QPushButton(groupBox_2);
        PushButton37->setObjectName(QString::fromUtf8("PushButton37"));
        PushButton37->setEnabled(true);

        gridLayout->addWidget(PushButton37, 0, 1, 1, 1);

        PushButton6 = new QPushButton(groupBox_2);
        PushButton6->setObjectName(QString::fromUtf8("PushButton6"));
        sizePolicy1.setHeightForWidth(PushButton6->sizePolicy().hasHeightForWidth());
        PushButton6->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(PushButton6, 1, 0, 1, 1);

        PushButton13 = new QPushButton(groupBox_2);
        PushButton13->setObjectName(QString::fromUtf8("PushButton13"));
        sizePolicy1.setHeightForWidth(PushButton13->sizePolicy().hasHeightForWidth());
        PushButton13->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(PushButton13, 1, 1, 1, 1);

        pushButton20 = new QPushButton(groupBox_2);
        pushButton20->setObjectName(QString::fromUtf8("pushButton20"));
        pushButton20->setEnabled(true);

        gridLayout->addWidget(pushButton20, 2, 0, 1, 1);


        horizontalLayout_4->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(Renamer);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        gridLayout_2 = new QGridLayout(groupBox_3);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(2, 2, 2, 2);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        PushButton7 = new QPushButton(groupBox_3);
        PushButton7->setObjectName(QString::fromUtf8("PushButton7"));
        sizePolicy1.setHeightForWidth(PushButton7->sizePolicy().hasHeightForWidth());
        PushButton7->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(PushButton7, 0, 0, 1, 2);

        isMixButton = new QPushButton(groupBox_3);
        isMixButton->setObjectName(QString::fromUtf8("isMixButton"));
        isMixButton->setEnabled(true);
        sizePolicy1.setHeightForWidth(isMixButton->sizePolicy().hasHeightForWidth());
        isMixButton->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(isMixButton, 0, 2, 1, 1);

        PushButton12 = new QPushButton(groupBox_3);
        PushButton12->setObjectName(QString::fromUtf8("PushButton12"));
        sizePolicy1.setHeightForWidth(PushButton12->sizePolicy().hasHeightForWidth());
        PushButton12->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(PushButton12, 1, 0, 1, 1);

        pushButton17 = new QPushButton(groupBox_3);
        pushButton17->setObjectName(QString::fromUtf8("pushButton17"));
        pushButton17->setEnabled(true);
        sizePolicy1.setHeightForWidth(pushButton17->sizePolicy().hasHeightForWidth());
        pushButton17->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(pushButton17, 1, 1, 1, 2);

        pushButton18 = new QPushButton(groupBox_3);
        pushButton18->setObjectName(QString::fromUtf8("pushButton18"));
        pushButton18->setEnabled(true);

        gridLayout_2->addWidget(pushButton18, 2, 0, 1, 2);

        pushButton23 = new QPushButton(groupBox_3);
        pushButton23->setObjectName(QString::fromUtf8("pushButton23"));

        gridLayout_2->addWidget(pushButton23, 2, 2, 1, 1);


        horizontalLayout_4->addWidget(groupBox_3);


        verticalLayout_2->addLayout(horizontalLayout_4);

        line1 = new QFrame(Renamer);
        line1->setObjectName(QString::fromUtf8("line1"));
        line1->setFrameShape(QFrame::HLine);
        line1->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(line1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        spacer1 = new QSpacerItem(240, 21, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(spacer1);

        PushButton34 = new QPushButton(Renamer);
        PushButton34->setObjectName(QString::fromUtf8("PushButton34"));
        sizePolicy1.setHeightForWidth(PushButton34->sizePolicy().hasHeightForWidth());
        PushButton34->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(PushButton34);

        PushButton16 = new QPushButton(Renamer);
        PushButton16->setObjectName(QString::fromUtf8("PushButton16"));
        sizePolicy1.setHeightForWidth(PushButton16->sizePolicy().hasHeightForWidth());
        PushButton16->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(PushButton16);

        PushButton35 = new QPushButton(Renamer);
        PushButton35->setObjectName(QString::fromUtf8("PushButton35"));
        sizePolicy1.setHeightForWidth(PushButton35->sizePolicy().hasHeightForWidth());
        PushButton35->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(PushButton35);


        verticalLayout_2->addLayout(horizontalLayout_3);


        horizontalLayout_5->addLayout(verticalLayout_2);


        verticalLayout_3->addLayout(horizontalLayout_5);


        verticalLayout_4->addLayout(verticalLayout_3);


        retranslateUi(Renamer);
        QObject::connect(PushButton35, SIGNAL(clicked()), Renamer, SLOT(reject()));
        QObject::connect(PushButton5, SIGNAL(clicked()), Renamer, SLOT(smallCapsInWords()));
        QObject::connect(PushButton3, SIGNAL(clicked()), Renamer, SLOT(capitalizeAfterSpace()));
        QObject::connect(PushButton2, SIGNAL(clicked()), Renamer, SLOT(removeSpaces()));
        QObject::connect(PushButton6, SIGNAL(clicked()), Renamer, SLOT(removeSpecials()));
        QObject::connect(PushButton7, SIGNAL(clicked()), Renamer, SLOT(deleteSubString()));
        QObject::connect(isMixButton, SIGNAL(clicked()), Renamer, SLOT(substringIsMix()));
        QObject::connect(PushButton12, SIGNAL(clicked()), Renamer, SLOT(subStringIsAuthor()));
        QObject::connect(PushButton13, SIGNAL(clicked()), Renamer, SLOT(removeFirstChar()));
        QObject::connect(PushButton58, SIGNAL(clicked()), Renamer, SLOT(replaceSubString()));
        QObject::connect(PushButton34, SIGNAL(clicked()), Renamer, SLOT(realizeSelection()));
        QObject::connect(PushButton16, SIGNAL(clicked()), Renamer, SLOT(ignoreSelection()));
        QObject::connect(pushButton17, SIGNAL(clicked()), Renamer, SLOT(beforeMinusIsAuthor()));
        QObject::connect(pushButton18, SIGNAL(clicked()), Renamer, SLOT(keySelectionIsAuthor()));
        QObject::connect(pushButton20, SIGNAL(clicked()), Renamer, SLOT(betweenBracesIsTrash()));
        QObject::connect(PushButton37, SIGNAL(clicked()), Renamer, SLOT(removeFirstSpecials()));
        QObject::connect(pushButton23, SIGNAL(clicked()), Renamer, SLOT(replaceUnderscores()));
        QObject::connect(PushButton101, SIGNAL(clicked()), Renamer, SLOT(fixExtention()));
        QObject::connect(betwseenBracesIsMixButtonbrace, SIGNAL(clicked()), Renamer, SLOT(betweenBracesIsMix()));
        QObject::connect(NameList, SIGNAL(itemSelectionChanged()), Renamer, SLOT(changeSelection()));

        QMetaObject::connectSlotsByName(Renamer);
    } // setupUi

    void retranslateUi(QDialog *Renamer)
    {
        Renamer->setWindowTitle(QApplication::translate("Renamer", "RenameBox", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = NameList->headerItem();
        ___qtreewidgetitem->setText(2, QApplication::translate("Renamer", "Full Name", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(1, QApplication::translate("Renamer", "Original", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("Renamer", "New", 0, QApplication::UnicodeUTF8));
        TextLabel2->setText(QApplication::translate("Renamer", "Key:", 0, QApplication::UnicodeUTF8));
        SubString->setText(QString());
        TextLabel1->setText(QApplication::translate("Renamer", "Replace:", 0, QApplication::UnicodeUTF8));
        PushButton58->setText(QApplication::translate("Renamer", "Change", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("Renamer", "Fix", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton5->setToolTip(QApplication::translate("Renamer", "Decapitalizes all letters in words", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton5->setText(QApplication::translate("Renamer", "SmallCapsIn&Words", 0, QApplication::UnicodeUTF8));
        PushButton5->setShortcut(QApplication::translate("Renamer", "Alt+W", 0, QApplication::UnicodeUTF8));
        PushButton101->setText(QApplication::translate("Renamer", "Extention (.MP3, .IDX)", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton3->setToolTip(QApplication::translate("Renamer", "Capitalizes all letters after spaces", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton3->setText(QApplication::translate("Renamer", "&CapitalizeAfterSpace", 0, QApplication::UnicodeUTF8));
        PushButton3->setShortcut(QApplication::translate("Renamer", "Alt+C", 0, QApplication::UnicodeUTF8));
        betwseenBracesIsMixButtonbrace->setText(QApplication::translate("Renamer", "'(..)' is Mix", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("Renamer", "Remove", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        PushButton2->setToolTip(QApplication::translate("Renamer", "Removes All Spaces", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        PushButton2->setText(QApplication::translate("Renamer", "&Space", 0, QApplication::UnicodeUTF8));
        PushButton2->setShortcut(QApplication::translate("Renamer", "Alt+S", 0, QApplication::UnicodeUTF8));
        PushButton37->setText(QApplication::translate("Renamer", "^Specials", 0, QApplication::UnicodeUTF8));
        PushButton6->setText(QApplication::translate("Renamer", "S&pecials", 0, QApplication::UnicodeUTF8));
        PushButton13->setText(QApplication::translate("Renamer", "^C&har", 0, QApplication::UnicodeUTF8));
        pushButton20->setText(QApplication::translate("Renamer", "'(...)' ", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("Renamer", "Key String", 0, QApplication::UnicodeUTF8));
        PushButton7->setText(QApplication::translate("Renamer", "&Delete ", 0, QApplication::UnicodeUTF8));
        isMixButton->setText(QApplication::translate("Renamer", "Is&Mix", 0, QApplication::UnicodeUTF8));
        isMixButton->setShortcut(QApplication::translate("Renamer", "Alt+M", 0, QApplication::UnicodeUTF8));
        PushButton12->setText(QApplication::translate("Renamer", "Is&Author", 0, QApplication::UnicodeUTF8));
        pushButton17->setText(QApplication::translate("Renamer", "Before '-' is author", 0, QApplication::UnicodeUTF8));
        pushButton18->setText(QApplication::translate("Renamer", "Key sel is auth", 0, QApplication::UnicodeUTF8));
        pushButton23->setText(QApplication::translate("Renamer", "'_' -> ' '", 0, QApplication::UnicodeUTF8));
        PushButton34->setText(QApplication::translate("Renamer", "&Realize", 0, QApplication::UnicodeUTF8));
        PushButton34->setShortcut(QApplication::translate("Renamer", "Alt+R", 0, QApplication::UnicodeUTF8));
        PushButton16->setText(QApplication::translate("Renamer", "&Ignore", 0, QApplication::UnicodeUTF8));
        PushButton16->setShortcut(QApplication::translate("Renamer", "Alt+I", 0, QApplication::UnicodeUTF8));
        PushButton35->setText(QApplication::translate("Renamer", "D&one", 0, QApplication::UnicodeUTF8));
        PushButton35->setShortcut(QApplication::translate("Renamer", "Alt+O", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Renamer: public Ui_Renamer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_RENAMER_H
