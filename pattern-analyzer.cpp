/****************************************************************************
** Form implementation generated from reading ui file 'pattern-analyzer.ui'
**
** Created: Sun Sep 14 13:38:46 2003
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.2   edited Dec 19 11:45 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "pattern-analyzer.h"

#include <qvariant.h>
#include <qslider.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a PatternDialog as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
PatternDialog::PatternDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "PatternDialog" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setSizeGripEnabled( TRUE );
    PatternDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "PatternDialogLayout"); 

    periodDelta = new QSlider( this, "periodDelta" );
    periodDelta->setMinValue( -100 );
    periodDelta->setMaxValue( 100 );
    periodDelta->setOrientation( QSlider::Vertical );
    periodDelta->setTickmarks( QSlider::Left );
    periodDelta->setTickInterval( 100 );

    PatternDialogLayout->addWidget( periodDelta, 1, 8 );

    periodDelta10 = new QSlider( this, "periodDelta10" );
    periodDelta10->setMinValue( -1000 );
    periodDelta10->setMaxValue( 1000 );
    periodDelta10->setLineStep( 10 );
    periodDelta10->setPageStep( 100 );
    periodDelta10->setOrientation( QSlider::Vertical );
    periodDelta10->setTickmarks( QSlider::Left );
    periodDelta10->setTickInterval( 100 );

    PatternDialogLayout->addWidget( periodDelta10, 1, 9 );

    textLabel1 = new QLabel( this, "textLabel1" );

    PatternDialogLayout->addMultiCellWidget( textLabel1, 2, 2, 0, 1 );

    pattern = new QLabel( this, "pattern" );
    pattern->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)2, 0, 0, pattern->sizePolicy().hasHeightForWidth() ) );
    pattern->setFrameShape( QLabel::Panel );
    pattern->setScaledContents( TRUE );

    PatternDialogLayout->addMultiCellWidget( pattern, 1, 1, 1, 7 );

    color = new QSlider( this, "color" );
    color->setMaxValue( 255 );
    color->setPageStep( 5 );
    color->setOrientation( QSlider::Horizontal );

    PatternDialogLayout->addMultiCellWidget( color, 3, 3, 2, 9 );

    textLabel1_2 = new QLabel( this, "textLabel1_2" );

    PatternDialogLayout->addMultiCellWidget( textLabel1_2, 3, 3, 0, 1 );

    pushButton4 = new QPushButton( this, "pushButton4" );

    PatternDialogLayout->addWidget( pushButton4, 2, 6 );

    pushButton2 = new QPushButton( this, "pushButton2" );

    PatternDialogLayout->addMultiCellWidget( pushButton2, 2, 2, 7, 9 );

    projection = new QLabel( this, "projection" );
    projection->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)2, 0, 0, projection->sizePolicy().hasHeightForWidth() ) );
    projection->setMaximumSize( QSize( 20, 32767 ) );
    projection->setScaledContents( TRUE );

    PatternDialogLayout->addWidget( projection, 1, 0 );

    updateButton = new QPushButton( this, "updateButton" );

    PatternDialogLayout->addWidget( updateButton, 2, 5 );

    beats = new QSpinBox( this, "beats" );
    beats->setMaxValue( 32 );
    beats->setMinValue( 1 );
    beats->setValue( 4 );

    PatternDialogLayout->addWidget( beats, 2, 2 );

    textLabel1_3 = new QLabel( this, "textLabel1_3" );
    textLabel1_3->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );

    PatternDialogLayout->addMultiCellWidget( textLabel1_3, 0, 0, 0, 9 );
    QSpacerItem* spacer = new QSpacerItem( 100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    PatternDialogLayout->addItem( spacer, 2, 3 );
    languageChange();
    resize( QSize(674, 437).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( updateButton, SIGNAL( clicked() ), this, SLOT( showPattern() ) );
    connect( pushButton2, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( periodDelta, SIGNAL( valueChanged(int) ), this, SLOT( slantChanged() ) );
    connect( pushButton4, SIGNAL( clicked() ), this, SLOT( setTempo() ) );
    connect( periodDelta10, SIGNAL( valueChanged(int) ), this, SLOT( slantChanged() ) );
    connect( color, SIGNAL( valueChanged(int) ), this, SLOT( balanceChanged() ) );
    connect( beats, SIGNAL( valueChanged(int) ), this, SLOT( slantChanged() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
PatternDialog::~PatternDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PatternDialog::languageChange()
{
    setCaption( tr( "Pattern Analyzer" ) );
    textLabel1->setText( tr( "Beats" ) );
    textLabel1_2->setText( tr( "Balance" ) );
    pushButton4->setText( tr( "Set Tempo" ) );
    pushButton2->setText( tr( "Ok" ) );
    updateButton->setText( tr( "Update" ) );
    textLabel1_3->setText( tr( "Horinzontally, this graph shows the measures of the song. Vertically, the content of one such measure is visualized. If the tempo of the song is correct, the song should contain distinctive visual horizontal lines. If the tempo is slightly incorrect these lines should be slanted. By using the two sliders at the right, the tempo of the song can be fine tuned. When this is done select 'set Tempo'. " ) );
}

void PatternDialog::slantChanged()
{
    qWarning( "PatternDialog::slantChanged(): Not implemented yet" );
}

void PatternDialog::showPattern()
{
    qWarning( "PatternDialog::showPattern(): Not implemented yet" );
}

void PatternDialog::cancel()
{
    qWarning( "PatternDialog::cancel(): Not implemented yet" );
}

void PatternDialog::finish()
{
    qWarning( "PatternDialog::finish(): Not implemented yet" );
}

void PatternDialog::startAutomaticCounter()
{
    qWarning( "PatternDialog::startAutomaticCounter(): Not implemented yet" );
}

void PatternDialog::tap()
{
    qWarning( "PatternDialog::tap(): Not implemented yet" );
}

void PatternDialog::fixPhases()
{
    qWarning( "PatternDialog::fixPhases(): Not implemented yet" );
}

void PatternDialog::setTempo()
{
    qWarning( "PatternDialog::setTempo(): Not implemented yet" );
}

void PatternDialog::balanceChanged()
{
    qWarning( "PatternDialog::balanceChanged(): Not implemented yet" );
}

void PatternDialog::dumpPatternToIdx()
{
    qWarning( "PatternDialog::dumpPatternToIdx(): Not implemented yet" );
}

