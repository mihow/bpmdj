/****************************************************************************
** Form implementation generated from reading ui file 'choose-analyzers.ui'
**
** Created: Sat May 8 12:31:21 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.3   edited May 19 14:22 $)
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
#include <qbuttongroup.h>
#include <qradiobutton.h>
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

    pushButton3 = new QPushButton( this, "pushButton3" );

    ChooseAnalyzersLayout->addWidget( pushButton3, 1, 2 );

    pushButton4 = new QPushButton( this, "pushButton4" );

    ChooseAnalyzersLayout->addWidget( pushButton4, 1, 1 );
    QSpacerItem* spacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    ChooseAnalyzersLayout->addItem( spacer, 1, 0 );

    tabWidget2 = new QTabWidget( this, "tabWidget2" );
    tabWidget2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)3, 0, 0, tabWidget2->sizePolicy().hasHeightForWidth() ) );

    tab = new QWidget( tabWidget2, "tab" );
    tabLayout = new QGridLayout( tab, 1, 1, 11, 6, "tabLayout"); 

    textLabel1 = new QLabel( tab, "textLabel1" );
    textLabel1->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );

    tabLayout->addWidget( textLabel1, 0, 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 90, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout->addItem( spacer_2, 4, 0 );

    tempoAnalyzer = new QCheckBox( tab, "tempoAnalyzer" );

    tabLayout->addWidget( tempoAnalyzer, 2, 0 );

    spectrumAnalyzer = new QCheckBox( tab, "spectrumAnalyzer" );

    tabLayout->addWidget( spectrumAnalyzer, 3, 0 );

    line2 = new QFrame( tab, "line2" );
    line2->setFrameShape( QFrame::HLine );
    line2->setFrameShadow( QFrame::Sunken );
    line2->setFrameShape( QFrame::HLine );

    tabLayout->addWidget( line2, 1, 0 );
    tabWidget2->insertTab( tab, QString("") );

    tab_2 = new QWidget( tabWidget2, "tab_2" );
    tabLayout_2 = new QGridLayout( tab_2, 1, 1, 11, 6, "tabLayout_2"); 

    TextLabel4 = new QLabel( tab_2, "TextLabel4" );
    TextLabel4->setAlignment( int( QLabel::WordBreak | QLabel::AlignTop | QLabel::AlignLeft ) );

    tabLayout_2->addMultiCellWidget( TextLabel4, 0, 0, 0, 1 );

    Line7 = new QFrame( tab_2, "Line7" );
    Line7->setFrameShape( QFrame::HLine );
    Line7->setFrameShadow( QFrame::Sunken );
    Line7->setFrameShape( QFrame::HLine );

    tabLayout_2->addMultiCellWidget( Line7, 1, 2, 0, 1 );

    enveloppeFft = new QButtonGroup( tab_2, "enveloppeFft" );
    enveloppeFft->setRadioButtonExclusive( TRUE );
    enveloppeFft->setColumnLayout(0, Qt::Vertical );
    enveloppeFft->layout()->setSpacing( 6 );
    enveloppeFft->layout()->setMargin( 11 );
    enveloppeFftLayout = new QGridLayout( enveloppeFft->layout() );
    enveloppeFftLayout->setAlignment( Qt::AlignTop );

    resamplingScan = new QRadioButton( enveloppeFft, "resamplingScan" );
    resamplingScan->setChecked( TRUE );

    enveloppeFftLayout->addWidget( resamplingScan, 0, 0 );

    ultraLongFFT = new QRadioButton( enveloppeFft, "ultraLongFFT" );
    ultraLongFFT->setChecked( FALSE );

    enveloppeFftLayout->addWidget( ultraLongFFT, 1, 0 );

    enveloppeSpectrum = new QRadioButton( enveloppeFft, "enveloppeSpectrum" );

    enveloppeFftLayout->addWidget( enveloppeSpectrum, 2, 0 );

    fullAutoCorrelation = new QRadioButton( enveloppeFft, "fullAutoCorrelation" );

    enveloppeFftLayout->addWidget( fullAutoCorrelation, 3, 0 );

    tabLayout_2->addMultiCellWidget( enveloppeFft, 2, 4, 1, 1 );

    layout13 = new QHBoxLayout( 0, 0, 6, "layout13"); 

    layout11 = new QVBoxLayout( 0, 0, 6, "layout11"); 

    TextLabel1 = new QLabel( tab_2, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)5, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );
    TextLabel1->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    layout11->addWidget( TextLabel1 );

    TextLabel2 = new QLabel( tab_2, "TextLabel2" );
    TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)5, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );
    TextLabel2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    layout11->addWidget( TextLabel2 );
    layout13->addLayout( layout11 );

    layout12 = new QVBoxLayout( 0, 0, 6, "layout12"); 

    From = new QLineEdit( tab_2, "From" );
    From->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, From->sizePolicy().hasHeightForWidth() ) );
    layout12->addWidget( From );

    To = new QLineEdit( tab_2, "To" );
    To->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, To->sizePolicy().hasHeightForWidth() ) );
    layout12->addWidget( To );
    layout13->addLayout( layout12 );

    tabLayout_2->addLayout( layout13, 3, 0 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout_2->addItem( spacer_3, 4, 0 );
    tabWidget2->insertTab( tab_2, QString("") );

    ChooseAnalyzersLayout->addMultiCellWidget( tabWidget2, 0, 0, 0, 2 );
    languageChange();
    resize( QSize(765, 473).expandedTo(minimumSizeHint()) );
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
    pushButton3->setText( tr( "Spawn Batch Process" ) );
    pushButton4->setText( tr( "Cancel" ) );
    textLabel1->setText( tr( "This process starts a number of analyzers on a song. Below you can choose which analyzers you want to apply. Once you press the 'Start Batch' button a backgroundprocess will be started. \n"
"\n"
"Warning: If the process is killed when you close kbpm-dj. Start analyze.sh manually (certain bashes kills childs without parent)." ) );
    tempoAnalyzer->setText( tr( "Tempo Analyzer" ) );
    spectrumAnalyzer->setText( tr( "Spectrum Analyzer" ) );
    tabWidget2->changeTab( tab, tr( "Analyzers" ) );
    TextLabel4->setText( tr( "Enter the bounds of the BPM search algorithm. If no bounds are specified the bounds currently specified in the .idx will be used.\n"
"\n"
"Determining the BPM of a song takes a while, therefore a background process will be spawned. When that process has finished you should restart kbpm-dj, otherwise the selector will still work with old (wrong) tempos. This also means that you can close kbpm-dj any time you want." ) );
    enveloppeFft->setTitle( tr( "Algorithm" ) );
    resamplingScan->setText( tr( "Rayshoot, Resampling" ) );
    ultraLongFFT->setText( tr( "Rayshoot, FFT Guidance" ) );
    enveloppeSpectrum->setText( tr( "Enveloppe Spectrum" ) );
    fullAutoCorrelation->setText( tr( "Full Autocorrelation" ) );
    TextLabel1->setText( tr( "From" ) );
    TextLabel2->setText( tr( "To" ) );
    tabWidget2->changeTab( tab_2, tr( "Tempo" ) );
}

