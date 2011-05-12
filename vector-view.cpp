#include "qvectorview.h"
#include "vector-view.h"
#include <qstring.h>
#include <qapp.h>
//#include "songselector.h"
#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qheader.h>
#include <qlistview.h>
#include <qlcdnumber.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QTabWidget;
class QWidget;
class QListView;
class QListViewItem;
class QLCDNumber;
class QLineEdit;
class QLabel;
class QSpinBox;
class QPushButton;
class QCheckBox;
class QFrame;

// ------

MainWindow::MainWindow( QWidget* parent, const char* name, bool modal, WFlags fl )
  : QDialog( parent, name, modal, fl )
{
  if ( !name )
    setName( "MainWindow" );
  setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, sizePolicy().hasHeightForWidth() ) );
  setSizeGripEnabled( TRUE );
  MainWindowLayout = new QGridLayout( this, 1, 1, 11, 6, "MainWindowLayout"); 
  QSpacerItem* spacer = new QSpacerItem( 20, 30, QSizePolicy::Minimum, QSizePolicy::Fixed );
  MainWindowLayout->addItem( spacer, 0, 0 );
  
  tabs = new QTabWidget( this, "tabs" );
  
  tab = new QWidget( tabs, "tab" );
  tabLayout = new QGridLayout( tab, 1, 1, 11, 6, "tabLayout"); 
  
  songList = new QVectorView( tab );
  songList->addColumn( tr( "Tempo" ) );
  songList->addColumn( tr( "Spectrum" ) );
  songList->addColumn( tr( "dColor" ) );
  songList->addColumn( tr( "Time" ) );
  songList->addColumn( tr( "Cues" ) );
  songList->addColumn( tr( "Title" ) );
  songList->addColumn( tr( "Author" ) );
  songList->addColumn( tr( "Version" ) );
  songList->addColumn( tr( "Tags" ) );
  songList->addColumn( tr( "OnDisk" ) );
  songList->addColumn( tr( "Index" ) );
  songList->addColumn( tr( "Md5" ) );
  songList->addColumn( tr( "Filename on disk" ) );
  songList->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, songList->sizePolicy().hasHeightForWidth() ) );
  songList->setFrameShape( QVectorView::Panel );
  songList->setResizePolicy( QScrollView::Manual );
  
  tabLayout->addMultiCellWidget( songList, 0, 0, 0, 2 );
  
  searchLine = new QLineEdit( tab, "searchLine" );
  
  tabLayout->addWidget( searchLine, 1, 0 );
  QSpacerItem* spacer_2 = new QSpacerItem( 430, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  tabLayout->addItem( spacer_2, 1, 2 );
  tabs->insertTab( tab, QString("") );
  
  TabPage = new QWidget( tabs, "TabPage" );
  TabPageLayout = new QGridLayout( TabPage, 1, 1, 11, 6, "TabPageLayout"); 
  
  albumList = new QListView( TabPage, "albumList" );
  albumList->addColumn( tr( "Album" ) );
  albumList->addColumn( tr( "Title" ) );
  albumList->addColumn( tr( "Author" ) );
  albumList->addColumn( tr( "Time" ) );
  albumList->addColumn( tr( "Index" ) );
  albumList->addColumn( tr( "File" ) );
  albumList->setSelectionMode( QListView::Single );
  albumList->setAllColumnsShowFocus( TRUE );
  albumList->setRootIsDecorated( FALSE );
  albumList->setDefaultRenameAction( QListView::Accept );
  
  TabPageLayout->addWidget( albumList, 0, 0 );
  
  tabs->insertTab( TabPage, QString("") );
  
  Tags = new QWidget( tabs, "Tags" );
  TagsLayout = new QGridLayout( Tags, 1, 1, 11, 6, "TagsLayout"); 
  
  tagList = new QListView( Tags, "tagList" );
  tagList->addColumn( tr( "Tag" ) );
  tagList->addColumn( tr( "Include" ) );
  tagList->addColumn( tr( "Must Be Present" ) );
  tagList->addColumn( tr( "Exclude" ) );
  tagList->setAllColumnsShowFocus( TRUE );
  tagList->setShowSortIndicator( FALSE );
  
  TagsLayout->addWidget( tagList, 0, 0 );
  tabs->insertTab( Tags, QString("") );
  
  tab_2 = new QWidget( tabs, "tab_2" );
  tabLayout_2 = new QGridLayout( tab_2, 1, 1, 11, 6, "tabLayout_2"); 
  
  extraSongs = new QSpinBox( tab_2, "extraSongs" );

  tabLayout_2->addWidget( extraSongs, 1, 7 );
  pushButton2 = new QPushButton( tab_2, "pushButton2" );
  
  tabLayout_2->addWidget( pushButton2, 2, 7 );
  
  tempoWeight = new QSpinBox( tab_2, "tempoWeight" );
  tempoWeight->setValue( 1 );
  
  tabLayout_2->addWidget( tempoWeight, 2, 4 );
  
  spectrumWeight = new QSpinBox( tab_2, "spectrumWeight" );
  spectrumWeight->setValue( 1 );

  tabLayout_2->addWidget( spectrumWeight, 2, 6 );
  
  weighDuplicateAuthors = new QCheckBox( tab_2, "weighDuplicateAuthors" );
  weighDuplicateAuthors->setChecked( TRUE );

  tabLayout_2->addMultiCellWidget( weighDuplicateAuthors, 2, 2, 1, 2 );
  QSpacerItem* spacer_3 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  tabLayout_2->addMultiCell( spacer_3, 1, 1, 0, 1 );
  QSpacerItem* spacer_4 = new QSpacerItem( 140, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  tabLayout_2->addItem( spacer_4, 2, 0 );
  tabs->insertTab( tab_2, QString("") );
  
  TabPage_2 = new QWidget( tabs, "TabPage_2" );
  TabPageLayout_2 = new QGridLayout( TabPage_2, 1, 1, 11, 6, "TabPageLayout_2"); 
  
  tabs->insertTab( TabPage_2, QString("") );
  
  TabPage_3 = new QWidget( tabs, "TabPage_3" );
  TabPageLayout_3 = new QGridLayout( TabPage_3, 1, 1, 11, 6, "TabPageLayout_3"); 
  
  frequency_map = new QLabel( TabPage_3, "frequency_map" );
  frequency_map->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 0, frequency_map->sizePolicy().hasHeightForWidth() ) );
  frequency_map->setScaledContents( TRUE );
  
  TabPageLayout_3->addWidget( frequency_map, 0, 0 );
  tabs->insertTab( TabPage_3, QString("") );
  
  MainWindowLayout->addWidget( tabs, 2, 0 );
  
  frame3 = new QFrame( this, "frame3" );
  frame3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 0, 0, frame3->sizePolicy().hasHeightForWidth() ) );
  frame3->setFrameShape( QFrame::StyledPanel );
  frame3->setFrameShadow( QFrame::Raised );
  frame3Layout = new QGridLayout( frame3, 1, 1, 11, 6, "frame3Layout"); 
  
  mainLCD = new QLCDNumber( frame3, "mainLCD" );
  mainLCD->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, mainLCD->sizePolicy().hasHeightForWidth() ) );
  mainLCD->setLineWidth( 0 );
  mainLCD->setMidLineWidth( 0 );
  mainLCD->setNumDigits( 3 );
  mainLCD->setMode( QLCDNumber::Dec );
  mainLCD->setSegmentStyle( QLCDNumber::Flat );
  
  frame3Layout->addWidget( mainLCD, 0, 0 );

  layout10 = new QVBoxLayout( 0, 0, 6, "layout10"); 
  
  tempo1 = new QLabel( frame3, "tempo1" );
  tempo1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, tempo1->sizePolicy().hasHeightForWidth() ) );
  tempo1->setBackgroundOrigin( QLabel::ParentOrigin );
  layout10->addWidget( tempo1 );
  
  tempo2 = new QLabel( frame3, "tempo2" );
  tempo2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, tempo2->sizePolicy().hasHeightForWidth() ) );
  tempo2->setBackgroundOrigin( QLabel::ParentOrigin );
  layout10->addWidget( tempo2 );

  tempo3 = new QLabel( frame3, "tempo3" );
  tempo3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, tempo3->sizePolicy().hasHeightForWidth() ) );
  tempo3->setBackgroundOrigin( QLabel::ParentOrigin );
  layout10->addWidget( tempo3 );

  tempo4 = new QLabel( frame3, "tempo4" );
  tempo4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, tempo4->sizePolicy().hasHeightForWidth() ) );
  tempo4->setBackgroundOrigin( QLabel::ParentOrigin );
  layout10->addWidget( tempo4 );

  frame3Layout->addLayout( layout10, 0, 2 );

  layout9 = new QVBoxLayout( 0, 0, 6, "layout9"); 
  
  frame3Layout->addLayout( layout9, 0, 1 );

  layout11 = new QVBoxLayout( 0, 0, 6, "layout11"); 

  title1 = new QLabel( frame3, "title1" );
  title1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, 0, 0, title1->sizePolicy().hasHeightForWidth() ) );
  title1->setBackgroundOrigin( QLabel::ParentOrigin );
  layout11->addWidget( title1 );

  title2 = new QLabel( frame3, "title2" );
  title2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, 0, 0, title2->sizePolicy().hasHeightForWidth() ) );
  title2->setBackgroundOrigin( QLabel::ParentOrigin );
  layout11->addWidget( title2 );

  title3 = new QLabel( frame3, "title3" );
  title3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 0, 0, title3->sizePolicy().hasHeightForWidth() ) );
  title3->setBackgroundOrigin( QLabel::ParentOrigin );
  layout11->addWidget( title3 );

  title4 = new QLabel( frame3, "title4" );
  title4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 0, 0, title4->sizePolicy().hasHeightForWidth() ) );
  title4->setBackgroundOrigin( QLabel::ParentOrigin );
  layout11->addWidget( title4 );
  
  frame3Layout->addLayout( layout11, 0, 3 );
  
  MainWindowLayout->addWidget( frame3, 1, 0 );
  languageChange();
  resize( QSize(914, 573).expandedTo(minimumSizeHint()) );
  clearWState( WState_Polished );
  
  // connect( songList, SIGNAL( doubleClicked(QVectorViewItem*) ), this, SLOT( selectSong(QVectorViewItem*) ) );
  // connect( songList, SIGNAL( returnPressed(QVectorViewItem*) ), this, SLOT( selectionMenu() ) );
  // connect( songList, SIGNAL( rightButtonPressed(QListViewItem*,const QPoint&,int) ), this, SLOT( selectionMenu() ) );
  
  setTabOrder( songList, searchLine );
  setTabOrder( searchLine, tabs );
  setTabOrder( tabs, pushButton2 );
  setTabOrder( extraSongs, weighDuplicateAuthors );

  printf("policy horizontally = %d\n",albumList->header()->sizePolicy().horData());  
}

MainWindow::~MainWindow()
{
}

void MainWindow::languageChange()
{
  setCaption( tr( "kBpmDj File Selector" ) );
  songList->header()->setLabel( 0, tr( "Tempo" ) );
  songList->header()->setLabel( 1, tr( "Spectrum" ) );
  songList->header()->setLabel( 2, tr( "dColor" ) );
  songList->header()->setLabel( 3, tr( "Time" ) );
  songList->header()->setLabel( 4, tr( "Cues" ) );
  songList->header()->setLabel( 5, tr( "Title" ) );
  songList->header()->setLabel( 6, tr( "Author" ) );
  songList->header()->setLabel( 7, tr( "Version" ) );
  songList->header()->setLabel( 8, tr( "Tags" ) );
  songList->header()->setLabel( 9, tr( "OnDisk" ) );
  songList->header()->setLabel( 10, tr( "Index" ) );
  songList->header()->setLabel( 11, tr( "Md5" ) );
  songList->header()->setLabel( 12, tr( "Filename on disk" ) );
  tabs->changeTab( tab, tr( "Songs" ) );
  albumList->header()->setLabel( 0, tr( "Album" ) );
  albumList->header()->setLabel( 1, tr( "Title" ) );
  albumList->header()->setLabel( 2, tr( "Author" ) );
  albumList->header()->setLabel( 3, tr( "Time" ) );
  albumList->header()->setLabel( 4, tr( "Index" ) );
  albumList->header()->setLabel( 5, tr( "File" ) );
  tabs->changeTab( TabPage, tr( "Albums" ) );
  tagList->header()->setLabel( 0, tr( "Tag" ) );
  tagList->header()->setLabel( 1, tr( "Include" ) );
  tagList->header()->setLabel( 2, tr( "Must Be Present" ) );
  tagList->header()->setLabel( 3, tr( "Exclude" ) );
  tagList->clear();
  QListViewItem * item = new QListViewItem( tagList, 0 );
  item->setText( 1, tr( "Yes" ) );
  item->setText( 2, tr( "___" ) );
  item->setText( 3, tr( "___" ) );
  
  tabs->changeTab( Tags, tr( "Tags" ) );
  pushButton2->setText( tr( "Fullly automatic mix !!!" ) );
  weighDuplicateAuthors->setText( tr( "No duplicate authors" ) );
  tabs->changeTab( tab_2, tr( "Queue" ) );
}

int main(int argc, char* argv[])
{
  QApplication * app = new QApplication(argc,argv);
  MainWindow * wind = new MainWindow();
  app->setMainWidget(wind);
  wind->show();
  app->exec();
}
