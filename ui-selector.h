/********************************************************************************
** Form generated from reading UI file 'ui-selector.ui'
**
** Created: Mon May 16 18:44:31 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_SELECTOR_H
#define UI_2D_SELECTOR_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QSplitter>
#include <QtGui/QTabWidget>
#include <QtGui/QTableView>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SongSelector
{
public:
    QVBoxLayout *vboxLayout;
    QSplitter *splitter;
    QWidget *layoutWidget1;
    QHBoxLayout *hboxLayout;
    QLCDNumber *mainLCD;
    QFrame *playerLayout;
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *playersLeft;
    QVBoxLayout *playersRight;
    QWidget *layoutWidget2;
    QVBoxLayout *vboxLayout1;
    QTabWidget *tabs;
    QWidget *tab1;
    QGridLayout *gridLayout;
    QTableView *songListView;
    QLineEdit *searchLine;
    QLCDNumber *countLcd;
    QSpacerItem *spacerItem;
    QWidget *TabPage1;
    QGridLayout *gridLayout_8;
    QTreeWidget *albumList;
    QLabel *textLabel1_4;
    QWidget *Tags;
    QGridLayout *gridLayout_1;
    QTreeWidget *tagList;
    QWidget *tab2;
    QGridLayout *gridLayout_5;
    QTreeWidget *queue;
    QHBoxLayout *horizontalLayout_5;
    QLabel *textLabel1;
    QSpinBox *extraSongs;
    QHBoxLayout *horizontalLayout_4;
    QCheckBox *weighDuplicateAuthors;
    QSpacerItem *spacerItem1;
    QPushButton *pushButton2;
    QWidget *TabPage2;
    QGridLayout *gridLayout_6;
    QTreeWidget *historyList;
    QTreeWidget *playedList;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *spacerItem2;
    QPushButton *pushButton116;
    QPushButton *pushButton114;
    QWidget *TabPage3;
    QGridLayout *gridLayout_3;
    QHBoxLayout *horizontalLayout;
    QFrame *analLayout1;
    QGridLayout *gridLayout_a;
    QVBoxLayout *verticalLayout;
    QFrame *analLayout2;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout_2;
    QTreeWidget *anal_queue;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *spacerItem3;
    QLCDNumber *eta_days;
    QLabel *textLabel1_5;
    QLCDNumber *eta_hours;
    QLabel *textLabel2_2;
    QLCDNumber *eta_minutes;
    QLabel *textLabel3_2;
    QLCDNumber *eta_seconds;
    QLabel *textLabel4_2;

    void setupUi(QDialog *SongSelector)
    {
        if (SongSelector->objectName().isEmpty())
            SongSelector->setObjectName(QString::fromUtf8("SongSelector"));
        SongSelector->resize(604, 531);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SongSelector->sizePolicy().hasHeightForWidth());
        SongSelector->setSizePolicy(sizePolicy);
        SongSelector->setMinimumSize(QSize(0, 531));
        SongSelector->setMaximumSize(QSize(16777215, 16777215));
        SongSelector->setContextMenuPolicy(Qt::CustomContextMenu);
        SongSelector->setSizeGripEnabled(true);
        vboxLayout = new QVBoxLayout(SongSelector);
        vboxLayout->setSpacing(3);
        vboxLayout->setContentsMargins(5, 5, 5, 5);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        splitter = new QSplitter(SongSelector);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Vertical);
        layoutWidget1 = new QWidget(splitter);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        hboxLayout = new QHBoxLayout(layoutWidget1);
        hboxLayout->setSpacing(2);
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        mainLCD = new QLCDNumber(layoutWidget1);
        mainLCD->setObjectName(QString::fromUtf8("mainLCD"));
        QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(mainLCD->sizePolicy().hasHeightForWidth());
        mainLCD->setSizePolicy(sizePolicy1);
        mainLCD->setAutoFillBackground(true);
        mainLCD->setLineWidth(0);
        mainLCD->setMidLineWidth(0);
        mainLCD->setNumDigits(3);
        mainLCD->setMode(QLCDNumber::Dec);
        mainLCD->setSegmentStyle(QLCDNumber::Flat);

        hboxLayout->addWidget(mainLCD);

        playerLayout = new QFrame(layoutWidget1);
        playerLayout->setObjectName(QString::fromUtf8("playerLayout"));
        playerLayout->setFrameShape(QFrame::StyledPanel);
        playerLayout->setFrameShadow(QFrame::Raised);
        gridLayout_4 = new QGridLayout(playerLayout);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        playersLeft = new QVBoxLayout();
        playersLeft->setSpacing(0);
        playersLeft->setObjectName(QString::fromUtf8("playersLeft"));

        horizontalLayout_3->addLayout(playersLeft);

        playersRight = new QVBoxLayout();
        playersRight->setSpacing(0);
        playersRight->setObjectName(QString::fromUtf8("playersRight"));

        horizontalLayout_3->addLayout(playersRight);


        gridLayout_4->addLayout(horizontalLayout_3, 0, 0, 1, 1);


        hboxLayout->addWidget(playerLayout);

        splitter->addWidget(layoutWidget1);
        layoutWidget2 = new QWidget(splitter);
        layoutWidget2->setObjectName(QString::fromUtf8("layoutWidget2"));
        vboxLayout1 = new QVBoxLayout(layoutWidget2);
        vboxLayout1->setSpacing(6);
        vboxLayout1->setContentsMargins(0, 0, 0, 0);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        vboxLayout1->setContentsMargins(0, 0, 0, 0);
        tabs = new QTabWidget(layoutWidget2);
        tabs->setObjectName(QString::fromUtf8("tabs"));
        tabs->setAutoFillBackground(true);
        tab1 = new QWidget();
        tab1->setObjectName(QString::fromUtf8("tab1"));
        gridLayout = new QGridLayout(tab1);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        songListView = new QTableView(tab1);
        songListView->setObjectName(QString::fromUtf8("songListView"));
        songListView->setContextMenuPolicy(Qt::CustomContextMenu);
        songListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        songListView->setProperty("showDropIndicator", QVariant(false));
        songListView->setDragDropOverwriteMode(false);
        songListView->setSelectionBehavior(QAbstractItemView::SelectRows);
        songListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        songListView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        songListView->setShowGrid(false);
        songListView->setGridStyle(Qt::NoPen);
        songListView->setSortingEnabled(true);
        songListView->setWordWrap(true);
        songListView->horizontalHeader()->setHighlightSections(false);
        songListView->verticalHeader()->setVisible(true);
        songListView->verticalHeader()->setHighlightSections(false);
        songListView->verticalHeader()->setMinimumSectionSize(0);

        gridLayout->addWidget(songListView, 0, 0, 1, 3);

        searchLine = new QLineEdit(tab1);
        searchLine->setObjectName(QString::fromUtf8("searchLine"));

        gridLayout->addWidget(searchLine, 1, 0, 1, 1);

        countLcd = new QLCDNumber(tab1);
        countLcd->setObjectName(QString::fromUtf8("countLcd"));
        countLcd->setAutoFillBackground(true);

        gridLayout->addWidget(countLcd, 1, 1, 1, 1);

        spacerItem = new QSpacerItem(430, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(spacerItem, 1, 2, 1, 1);

        tabs->addTab(tab1, QString());
        TabPage1 = new QWidget();
        TabPage1->setObjectName(QString::fromUtf8("TabPage1"));
        gridLayout_8 = new QGridLayout(TabPage1);
        gridLayout_8->setSpacing(6);
        gridLayout_8->setContentsMargins(11, 11, 11, 11);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        albumList = new QTreeWidget(TabPage1);
        albumList->setObjectName(QString::fromUtf8("albumList"));
        albumList->setContextMenuPolicy(Qt::CustomContextMenu);
        albumList->setEditTriggers(QAbstractItemView::AnyKeyPressed|QAbstractItemView::SelectedClicked);
        albumList->setSortingEnabled(true);
        albumList->header()->setProperty("showSortIndicator", QVariant(true));

        gridLayout_8->addWidget(albumList, 0, 0, 1, 1);

        textLabel1_4 = new QLabel(TabPage1);
        textLabel1_4->setObjectName(QString::fromUtf8("textLabel1_4"));
        textLabel1_4->setWordWrap(false);

        gridLayout_8->addWidget(textLabel1_4, 1, 0, 1, 1);

        tabs->addTab(TabPage1, QString());
        Tags = new QWidget();
        Tags->setObjectName(QString::fromUtf8("Tags"));
        gridLayout_1 = new QGridLayout(Tags);
        gridLayout_1->setSpacing(6);
        gridLayout_1->setContentsMargins(11, 11, 11, 11);
        gridLayout_1->setObjectName(QString::fromUtf8("gridLayout_1"));
        tagList = new QTreeWidget(Tags);
        tagList->setObjectName(QString::fromUtf8("tagList"));
        tagList->setMouseTracking(true);
        tagList->setSortingEnabled(true);
        tagList->header()->setCascadingSectionResizes(true);
        tagList->header()->setProperty("showSortIndicator", QVariant(true));

        gridLayout_1->addWidget(tagList, 0, 0, 1, 1);

        tabs->addTab(Tags, QString());
        tab2 = new QWidget();
        tab2->setObjectName(QString::fromUtf8("tab2"));
        gridLayout_5 = new QGridLayout(tab2);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        queue = new QTreeWidget(tab2);
        queue->setObjectName(QString::fromUtf8("queue"));
        queue->setContextMenuPolicy(Qt::CustomContextMenu);
        queue->setSelectionMode(QAbstractItemView::ExtendedSelection);
        queue->setAutoExpandDelay(0);

        gridLayout_5->addWidget(queue, 0, 0, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        textLabel1 = new QLabel(tab2);
        textLabel1->setObjectName(QString::fromUtf8("textLabel1"));
        textLabel1->setWordWrap(false);

        horizontalLayout_5->addWidget(textLabel1);

        extraSongs = new QSpinBox(tab2);
        extraSongs->setObjectName(QString::fromUtf8("extraSongs"));

        horizontalLayout_5->addWidget(extraSongs);


        gridLayout_5->addLayout(horizontalLayout_5, 1, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        weighDuplicateAuthors = new QCheckBox(tab2);
        weighDuplicateAuthors->setObjectName(QString::fromUtf8("weighDuplicateAuthors"));
        weighDuplicateAuthors->setChecked(true);

        horizontalLayout_4->addWidget(weighDuplicateAuthors);

        spacerItem1 = new QSpacerItem(140, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(spacerItem1);

        pushButton2 = new QPushButton(tab2);
        pushButton2->setObjectName(QString::fromUtf8("pushButton2"));

        horizontalLayout_4->addWidget(pushButton2);


        gridLayout_5->addLayout(horizontalLayout_4, 2, 0, 1, 1);

        tabs->addTab(tab2, QString());
        TabPage2 = new QWidget();
        TabPage2->setObjectName(QString::fromUtf8("TabPage2"));
        gridLayout_6 = new QGridLayout(TabPage2);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        historyList = new QTreeWidget(TabPage2);
        historyList->setObjectName(QString::fromUtf8("historyList"));
        historyList->setSelectionMode(QAbstractItemView::ExtendedSelection);

        gridLayout_6->addWidget(historyList, 0, 0, 1, 1);

        playedList = new QTreeWidget(TabPage2);
        playedList->setObjectName(QString::fromUtf8("playedList"));

        gridLayout_6->addWidget(playedList, 1, 0, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        spacerItem2 = new QSpacerItem(500, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(spacerItem2);

        pushButton116 = new QPushButton(TabPage2);
        pushButton116->setObjectName(QString::fromUtf8("pushButton116"));

        horizontalLayout_6->addWidget(pushButton116);

        pushButton114 = new QPushButton(TabPage2);
        pushButton114->setObjectName(QString::fromUtf8("pushButton114"));

        horizontalLayout_6->addWidget(pushButton114);


        gridLayout_6->addLayout(horizontalLayout_6, 2, 0, 1, 1);

        tabs->addTab(TabPage2, QString());
        TabPage3 = new QWidget();
        TabPage3->setObjectName(QString::fromUtf8("TabPage3"));
        gridLayout_3 = new QGridLayout(TabPage3);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        analLayout1 = new QFrame(TabPage3);
        analLayout1->setObjectName(QString::fromUtf8("analLayout1"));
        analLayout1->setMaximumSize(QSize(16777215, 199));
        analLayout1->setFrameShape(QFrame::StyledPanel);
        analLayout1->setFrameShadow(QFrame::Raised);
        gridLayout_a = new QGridLayout(analLayout1);
        gridLayout_a->setSpacing(0);
        gridLayout_a->setContentsMargins(0, 0, 0, 0);
        gridLayout_a->setObjectName(QString::fromUtf8("gridLayout_a"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

        gridLayout_a->addLayout(verticalLayout, 0, 0, 1, 1);


        horizontalLayout->addWidget(analLayout1);

        analLayout2 = new QFrame(TabPage3);
        analLayout2->setObjectName(QString::fromUtf8("analLayout2"));
        analLayout2->setFrameShape(QFrame::StyledPanel);
        analLayout2->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(analLayout2);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));

        gridLayout_2->addLayout(verticalLayout_2, 0, 0, 1, 1);


        horizontalLayout->addWidget(analLayout2);


        gridLayout_3->addLayout(horizontalLayout, 0, 0, 1, 1);

        anal_queue = new QTreeWidget(TabPage3);
        anal_queue->setObjectName(QString::fromUtf8("anal_queue"));

        gridLayout_3->addWidget(anal_queue, 1, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        spacerItem3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(spacerItem3);

        eta_days = new QLCDNumber(TabPage3);
        eta_days->setObjectName(QString::fromUtf8("eta_days"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Maximum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(eta_days->sizePolicy().hasHeightForWidth());
        eta_days->setSizePolicy(sizePolicy2);
        eta_days->setNumDigits(3);

        horizontalLayout_2->addWidget(eta_days);

        textLabel1_5 = new QLabel(TabPage3);
        textLabel1_5->setObjectName(QString::fromUtf8("textLabel1_5"));
        QFont font;
        font.setPointSize(10);
        textLabel1_5->setFont(font);
        textLabel1_5->setWordWrap(false);

        horizontalLayout_2->addWidget(textLabel1_5);

        eta_hours = new QLCDNumber(TabPage3);
        eta_hours->setObjectName(QString::fromUtf8("eta_hours"));
        sizePolicy2.setHeightForWidth(eta_hours->sizePolicy().hasHeightForWidth());
        eta_hours->setSizePolicy(sizePolicy2);
        eta_hours->setNumDigits(2);

        horizontalLayout_2->addWidget(eta_hours);

        textLabel2_2 = new QLabel(TabPage3);
        textLabel2_2->setObjectName(QString::fromUtf8("textLabel2_2"));
        textLabel2_2->setFont(font);
        textLabel2_2->setWordWrap(false);

        horizontalLayout_2->addWidget(textLabel2_2);

        eta_minutes = new QLCDNumber(TabPage3);
        eta_minutes->setObjectName(QString::fromUtf8("eta_minutes"));
        sizePolicy2.setHeightForWidth(eta_minutes->sizePolicy().hasHeightForWidth());
        eta_minutes->setSizePolicy(sizePolicy2);
        eta_minutes->setNumDigits(2);

        horizontalLayout_2->addWidget(eta_minutes);

        textLabel3_2 = new QLabel(TabPage3);
        textLabel3_2->setObjectName(QString::fromUtf8("textLabel3_2"));
        textLabel3_2->setFont(font);
        textLabel3_2->setWordWrap(false);

        horizontalLayout_2->addWidget(textLabel3_2);

        eta_seconds = new QLCDNumber(TabPage3);
        eta_seconds->setObjectName(QString::fromUtf8("eta_seconds"));
        sizePolicy2.setHeightForWidth(eta_seconds->sizePolicy().hasHeightForWidth());
        eta_seconds->setSizePolicy(sizePolicy2);
        eta_seconds->setNumDigits(2);

        horizontalLayout_2->addWidget(eta_seconds);

        textLabel4_2 = new QLabel(TabPage3);
        textLabel4_2->setObjectName(QString::fromUtf8("textLabel4_2"));
        textLabel4_2->setFont(font);
        textLabel4_2->setWordWrap(false);

        horizontalLayout_2->addWidget(textLabel4_2);


        gridLayout_3->addLayout(horizontalLayout_2, 2, 0, 1, 1);

        tabs->addTab(TabPage3, QString());

        vboxLayout1->addWidget(tabs);

        splitter->addWidget(layoutWidget2);

        vboxLayout->addWidget(splitter);

        QWidget::setTabOrder(searchLine, pushButton2);
        QWidget::setTabOrder(pushButton2, extraSongs);
        QWidget::setTabOrder(extraSongs, weighDuplicateAuthors);

        retranslateUi(SongSelector);
        QObject::connect(pushButton2, SIGNAL(clicked()), SongSelector, SLOT(doAutoMix()));
        QObject::connect(searchLine, SIGNAL(returnPressed()), SongSelector, SLOT(searchLineEntered()));
        QObject::connect(tagList, SIGNAL(itemClicked(QTreeWidgetItem*,int)), SongSelector, SLOT(changeTagList(QTreeWidgetItem*,int)));
        QObject::connect(pushButton114, SIGNAL(clicked()), SongSelector, SLOT(clearPlayHistory()));
        QObject::connect(pushButton116, SIGNAL(clicked()), SongSelector, SLOT(savePlayHistory()));
        QObject::connect(queue, SIGNAL(customContextMenuRequested(QPoint)), SongSelector, SLOT(openQueueMenu()));
        QObject::connect(historyList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SongSelector, SLOT(playTreeSong(QTreeWidgetItem*)));
        QObject::connect(queue, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SongSelector, SLOT(playTreeSong(QTreeWidgetItem*)));
        QObject::connect(albumList, SIGNAL(itemChanged(QTreeWidgetItem*,int)), SongSelector, SLOT(albumItemChanged(QTreeWidgetItem*,int)));
        QObject::connect(albumList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SongSelector, SLOT(selectAlbumItem(QTreeWidgetItem*)));

        tabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SongSelector);
    } // setupUi

    void retranslateUi(QDialog *SongSelector)
    {
        SongSelector->setWindowTitle(QApplication::translate("SongSelector", "BpmDj Song Selector", 0, QApplication::UnicodeUTF8));
        tabs->setTabText(tabs->indexOf(tab1), QApplication::translate("SongSelector", "Songs", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = albumList->headerItem();
        ___qtreewidgetitem->setText(5, QApplication::translate("SongSelector", "File", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(4, QApplication::translate("SongSelector", "Index", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(3, QApplication::translate("SongSelector", "Time", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(2, QApplication::translate("SongSelector", "Author", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(1, QApplication::translate("SongSelector", "Title", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("SongSelector", "Album", 0, QApplication::UnicodeUTF8));
        textLabel1_4->setText(QApplication::translate("SongSelector", "Double click the albumname album to add all its songs to the queue", 0, QApplication::UnicodeUTF8));
        tabs->setTabText(tabs->indexOf(TabPage1), QApplication::translate("SongSelector", "Albums", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem1 = tagList->headerItem();
        ___qtreewidgetitem1->setText(3, QApplication::translate("SongSelector", "Exclude", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem1->setText(2, QApplication::translate("SongSelector", "Must Be Present", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem1->setText(1, QApplication::translate("SongSelector", "Include", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem1->setText(0, QApplication::translate("SongSelector", "Tag", 0, QApplication::UnicodeUTF8));
        tabs->setTabText(tabs->indexOf(Tags), QApplication::translate("SongSelector", "Tags", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem2 = queue->headerItem();
        ___qtreewidgetitem2->setText(10, QApplication::translate("SongSelector", "OnDisk", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem2->setText(9, QApplication::translate("SongSelector", "Tags", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem2->setText(8, QApplication::translate("SongSelector", "Version", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem2->setText(7, QApplication::translate("SongSelector", "Author", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem2->setText(6, QApplication::translate("SongSelector", "Title", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem2->setText(5, QApplication::translate("SongSelector", "Cues", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem2->setText(4, QApplication::translate("SongSelector", "Time", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem2->setText(3, QApplication::translate("SongSelector", "Spectrum", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem2->setText(2, QApplication::translate("SongSelector", "Tempo", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem2->setText(1, QApplication::translate("SongSelector", "Match", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem2->setText(0, QApplication::translate("SongSelector", "Nr", 0, QApplication::UnicodeUTF8));
        textLabel1->setText(QApplication::translate("SongSelector", "Number of extra songs to insert when adding a song to the queue", 0, QApplication::UnicodeUTF8));
        weighDuplicateAuthors->setText(QApplication::translate("SongSelector", "No duplicate authors", 0, QApplication::UnicodeUTF8));
        pushButton2->setText(QApplication::translate("SongSelector", "Generate process_mix.sh for an automatic mix", 0, QApplication::UnicodeUTF8));
        tabs->setTabText(tabs->indexOf(tab2), QApplication::translate("SongSelector", "Queue", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem3 = historyList->headerItem();
        ___qtreewidgetitem3->setText(10, QApplication::translate("SongSelector", "Tags", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem3->setText(9, QApplication::translate("SongSelector", "Version", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem3->setText(8, QApplication::translate("SongSelector", "Author", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem3->setText(7, QApplication::translate("SongSelector", "Title", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem3->setText(6, QApplication::translate("SongSelector", "Cues", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem3->setText(5, QApplication::translate("SongSelector", "Time", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem3->setText(4, QApplication::translate("SongSelector", "Color", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem3->setText(3, QApplication::translate("SongSelector", "Spectrum", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem3->setText(2, QApplication::translate("SongSelector", "Tempo", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem3->setText(1, QApplication::translate("SongSelector", "Comments", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem3->setText(0, QApplication::translate("SongSelector", "Rel", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem4 = playedList->headerItem();
        ___qtreewidgetitem4->setText(2, QApplication::translate("SongSelector", "Author", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem4->setText(1, QApplication::translate("SongSelector", "Title", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem4->setText(0, QApplication::translate("SongSelector", "Nr", 0, QApplication::UnicodeUTF8));
        pushButton116->setText(QApplication::translate("SongSelector", "Save History", 0, QApplication::UnicodeUTF8));
        pushButton114->setText(QApplication::translate("SongSelector", "Clear History", 0, QApplication::UnicodeUTF8));
        tabs->setTabText(tabs->indexOf(TabPage2), QApplication::translate("SongSelector", "History", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem5 = anal_queue->headerItem();
        ___qtreewidgetitem5->setText(8, QApplication::translate("SongSelector", "Index", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem5->setText(7, QApplication::translate("SongSelector", "Composition", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem5->setText(6, QApplication::translate("SongSelector", "Rhythm", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem5->setText(5, QApplication::translate("SongSelector", "Tempo", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem5->setText(4, QApplication::translate("SongSelector", "Echo", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem5->setText(3, QApplication::translate("SongSelector", "Spectrum", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem5->setText(2, QApplication::translate("SongSelector", "RMS", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem5->setText(1, QApplication::translate("SongSelector", "Author", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem5->setText(0, QApplication::translate("SongSelector", "Title", 0, QApplication::UnicodeUTF8));
        textLabel1_5->setText(QApplication::translate("SongSelector", "days", 0, QApplication::UnicodeUTF8));
        textLabel2_2->setText(QApplication::translate("SongSelector", "hours", 0, QApplication::UnicodeUTF8));
        textLabel3_2->setText(QApplication::translate("SongSelector", "minutes", 0, QApplication::UnicodeUTF8));
        textLabel4_2->setText(QApplication::translate("SongSelector", "seconds remaining", 0, QApplication::UnicodeUTF8));
        tabs->setTabText(tabs->indexOf(TabPage3), QApplication::translate("SongSelector", "Analysis", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SongSelector: public Ui_SongSelector {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_SELECTOR_H
