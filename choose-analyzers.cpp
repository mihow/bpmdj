/****************************************************************************
** Form implementation generated from reading ui file 'choose-analyzers.ui'
**
** Created: Sun Aug 10 14:06:56 2003
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.2   edited Dec 19 11:45 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "choose-analyzers.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a ChooseAnalyzers as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ChooseAnalyzers::ChooseAnalyzers( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "ChooseAnalyzers" );
    setSizeGripEnabled( TRUE );
    ChooseAnalyzersLayout = new QGridLayout( this, 1, 1, 11, 6, "ChooseAnalyzersLayout"); 

    tabWidget2 = new QTabWidget( this, "tabWidget2" );

    tab = new QWidget( tabWidget2, "tab" );
    tabLayout = new QGridLayout( tab, 1, 1, 11, 6, "tabLayout"); 

    textLabel1 = new QLabel( tab, "textLabel1" );
    textLabel1->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );

    tabLayout->addWidget( textLabel1, 0, 0 );

    spectrumAnalyzer = new QCheckBox( tab, "spectrumAnalyzer" );

    tabLayout->addWidget( spectrumAnalyzer, 3, 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 30, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout->addItem( spacer, 1, 0 );

    tempoAnalyzer = new QCheckBox( tab, "tempoAnalyzer" );

    tabLayout->addWidget( tempoAnalyzer, 2, 0 );
    tabWidget2->insertTab( tab, QString("") );

    tab_2 = new QWidget( tabWidget2, "tab_2" );
    tabLayout_2 = new QGridLayout( tab_2, 1, 1, 11, 6, "tabLayout_2"); 

    TextLabel1 = new QLabel( tab_2, "TextLabel1" );
    TextLabel1->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    tabLayout_2->addWidget( TextLabel1, 2, 0 );

    TextLabel2 = new QLabel( tab_2, "TextLabel2" );
    TextLabel2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    tabLayout_2->addWidget( TextLabel2, 3, 0 );

    Line7 = new QFrame( tab_2, "Line7" );
    Line7->setFrameShape( QFrame::HLine );
    Line7->setFrameShadow( QFrame::Sunken );
    Line7->setFrameShape( QFrame::HLine );

    tabLayout_2->addMultiCellWidget( Line7, 1, 1, 0, 1 );

    From = new QLineEdit( tab_2, "From" );

    tabLayout_2->addWidget( From, 2, 1 );

    To = new QLineEdit( tab_2, "To" );

    tabLayout_2->addWidget( To, 3, 1 );

    TextLabel4 = new QLabel( tab_2, "TextLabel4" );
    TextLabel4->setAlignment( int( QLabel::WordBreak | QLabel::AlignTop | QLabel::AlignLeft ) );

    tabLayout_2->addMultiCellWidget( TextLabel4, 0, 0, 0, 1 );
    tabWidget2->insertTab( tab_2, QString("") );

    ChooseAnalyzersLayout->addMultiCellWidget( tabWidget2, 0, 0, 0, 2 );

    pushButton3 = new QPushButton( this, "pushButton3" );

    ChooseAnalyzersLayout->addWidget( pushButton3, 1, 2 );

    pushButton4 = new QPushButton( this, "pushButton4" );

    ChooseAnalyzersLayout->addWidget( pushButton4, 1, 1 );
    QSpacerItem* spacer_2 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    ChooseAnalyzersLayout->addItem( spacer_2, 1, 0 );
    languageChange();
    resize( QSize(604, 349).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pushButton3, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( pushButton4, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
ChooseAnalyzers::~ChooseAnalyzers()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ChooseAnalyzers::languageChange()
{
    setCaption( tr( "Batch Analyzing" ) );
    textLabel1->setText( tr( "This process starts a number of analyzers on a song. Below you can choose which analyzers you want to apply. Once you press the 'Start Batch' button a backgroundprocess will be started. \n"
"\n"
"Warning: If the process is killed when you close kbpm-dj. Start analyze.sh manually (certain bashes kills childs without parent)." ) );
    spectrumAnalyzer->setText( tr( "Spectrum Analyzer" ) );
    tempoAnalyzer->setText( tr( "Tempo Analyzer" ) );
    tabWidget2->changeTab( tab, tr( "Analyzers" ) );
    TextLabel1->setText( tr( "From :" ) );
    TextLabel2->setText( tr( "To :" ) );
    TextLabel4->setText( tr( "Enter the bounds of the BPM search algorithm. If no bounds are specified the bounds currently specified in the .idx will be used.\n"
"\n"
"Determining the BPM of a song takes a while, therefore a background process will be spawned. When that process has finished you should restart kbpm-dj, otherwise the selector will still work with old (wrong) tempos. This also means that you can close kbpm-dj any time you want." ) );
    tabWidget2->changeTab( tab_2, tr( "Tempo" ) );
    pushButton3->setText( tr( "Spawn Batch Process" ) );
    pushButton4->setText( tr( "Cancel" ) );
}

