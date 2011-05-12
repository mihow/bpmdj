/****************************************************************************
** Form implementation generated from reading ui file 'playercommandwizard.ui'
**
** Created: Fri Oct 10 19:42:54 2003
**      by: The User Interface Compiler ($Id: playercommandwizard.cpp,v 1.2 2003/10/10 18:56:00 krubbens Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "playercommandwizard.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a PlayerCommandWizard as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
PlayerCommandWizard::PlayerCommandWizard( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "PlayerCommandWizard" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)3, 0, 0, sizePolicy().hasHeightForWidth() ) );
    PlayerCommandWizardLayout = new QGridLayout( this, 1, 1, 11, 6, "PlayerCommandWizardLayout"); 

    buttonGroup1 = new QButtonGroup( this, "buttonGroup1" );
    buttonGroup1->setExclusive( TRUE );
    buttonGroup1->setColumnLayout(0, Qt::Vertical );
    buttonGroup1->layout()->setSpacing( 6 );
    buttonGroup1->layout()->setMargin( 11 );
    buttonGroup1Layout = new QGridLayout( buttonGroup1->layout() );
    buttonGroup1Layout->setAlignment( Qt::AlignTop );

    oss = new QCheckBox( buttonGroup1, "oss" );
    oss->setChecked( TRUE );

    buttonGroup1Layout->addWidget( oss, 0, 0 );

    alsa = new QCheckBox( buttonGroup1, "alsa" );

    buttonGroup1Layout->addWidget( alsa, 1, 0 );

    PlayerCommandWizardLayout->addWidget( buttonGroup1, 0, 0 );

    buttonGroup2 = new QButtonGroup( this, "buttonGroup2" );
    buttonGroup2->setColumnLayout(0, Qt::Vertical );
    buttonGroup2->layout()->setSpacing( 6 );
    buttonGroup2->layout()->setMargin( 11 );
    buttonGroup2Layout = new QGridLayout( buttonGroup2->layout() );
    buttonGroup2Layout->setAlignment( Qt::AlignTop );

    textLabel4 = new QLabel( buttonGroup2, "textLabel4" );

    buttonGroup2Layout->addMultiCellWidget( textLabel4, 1, 1, 0, 1 );

    verbose = new QCheckBox( buttonGroup2, "verbose" );
    verbose->setChecked( TRUE );

    buttonGroup2Layout->addWidget( verbose, 2, 0 );

    textLabel3 = new QLabel( buttonGroup2, "textLabel3" );

    buttonGroup2Layout->addWidget( textLabel3, 0, 0 );

    latency = new QLineEdit( buttonGroup2, "latency" );

    buttonGroup2Layout->addMultiCellWidget( latency, 0, 0, 1, 2 );

    comboBox5 = new QComboBox( FALSE, buttonGroup2, "comboBox5" );

    buttonGroup2Layout->addWidget( comboBox5, 0, 3 );

    comboBox7 = new QComboBox( FALSE, buttonGroup2, "comboBox7" );

    buttonGroup2Layout->addWidget( comboBox7, 1, 3 );

    layout1 = new QHBoxLayout( 0, 0, 6, "layout1"); 

    x = new QLineEdit( buttonGroup2, "x" );
    x->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, x->sizePolicy().hasHeightForWidth() ) );
    x->setMinimumSize( QSize( 64, 0 ) );
    layout1->addWidget( x );

    textLabel5 = new QLabel( buttonGroup2, "textLabel5" );
    layout1->addWidget( textLabel5 );

    y = new QLineEdit( buttonGroup2, "y" );
    y->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, y->sizePolicy().hasHeightForWidth() ) );
    y->setMinimumSize( QSize( 64, 0 ) );
    layout1->addWidget( y );

    buttonGroup2Layout->addLayout( layout1, 1, 2 );

    PlayerCommandWizardLayout->addMultiCellWidget( buttonGroup2, 0, 0, 1, 3 );

    buttonGroup4 = new QButtonGroup( this, "buttonGroup4" );
    buttonGroup4->setColumnLayout(0, Qt::Vertical );
    buttonGroup4->layout()->setSpacing( 6 );
    buttonGroup4->layout()->setMargin( 11 );
    buttonGroup4Layout = new QGridLayout( buttonGroup4->layout() );
    buttonGroup4Layout->setAlignment( Qt::AlignTop );

    textLabel9 = new QLabel( buttonGroup4, "textLabel9" );

    buttonGroup4Layout->addWidget( textLabel9, 0, 0 );

    dev = new QLineEdit( buttonGroup4, "dev" );

    buttonGroup4Layout->addWidget( dev, 0, 1 );

    comboBox2 = new QComboBox( FALSE, buttonGroup4, "comboBox2" );

    buttonGroup4Layout->addWidget( comboBox2, 0, 2 );

    PlayerCommandWizardLayout->addMultiCellWidget( buttonGroup4, 2, 2, 0, 3 );

    buttonGroup3 = new QButtonGroup( this, "buttonGroup3" );
    buttonGroup3->setColumnLayout(0, Qt::Vertical );
    buttonGroup3->layout()->setSpacing( 6 );
    buttonGroup3->layout()->setMargin( 11 );
    buttonGroup3Layout = new QGridLayout( buttonGroup3->layout() );
    buttonGroup3Layout->setAlignment( Qt::AlignTop );

    textLabel6 = new QLabel( buttonGroup3, "textLabel6" );

    buttonGroup3Layout->addWidget( textLabel6, 0, 0 );

    dsp = new QLineEdit( buttonGroup3, "dsp" );

    buttonGroup3Layout->addWidget( dsp, 0, 1 );

    fragments = new QLineEdit( buttonGroup3, "fragments" );

    buttonGroup3Layout->addWidget( fragments, 1, 1 );

    textLabel7 = new QLabel( buttonGroup3, "textLabel7" );

    buttonGroup3Layout->addWidget( textLabel7, 1, 0 );

    nolatency = new QCheckBox( buttonGroup3, "nolatency" );

    buttonGroup3Layout->addMultiCellWidget( nolatency, 2, 2, 0, 1 );

    comboBox3 = new QComboBox( FALSE, buttonGroup3, "comboBox3" );

    buttonGroup3Layout->addWidget( comboBox3, 0, 2 );

    comboBox4 = new QComboBox( FALSE, buttonGroup3, "comboBox4" );

    buttonGroup3Layout->addWidget( comboBox4, 1, 2 );

    PlayerCommandWizardLayout->addMultiCellWidget( buttonGroup3, 1, 1, 0, 3 );
    QSpacerItem* spacer = new QSpacerItem( 460, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    PlayerCommandWizardLayout->addMultiCell( spacer, 3, 3, 0, 1 );

    pushButton7 = new QPushButton( this, "pushButton7" );

    PlayerCommandWizardLayout->addWidget( pushButton7, 3, 2 );

    pushButton8 = new QPushButton( this, "pushButton8" );

    PlayerCommandWizardLayout->addWidget( pushButton8, 3, 3 );
    languageChange();
    resize( QSize(558, 378).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( comboBox2, SIGNAL( activated(const QString&) ), dev, SLOT( setText(const QString&) ) );
    connect( comboBox3, SIGNAL( activated(const QString&) ), dsp, SLOT( setText(const QString&) ) );
    connect( comboBox4, SIGNAL( activated(const QString&) ), fragments, SLOT( setText(const QString&) ) );
    connect( comboBox5, SIGNAL( activated(const QString&) ), latency, SLOT( setText(const QString&) ) );
    connect( pushButton7, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( pushButton8, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( comboBox7, SIGNAL( activated(const QString&) ), y, SLOT( setText(const QString&) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
PlayerCommandWizard::~PlayerCommandWizard()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PlayerCommandWizard::languageChange()
{
    setCaption( tr( "Player Command" ) );
    buttonGroup1->setTitle( tr( "Device Driver" ) );
    oss->setText( tr( "OSS" ) );
    alsa->setText( tr( "ALSA" ) );
    buttonGroup2->setTitle( tr( "Common" ) );
    textLabel4->setText( tr( "Screen Position" ) );
    verbose->setText( tr( "verbose" ) );
    textLabel3->setText( tr( "Latency (ms)" ) );
    latency->setText( tr( "150" ) );
    comboBox5->clear();
    comboBox5->insertItem( tr( "50" ) );
    comboBox5->insertItem( tr( "100" ) );
    comboBox5->insertItem( tr( "150" ) );
    comboBox5->insertItem( tr( "200" ) );
    comboBox5->insertItem( tr( "250" ) );
    comboBox5->insertItem( tr( "300" ) );
    comboBox7->clear();
    comboBox7->insertItem( tr( "0" ) );
    comboBox7->insertItem( tr( "400" ) );
    comboBox7->insertItem( tr( "800" ) );
    x->setText( tr( "0" ) );
    textLabel5->setText( tr( "," ) );
    y->setText( tr( "0" ) );
    buttonGroup4->setTitle( tr( "ALSA options" ) );
    textLabel9->setText( tr( "ALSA device to use" ) );
    dev->setText( tr( "hw:0" ) );
    comboBox2->clear();
    comboBox2->insertItem( tr( "hw:0,0" ) );
    comboBox2->insertItem( tr( "hw:1,0" ) );
    comboBox2->insertItem( tr( "plughw:0" ) );
    comboBox2->insertItem( tr( "plughw:1" ) );
    comboBox2->insertItem( tr( "default" ) );
    buttonGroup3->setTitle( tr( "OSS options" ) );
    textLabel6->setText( tr( "DSP device to use" ) );
    dsp->setText( tr( "/dev/dsp" ) );
    fragments->setText( tr( "16" ) );
    textLabel7->setText( tr( "Fragment count" ) );
    nolatency->setText( tr( "no latency accounting" ) );
    comboBox3->clear();
    comboBox3->insertItem( tr( "/dev/dsp" ) );
    comboBox3->insertItem( tr( "/dev/dsp1" ) );
    comboBox3->insertItem( tr( "/dev/dsp2" ) );
    comboBox4->clear();
    comboBox4->insertItem( tr( "16" ) );
    comboBox4->insertItem( tr( "8" ) );
    comboBox4->insertItem( tr( "4" ) );
    comboBox4->insertItem( tr( "2" ) );
    pushButton7->setText( tr( "Ok" ) );
    pushButton8->setText( tr( "Cancel" ) );
}

