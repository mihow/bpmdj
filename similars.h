#ifndef SIMILARS_H
#define SIMILARS_H

#include <qvariant.h>


#include <Qt3Support/Q3Header>
#include <Qt3Support/Q3ListView>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <Qt3Support/Q3MimeSourceFactory>

class Ui_Similars
{
public:
    QGridLayout *gridLayout;
    QLabel *textLabel1;
    Q3ListView *NameList;
    QPushButton *ignoreButton;
    QPushButton *fetchButton;
    QPushButton *finishedButton;
    QFrame *line1;
    QPushButton *startStopButton;
    QPushButton *deleteButton;
    QSpacerItem *spacerItem;
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout;
    QLabel *TextLabel2;
    QLabel *textLabel3;
    QVBoxLayout *vboxLayout1;
    QLabel *root;
    QLabel *currentFile;
    QFrame *line2;
    QCheckBox *conformingFilesOnly;
    QCheckBox *autoFetch;

    void setupUi(QDialog *Similars)
    {
    Similars->setObjectName(QString::fromUtf8("Similars"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(3));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(Similars->sizePolicy().hasHeightForWidth());
    Similars->setSizePolicy(sizePolicy);
    gridLayout = new QGridLayout(Similars);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(11);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    textLabel1 = new QLabel(Similars);
    textLabel1->setObjectName(QString::fromUtf8("textLabel1"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(1));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(textLabel1->sizePolicy().hasHeightForWidth());
    textLabel1->setSizePolicy(sizePolicy1);
    textLabel1->setAlignment(Qt::AlignVCenter);
    textLabel1->setWordWrap(true);

    gridLayout->addWidget(textLabel1, 0, 0, 1, 2);

    NameList = new Q3ListView(Similars);
    NameList->addColumn(QApplication::translate("Similars", "Songname", 0, QApplication::UnicodeUTF8));
    NameList->header()->setClickEnabled(true, NameList->header()->count() - 1);
    NameList->header()->setResizeEnabled(true, NameList->header()->count() - 1);
    NameList->addColumn(QApplication::translate("Similars", "Similar", 0, QApplication::UnicodeUTF8));
    NameList->header()->setClickEnabled(true, NameList->header()->count() - 1);
    NameList->header()->setResizeEnabled(true, NameList->header()->count() - 1);
    NameList->addColumn(QApplication::translate("Similars", "Edit Distance", 0, QApplication::UnicodeUTF8));
    NameList->header()->setClickEnabled(true, NameList->header()->count() - 1);
    NameList->header()->setResizeEnabled(true, NameList->header()->count() - 1);
    NameList->addColumn(QApplication::translate("Similars", "Full Songname", 0, QApplication::UnicodeUTF8));
    NameList->header()->setClickEnabled(true, NameList->header()->count() - 1);
    NameList->header()->setResizeEnabled(true, NameList->header()->count() - 1);
    NameList->addColumn(QApplication::translate("Similars", "Full Similar Name", 0, QApplication::UnicodeUTF8));
    NameList->header()->setClickEnabled(true, NameList->header()->count() - 1);
    NameList->header()->setResizeEnabled(true, NameList->header()->count() - 1);
    NameList->setObjectName(QString::fromUtf8("NameList"));
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(3));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(NameList->sizePolicy().hasHeightForWidth());
    NameList->setSizePolicy(sizePolicy2);
    QFont font;
    font.setFamily(QString::fromUtf8("Charter"));
    NameList->setFont(font);
    NameList->setFrameShape(QFrame::NoFrame);
    NameList->setSelectionMode(Q3ListView::Extended);
    NameList->setAllColumnsShowFocus(true);

    gridLayout->addWidget(NameList, 5, 0, 7, 1);

    ignoreButton = new QPushButton(Similars);
    ignoreButton->setObjectName(QString::fromUtf8("ignoreButton"));
    ignoreButton->setEnabled(true);

    gridLayout->addWidget(ignoreButton, 8, 1, 1, 1);

    fetchButton = new QPushButton(Similars);
    fetchButton->setObjectName(QString::fromUtf8("fetchButton"));

    gridLayout->addWidget(fetchButton, 7, 1, 1, 1);

    finishedButton = new QPushButton(Similars);
    finishedButton->setObjectName(QString::fromUtf8("finishedButton"));

    gridLayout->addWidget(finishedButton, 11, 1, 1, 1);

    line1 = new QFrame(Similars);
    line1->setObjectName(QString::fromUtf8("line1"));
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);

    gridLayout->addWidget(line1, 9, 1, 1, 1);

    startStopButton = new QPushButton(Similars);
    startStopButton->setObjectName(QString::fromUtf8("startStopButton"));

    gridLayout->addWidget(startStopButton, 10, 1, 1, 1);

    deleteButton = new QPushButton(Similars);
    deleteButton->setObjectName(QString::fromUtf8("deleteButton"));

    gridLayout->addWidget(deleteButton, 6, 1, 1, 1);

    spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem, 5, 1, 1, 1);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    vboxLayout = new QVBoxLayout();
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(0);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    TextLabel2 = new QLabel(Similars);
    TextLabel2->setObjectName(QString::fromUtf8("TextLabel2"));
    QSizePolicy sizePolicy3(static_cast<QSizePolicy::Policy>(1), static_cast<QSizePolicy::Policy>(1));
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(TextLabel2->sizePolicy().hasHeightForWidth());
    TextLabel2->setSizePolicy(sizePolicy3);
    TextLabel2->setWordWrap(false);

    vboxLayout->addWidget(TextLabel2);

    textLabel3 = new QLabel(Similars);
    textLabel3->setObjectName(QString::fromUtf8("textLabel3"));
    QSizePolicy sizePolicy4(static_cast<QSizePolicy::Policy>(1), static_cast<QSizePolicy::Policy>(1));
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(textLabel3->sizePolicy().hasHeightForWidth());
    textLabel3->setSizePolicy(sizePolicy4);
    textLabel3->setWordWrap(false);

    vboxLayout->addWidget(textLabel3);


    hboxLayout->addLayout(vboxLayout);

    vboxLayout1 = new QVBoxLayout();
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(0);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    root = new QLabel(Similars);
    root->setObjectName(QString::fromUtf8("root"));
    QSizePolicy sizePolicy5(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(1));
    sizePolicy5.setHorizontalStretch(0);
    sizePolicy5.setVerticalStretch(0);
    sizePolicy5.setHeightForWidth(root->sizePolicy().hasHeightForWidth());
    root->setSizePolicy(sizePolicy5);
    root->setWordWrap(false);

    vboxLayout1->addWidget(root);

    currentFile = new QLabel(Similars);
    currentFile->setObjectName(QString::fromUtf8("currentFile"));
    QSizePolicy sizePolicy6(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(1));
    sizePolicy6.setHorizontalStretch(0);
    sizePolicy6.setVerticalStretch(0);
    sizePolicy6.setHeightForWidth(currentFile->sizePolicy().hasHeightForWidth());
    currentFile->setSizePolicy(sizePolicy6);
    currentFile->setWordWrap(false);

    vboxLayout1->addWidget(currentFile);


    hboxLayout->addLayout(vboxLayout1);


    gridLayout->addLayout(hboxLayout, 1, 0, 1, 2);

    line2 = new QFrame(Similars);
    line2->setObjectName(QString::fromUtf8("line2"));
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);

    gridLayout->addWidget(line2, 2, 0, 1, 2);

    conformingFilesOnly = new QCheckBox(Similars);
    conformingFilesOnly->setObjectName(QString::fromUtf8("conformingFilesOnly"));

    gridLayout->addWidget(conformingFilesOnly, 4, 0, 1, 2);

    autoFetch = new QCheckBox(Similars);
    autoFetch->setObjectName(QString::fromUtf8("autoFetch"));

    gridLayout->addWidget(autoFetch, 3, 0, 1, 2);


    retranslateUi(Similars);

    QSize size(751, 536);
    size = size.expandedTo(Similars->minimumSizeHint());
    Similars->resize(size);

    QObject::connect(fetchButton, SIGNAL(clicked()), Similars, SLOT(goFetch()));
    QObject::connect(ignoreButton, SIGNAL(clicked()), Similars, SLOT(ignore()));
    QObject::connect(finishedButton, SIGNAL(clicked()), Similars, SLOT(accept()));
    QObject::connect(startStopButton, SIGNAL(clicked()), Similars, SLOT(startStop()));
    QObject::connect(deleteButton, SIGNAL(clicked()), Similars, SLOT(alreadyHave()));

    QMetaObject::connectSlotsByName(Similars);
    } // setupUi

    void retranslateUi(QDialog *Similars)
    {
    Similars->setWindowTitle(QApplication::translate("Similars", "Similar Name Scanner", 0, QApplication::UnicodeUTF8));
    textLabel1->setText(QApplication::translate("Similars", "In the box below songs will appear that are maybe songs that are already in the database. If you already have the song, you can press 'delete'. If a song is new, you can press 'fech', or if you don't know what to do yet, press 'ignore'.", 0, QApplication::UnicodeUTF8));
    NameList->header()->setLabel(0, QApplication::translate("Similars", "Songname", 0, QApplication::UnicodeUTF8));
    NameList->header()->setLabel(1, QApplication::translate("Similars", "Similar", 0, QApplication::UnicodeUTF8));
    NameList->header()->setLabel(2, QApplication::translate("Similars", "Edit Distance", 0, QApplication::UnicodeUTF8));
    NameList->header()->setLabel(3, QApplication::translate("Similars", "Full Songname", 0, QApplication::UnicodeUTF8));
    NameList->header()->setLabel(4, QApplication::translate("Similars", "Full Similar Name", 0, QApplication::UnicodeUTF8));
    ignoreButton->setText(QApplication::translate("Similars", "&Ignore", 0, QApplication::UnicodeUTF8));
    ignoreButton->setProperty("toolTip", QVariant(QApplication::translate("Similars", "Don't know yet, ignore for later", 0, QApplication::UnicodeUTF8)));
    fetchButton->setText(QApplication::translate("Similars", "&Fetch", 0, QApplication::UnicodeUTF8));
    fetchButton->setProperty("toolTip", QVariant(QApplication::translate("Similars", "Probably a new song", 0, QApplication::UnicodeUTF8)));
    finishedButton->setText(QApplication::translate("Similars", "Finished", 0, QApplication::UnicodeUTF8));
    finishedButton->setProperty("toolTip", QVariant(QString()));
    startStopButton->setText(QApplication::translate("Similars", "Start", 0, QApplication::UnicodeUTF8));
    deleteButton->setText(QApplication::translate("Similars", "&Delete", 0, QApplication::UnicodeUTF8));
    deleteButton->setProperty("toolTip", QVariant(QApplication::translate("Similars", "Already have the song, delete it", 0, QApplication::UnicodeUTF8)));
    TextLabel2->setText(QApplication::translate("Similars", "Root:", 0, QApplication::UnicodeUTF8));
    textLabel3->setText(QApplication::translate("Similars", "Current:", 0, QApplication::UnicodeUTF8));
    root->setText(QApplication::translate("Similars", "root directory", 0, QApplication::UnicodeUTF8));
    currentFile->setText(QApplication::translate("Similars", "textLabel2", 0, QApplication::UnicodeUTF8));
    conformingFilesOnly->setText(QApplication::translate("Similars", "Only investigate conforming filenames", 0, QApplication::UnicodeUTF8));
    autoFetch->setText(QApplication::translate("Similars", "Automatically rename really unknown songs to *.tofetch", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(Similars);
    } // retranslateUi

};

namespace Ui {
    class Similars: public Ui_Similars {};
} // namespace Ui

class Similars : public QDialog, public Ui::Similars
{
    Q_OBJECT

public:
    Similars(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0);
    ~Similars();

public slots:
    virtual void startStop();
    virtual void goFetch();
    virtual void ignore();
    virtual void alreadyHave();

protected slots:
    virtual void languageChange();

};

#endif // SIMILARS_H
