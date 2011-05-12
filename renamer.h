#ifndef RENAMER_H
#define RENAMER_H

#include <qvariant.h>


#include <Qt3Support/Q3GroupBox>
#include <Qt3Support/Q3Header>
#include <Qt3Support/Q3ListView>
#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_Renamer
{
public:
    QGridLayout *gridLayout;
    QLabel *TextLabel2;
    QLabel *TextLabel1;
    QLineEdit *ReplaceString;
    QPushButton *PushButton58;
    QLineEdit *SubString;
    Q3ListView *NameList;
    Q3GroupBox *GroupBox1;
    QGridLayout *gridLayout1;
    QPushButton *PushButton5;
    QPushButton *PushButton101;
    QPushButton *PushButton3;
    QPushButton *betwseenBracesIsMixButtonbrace;
    Q3GroupBox *GroupBox2;
    QGridLayout *gridLayout2;
    QPushButton *PushButton6;
    QPushButton *PushButton2;
    QPushButton *PushButton13;
    QPushButton *PushButton37;
    QPushButton *pushButton20;
    QSpacerItem *spacer1;
    QPushButton *PushButton34;
    QPushButton *PushButton16;
    QPushButton *PushButton35;
    QFrame *line1;
    Q3GroupBox *GroupBox3;
    QGridLayout *gridLayout3;
    QPushButton *PushButton7;
    QPushButton *PushButton12;
    QPushButton *isMixButton;
    QPushButton *pushButton17;
    QPushButton *pushButton18;
    QPushButton *pushButton23;

    void setupUi(QDialog *Renamer)
    {
    if (Renamer->objectName().isEmpty())
        Renamer->setObjectName(QString::fromUtf8("Renamer"));
    Renamer->resize(667, 471);
    gridLayout = new QGridLayout(Renamer);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(11);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    TextLabel2 = new QLabel(Renamer);
    TextLabel2->setObjectName(QString::fromUtf8("TextLabel2"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(1));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(TextLabel2->sizePolicy().hasHeightForWidth());
    TextLabel2->setSizePolicy(sizePolicy);
    TextLabel2->setFrameShape(QFrame::NoFrame);
    TextLabel2->setFrameShadow(QFrame::Plain);
    TextLabel2->setWordWrap(false);

    gridLayout->addWidget(TextLabel2, 1, 0, 1, 1);

    TextLabel1 = new QLabel(Renamer);
    TextLabel1->setObjectName(QString::fromUtf8("TextLabel1"));
    sizePolicy.setHeightForWidth(TextLabel1->sizePolicy().hasHeightForWidth());
    TextLabel1->setSizePolicy(sizePolicy);
    TextLabel1->setWordWrap(false);

    gridLayout->addWidget(TextLabel1, 2, 0, 1, 1);

    ReplaceString = new QLineEdit(Renamer);
    ReplaceString->setObjectName(QString::fromUtf8("ReplaceString"));
    QFont font;
    font.setFamily(QString::fromUtf8("adobe-courier"));
    ReplaceString->setFont(font);

    gridLayout->addWidget(ReplaceString, 2, 1, 1, 4);

    PushButton58 = new QPushButton(Renamer);
    PushButton58->setObjectName(QString::fromUtf8("PushButton58"));
    PushButton58->setEnabled(true);

    gridLayout->addWidget(PushButton58, 2, 5, 1, 1);

    SubString = new QLineEdit(Renamer);
    SubString->setObjectName(QString::fromUtf8("SubString"));
    SubString->setFont(font);

    gridLayout->addWidget(SubString, 1, 1, 1, 5);

    NameList = new Q3ListView(Renamer);
    NameList->addColumn(QApplication::translate("Renamer", "New", 0, QApplication::UnicodeUTF8));
    NameList->header()->setClickEnabled(true, NameList->header()->count() - 1);
    NameList->header()->setResizeEnabled(true, NameList->header()->count() - 1);
    NameList->addColumn(QApplication::translate("Renamer", "Original ", 0, QApplication::UnicodeUTF8));
    NameList->header()->setClickEnabled(true, NameList->header()->count() - 1);
    NameList->header()->setResizeEnabled(true, NameList->header()->count() - 1);
    NameList->addColumn(QApplication::translate("Renamer", "Full Name", 0, QApplication::UnicodeUTF8));
    NameList->header()->setClickEnabled(true, NameList->header()->count() - 1);
    NameList->header()->setResizeEnabled(true, NameList->header()->count() - 1);
    NameList->setObjectName(QString::fromUtf8("NameList"));
    NameList->setFont(font);
    NameList->setFrameShape(QFrame::NoFrame);
    NameList->setSelectionMode(Q3ListView::Extended);
    NameList->setAllColumnsShowFocus(true);

    gridLayout->addWidget(NameList, 0, 0, 1, 6);

    GroupBox1 = new Q3GroupBox(Renamer);
    GroupBox1->setObjectName(QString::fromUtf8("GroupBox1"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(1), static_cast<QSizePolicy::Policy>(5));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(GroupBox1->sizePolicy().hasHeightForWidth());
    GroupBox1->setSizePolicy(sizePolicy1);
    GroupBox1->setColumnLayout(0, Qt::Vertical);
    GroupBox1->layout()->setSpacing(6);
    GroupBox1->layout()->setMargin(11);
    gridLayout1 = new QGridLayout();
    QBoxLayout *boxlayout = qobject_cast<QBoxLayout *>(GroupBox1->layout());
    if (boxlayout)
        boxlayout->addLayout(gridLayout1);
    gridLayout1->setAlignment(Qt::AlignTop);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    PushButton5 = new QPushButton(GroupBox1);
    PushButton5->setObjectName(QString::fromUtf8("PushButton5"));

    gridLayout1->addWidget(PushButton5, 0, 0, 1, 1);

    PushButton101 = new QPushButton(GroupBox1);
    PushButton101->setObjectName(QString::fromUtf8("PushButton101"));
    PushButton101->setEnabled(true);
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(0));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(PushButton101->sizePolicy().hasHeightForWidth());
    PushButton101->setSizePolicy(sizePolicy2);

    gridLayout1->addWidget(PushButton101, 1, 0, 1, 1);

    PushButton3 = new QPushButton(GroupBox1);
    PushButton3->setObjectName(QString::fromUtf8("PushButton3"));

    gridLayout1->addWidget(PushButton3, 2, 0, 1, 1);

    betwseenBracesIsMixButtonbrace = new QPushButton(GroupBox1);
    betwseenBracesIsMixButtonbrace->setObjectName(QString::fromUtf8("betwseenBracesIsMixButtonbrace"));
    betwseenBracesIsMixButtonbrace->setEnabled(true);

    gridLayout1->addWidget(betwseenBracesIsMixButtonbrace, 3, 0, 1, 1);


    gridLayout->addWidget(GroupBox1, 3, 0, 3, 2);

    GroupBox2 = new Q3GroupBox(Renamer);
    GroupBox2->setObjectName(QString::fromUtf8("GroupBox2"));
    sizePolicy1.setHeightForWidth(GroupBox2->sizePolicy().hasHeightForWidth());
    GroupBox2->setSizePolicy(sizePolicy1);
    GroupBox2->setColumnLayout(0, Qt::Vertical);
    GroupBox2->layout()->setSpacing(6);
    GroupBox2->layout()->setMargin(11);
    gridLayout2 = new QGridLayout();
    QBoxLayout *boxlayout1 = qobject_cast<QBoxLayout *>(GroupBox2->layout());
    if (boxlayout1)
        boxlayout1->addLayout(gridLayout2);
    gridLayout2->setAlignment(Qt::AlignTop);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    PushButton6 = new QPushButton(GroupBox2);
    PushButton6->setObjectName(QString::fromUtf8("PushButton6"));
    sizePolicy2.setHeightForWidth(PushButton6->sizePolicy().hasHeightForWidth());
    PushButton6->setSizePolicy(sizePolicy2);

    gridLayout2->addWidget(PushButton6, 1, 0, 1, 1);

    PushButton2 = new QPushButton(GroupBox2);
    PushButton2->setObjectName(QString::fromUtf8("PushButton2"));

    gridLayout2->addWidget(PushButton2, 0, 0, 1, 1);

    PushButton13 = new QPushButton(GroupBox2);
    PushButton13->setObjectName(QString::fromUtf8("PushButton13"));
    sizePolicy2.setHeightForWidth(PushButton13->sizePolicy().hasHeightForWidth());
    PushButton13->setSizePolicy(sizePolicy2);

    gridLayout2->addWidget(PushButton13, 1, 1, 1, 1);

    PushButton37 = new QPushButton(GroupBox2);
    PushButton37->setObjectName(QString::fromUtf8("PushButton37"));
    PushButton37->setEnabled(true);

    gridLayout2->addWidget(PushButton37, 0, 1, 1, 1);

    pushButton20 = new QPushButton(GroupBox2);
    pushButton20->setObjectName(QString::fromUtf8("pushButton20"));
    pushButton20->setEnabled(true);

    gridLayout2->addWidget(pushButton20, 2, 0, 1, 2);


    gridLayout->addWidget(GroupBox2, 3, 2, 1, 1);

    spacer1 = new QSpacerItem(240, 21, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(spacer1, 5, 2, 1, 1);

    PushButton34 = new QPushButton(Renamer);
    PushButton34->setObjectName(QString::fromUtf8("PushButton34"));
    sizePolicy2.setHeightForWidth(PushButton34->sizePolicy().hasHeightForWidth());
    PushButton34->setSizePolicy(sizePolicy2);

    gridLayout->addWidget(PushButton34, 5, 3, 1, 1);

    PushButton16 = new QPushButton(Renamer);
    PushButton16->setObjectName(QString::fromUtf8("PushButton16"));
    sizePolicy2.setHeightForWidth(PushButton16->sizePolicy().hasHeightForWidth());
    PushButton16->setSizePolicy(sizePolicy2);

    gridLayout->addWidget(PushButton16, 5, 4, 1, 1);

    PushButton35 = new QPushButton(Renamer);
    PushButton35->setObjectName(QString::fromUtf8("PushButton35"));
    sizePolicy2.setHeightForWidth(PushButton35->sizePolicy().hasHeightForWidth());
    PushButton35->setSizePolicy(sizePolicy2);

    gridLayout->addWidget(PushButton35, 5, 5, 1, 1);

    line1 = new QFrame(Renamer);
    line1->setObjectName(QString::fromUtf8("line1"));
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);

    gridLayout->addWidget(line1, 4, 2, 1, 4);

    GroupBox3 = new Q3GroupBox(Renamer);
    GroupBox3->setObjectName(QString::fromUtf8("GroupBox3"));
    sizePolicy1.setHeightForWidth(GroupBox3->sizePolicy().hasHeightForWidth());
    GroupBox3->setSizePolicy(sizePolicy1);
    GroupBox3->setColumnLayout(0, Qt::Vertical);
    GroupBox3->layout()->setSpacing(6);
    GroupBox3->layout()->setMargin(11);
    gridLayout3 = new QGridLayout();
    QBoxLayout *boxlayout2 = qobject_cast<QBoxLayout *>(GroupBox3->layout());
    if (boxlayout2)
        boxlayout2->addLayout(gridLayout3);
    gridLayout3->setAlignment(Qt::AlignTop);
    gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
    PushButton7 = new QPushButton(GroupBox3);
    PushButton7->setObjectName(QString::fromUtf8("PushButton7"));
    sizePolicy2.setHeightForWidth(PushButton7->sizePolicy().hasHeightForWidth());
    PushButton7->setSizePolicy(sizePolicy2);

    gridLayout3->addWidget(PushButton7, 0, 0, 1, 1);

    PushButton12 = new QPushButton(GroupBox3);
    PushButton12->setObjectName(QString::fromUtf8("PushButton12"));
    sizePolicy2.setHeightForWidth(PushButton12->sizePolicy().hasHeightForWidth());
    PushButton12->setSizePolicy(sizePolicy2);

    gridLayout3->addWidget(PushButton12, 1, 0, 1, 1);

    isMixButton = new QPushButton(GroupBox3);
    isMixButton->setObjectName(QString::fromUtf8("isMixButton"));
    isMixButton->setEnabled(true);
    sizePolicy2.setHeightForWidth(isMixButton->sizePolicy().hasHeightForWidth());
    isMixButton->setSizePolicy(sizePolicy2);

    gridLayout3->addWidget(isMixButton, 0, 1, 1, 2);

    pushButton17 = new QPushButton(GroupBox3);
    pushButton17->setObjectName(QString::fromUtf8("pushButton17"));
    pushButton17->setEnabled(true);
    sizePolicy2.setHeightForWidth(pushButton17->sizePolicy().hasHeightForWidth());
    pushButton17->setSizePolicy(sizePolicy2);

    gridLayout3->addWidget(pushButton17, 1, 1, 1, 2);

    pushButton18 = new QPushButton(GroupBox3);
    pushButton18->setObjectName(QString::fromUtf8("pushButton18"));
    pushButton18->setEnabled(true);

    gridLayout3->addWidget(pushButton18, 2, 0, 1, 2);

    pushButton23 = new QPushButton(GroupBox3);
    pushButton23->setObjectName(QString::fromUtf8("pushButton23"));

    gridLayout3->addWidget(pushButton23, 2, 2, 1, 1);


    gridLayout->addWidget(GroupBox3, 3, 3, 1, 3);


    retranslateUi(Renamer);
    QObject::connect(PushButton35, SIGNAL(clicked()), Renamer, SLOT(reject()));
    QObject::connect(PushButton5, SIGNAL(clicked()), Renamer, SLOT(smallCapsInWords()));
    QObject::connect(PushButton3, SIGNAL(clicked()), Renamer, SLOT(capitalizeAfterSpace()));
    QObject::connect(PushButton2, SIGNAL(clicked()), Renamer, SLOT(removeSpaces()));
    QObject::connect(PushButton6, SIGNAL(clicked()), Renamer, SLOT(removeSpecials()));
    QObject::connect(NameList, SIGNAL(selectionChanged()), Renamer, SLOT(changeSelection()));
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

    QMetaObject::connectSlotsByName(Renamer);
    } // setupUi

    void retranslateUi(QDialog *Renamer)
    {
    Renamer->setWindowTitle(QApplication::translate("Renamer", "RenameBox", 0, QApplication::UnicodeUTF8));
    TextLabel2->setText(QApplication::translate("Renamer", "Key:", 0, QApplication::UnicodeUTF8));
    TextLabel1->setText(QApplication::translate("Renamer", "Replace:", 0, QApplication::UnicodeUTF8));
    PushButton58->setText(QApplication::translate("Renamer", "Change", 0, QApplication::UnicodeUTF8));
    SubString->setText(QString());
    NameList->header()->setLabel(0, QApplication::translate("Renamer", "New", 0, QApplication::UnicodeUTF8));
    NameList->header()->setLabel(1, QApplication::translate("Renamer", "Original ", 0, QApplication::UnicodeUTF8));
    NameList->header()->setLabel(2, QApplication::translate("Renamer", "Full Name", 0, QApplication::UnicodeUTF8));
    GroupBox1->setTitle(QApplication::translate("Renamer", "Fix", 0, QApplication::UnicodeUTF8));
    PushButton5->setText(QApplication::translate("Renamer", "SmallCapsIn&Words", 0, QApplication::UnicodeUTF8));
    PushButton5->setShortcut(QApplication::translate("Renamer", "Alt+W", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    PushButton5->setProperty("toolTip", QVariant(QApplication::translate("Renamer", "Decapitalizes all letters in words", 0, QApplication::UnicodeUTF8)));
#endif // QT_NO_TOOLTIP

    PushButton101->setText(QApplication::translate("Renamer", "Extention (.MP3, .IDX)", 0, QApplication::UnicodeUTF8));
    PushButton3->setText(QApplication::translate("Renamer", "&CapitalizeAfterSpace", 0, QApplication::UnicodeUTF8));
    PushButton3->setShortcut(QApplication::translate("Renamer", "Alt+C", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    PushButton3->setProperty("toolTip", QVariant(QApplication::translate("Renamer", "Capitalizes all letters after spaces", 0, QApplication::UnicodeUTF8)));
#endif // QT_NO_TOOLTIP

    betwseenBracesIsMixButtonbrace->setText(QApplication::translate("Renamer", "'(..)' is Mix", 0, QApplication::UnicodeUTF8));
    GroupBox2->setTitle(QApplication::translate("Renamer", "Remove", 0, QApplication::UnicodeUTF8));
    PushButton6->setText(QApplication::translate("Renamer", "S&pecials", 0, QApplication::UnicodeUTF8));
    PushButton2->setText(QApplication::translate("Renamer", "&Space", 0, QApplication::UnicodeUTF8));
    PushButton2->setShortcut(QApplication::translate("Renamer", "Alt+S", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    PushButton2->setProperty("toolTip", QVariant(QApplication::translate("Renamer", "Removes All Spaces", 0, QApplication::UnicodeUTF8)));
#endif // QT_NO_TOOLTIP

    PushButton13->setText(QApplication::translate("Renamer", "^C&har", 0, QApplication::UnicodeUTF8));
    PushButton37->setText(QApplication::translate("Renamer", "^Specials", 0, QApplication::UnicodeUTF8));
    pushButton20->setText(QApplication::translate("Renamer", "'(...)' ", 0, QApplication::UnicodeUTF8));
    PushButton34->setText(QApplication::translate("Renamer", "&Realize", 0, QApplication::UnicodeUTF8));
    PushButton34->setShortcut(QApplication::translate("Renamer", "Alt+R", 0, QApplication::UnicodeUTF8));
    PushButton16->setText(QApplication::translate("Renamer", "&Ignore", 0, QApplication::UnicodeUTF8));
    PushButton16->setShortcut(QApplication::translate("Renamer", "Alt+I", 0, QApplication::UnicodeUTF8));
    PushButton35->setText(QApplication::translate("Renamer", "D&one", 0, QApplication::UnicodeUTF8));
    PushButton35->setShortcut(QApplication::translate("Renamer", "Alt+O", 0, QApplication::UnicodeUTF8));
    GroupBox3->setTitle(QApplication::translate("Renamer", "Key String", 0, QApplication::UnicodeUTF8));
    PushButton7->setText(QApplication::translate("Renamer", "&Delete ", 0, QApplication::UnicodeUTF8));
    PushButton12->setText(QApplication::translate("Renamer", "Is&Author", 0, QApplication::UnicodeUTF8));
    isMixButton->setText(QApplication::translate("Renamer", "Is&Mix", 0, QApplication::UnicodeUTF8));
    isMixButton->setShortcut(QApplication::translate("Renamer", "Alt+M", 0, QApplication::UnicodeUTF8));
    pushButton17->setText(QApplication::translate("Renamer", "Before '-' is author", 0, QApplication::UnicodeUTF8));
    pushButton18->setText(QApplication::translate("Renamer", "Key sel is auth", 0, QApplication::UnicodeUTF8));
    pushButton23->setText(QApplication::translate("Renamer", "'_' -> ' '", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(Renamer);
    } // retranslateUi

};

namespace Ui {
    class Renamer: public Ui_Renamer {};
} // namespace Ui

QT_END_NAMESPACE

class Renamer : public QDialog, public Ui::Renamer
{
    Q_OBJECT

public:
    Renamer(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0);
    ~Renamer();

public slots:
    virtual void capitalizeAfterSpace();
    virtual void changeSelection();
    virtual void deleteSubString();
    virtual void ignoreSelection();
    virtual void realizeSelection();
    virtual void removeFirstChar();
    virtual void removeSpaces();
    virtual void removeSpecials();
    virtual void replaceSubString();
    virtual void smallCapsInWords();
    virtual void subStringIsAuthor();
    virtual void beforeMinusIsAuthor();
    virtual void substringIsMix();
    virtual void keySelectionIsAuthor();
    virtual void betweenBracesIsTrash();
    virtual void removeFirstSpecials();
    virtual void replaceUnderscores();
    virtual void fixExtention();
    virtual void betweenBracesIsMix();

protected slots:
    virtual void languageChange();

};

#endif // RENAMER_H
