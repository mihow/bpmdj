/****************************************************************************
** Form implementation generated from reading ui file 'spectrumanalyzer.ui'
**
** Created: Wed Jul 9 19:18:38 2003
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.2   edited Dec 19 11:45 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "spectrumanalyzer.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qslider.h>
#include <qprogressbar.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a SpectrumDialog as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SpectrumDialog::SpectrumDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "SpectrumDialog" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setSizeGripEnabled( TRUE );
    SpectrumDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "SpectrumDialogLayout"); 

    pushButton8 = new QPushButton( this, "pushButton8" );

    SpectrumDialogLayout->addWidget( pushButton8, 0, 0 );

    groupBox1 = new QGroupBox( this, "groupBox1" );
    groupBox1->setColumnLayout(0, Qt::Vertical );
    groupBox1->layout()->setSpacing( 6 );
    groupBox1->layout()->setMargin( 11 );
    groupBox1Layout = new QGridLayout( groupBox1->layout() );
    groupBox1Layout->setAlignment( Qt::AlignTop );

    bark12 = new QSlider( groupBox1, "bark12" );
    bark12->setEnabled( FALSE );
    bark12->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark12, 0, 12 );

    bark11 = new QSlider( groupBox1, "bark11" );
    bark11->setEnabled( FALSE );
    bark11->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark11, 0, 11 );

    bark10 = new QSlider( groupBox1, "bark10" );
    bark10->setEnabled( FALSE );
    bark10->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark10, 0, 10 );

    bark9 = new QSlider( groupBox1, "bark9" );
    bark9->setEnabled( FALSE );
    bark9->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark9, 0, 9 );

    bark8 = new QSlider( groupBox1, "bark8" );
    bark8->setEnabled( FALSE );
    bark8->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark8, 0, 8 );

    bark7 = new QSlider( groupBox1, "bark7" );
    bark7->setEnabled( FALSE );
    bark7->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark7, 0, 7 );

    bark6 = new QSlider( groupBox1, "bark6" );
    bark6->setEnabled( FALSE );
    bark6->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark6, 0, 6 );

    bark5 = new QSlider( groupBox1, "bark5" );
    bark5->setEnabled( FALSE );
    bark5->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark5, 0, 5 );

    bark4 = new QSlider( groupBox1, "bark4" );
    bark4->setEnabled( FALSE );
    bark4->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark4, 0, 4 );

    bark3 = new QSlider( groupBox1, "bark3" );
    bark3->setEnabled( FALSE );
    bark3->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark3, 0, 3 );

    bark0 = new QSlider( groupBox1, "bark0" );
    bark0->setEnabled( FALSE );
    bark0->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark0, 0, 0 );

    bark1 = new QSlider( groupBox1, "bark1" );
    bark1->setEnabled( FALSE );
    bark1->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark1, 0, 1 );

    bark2 = new QSlider( groupBox1, "bark2" );
    bark2->setEnabled( FALSE );
    bark2->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark2, 0, 2 );

    bark17 = new QSlider( groupBox1, "bark17" );
    bark17->setEnabled( FALSE );
    bark17->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark17, 0, 17 );

    bark20 = new QSlider( groupBox1, "bark20" );
    bark20->setEnabled( FALSE );
    bark20->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark20, 0, 20 );

    bark22 = new QSlider( groupBox1, "bark22" );
    bark22->setEnabled( FALSE );
    bark22->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark22, 0, 22 );

    bark18 = new QSlider( groupBox1, "bark18" );
    bark18->setEnabled( FALSE );
    bark18->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark18, 0, 18 );

    bark23 = new QSlider( groupBox1, "bark23" );
    bark23->setEnabled( FALSE );
    bark23->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark23, 0, 23 );

    bark13 = new QSlider( groupBox1, "bark13" );
    bark13->setEnabled( FALSE );
    bark13->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark13, 0, 13 );

    bark19 = new QSlider( groupBox1, "bark19" );
    bark19->setEnabled( FALSE );
    bark19->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark19, 0, 19 );

    bark14 = new QSlider( groupBox1, "bark14" );
    bark14->setEnabled( FALSE );
    bark14->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark14, 0, 14 );

    bark21 = new QSlider( groupBox1, "bark21" );
    bark21->setEnabled( FALSE );
    bark21->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark21, 0, 21 );

    bark16 = new QSlider( groupBox1, "bark16" );
    bark16->setEnabled( FALSE );
    bark16->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark16, 0, 16 );

    bark15 = new QSlider( groupBox1, "bark15" );
    bark15->setEnabled( FALSE );
    bark15->setOrientation( QSlider::Vertical );

    groupBox1Layout->addWidget( bark15, 0, 15 );

    SpectrumDialogLayout->addMultiCellWidget( groupBox1, 1, 1, 0, 1 );

    Progress = new QProgressBar( this, "Progress" );
    Progress->setProgress( 0 );

    SpectrumDialogLayout->addWidget( Progress, 0, 1 );
    languageChange();
    resize( QSize(566, 329).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pushButton8, SIGNAL( clicked() ), this, SLOT( fetchSpectrum() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
SpectrumDialog::~SpectrumDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SpectrumDialog::languageChange()
{
    setCaption( tr( "Spectrum analyzer" ) );
    pushButton8->setText( tr( "Fetch Spectrum" ) );
    groupBox1->setTitle( tr( "Bark Frequencies" ) );
}

void SpectrumDialog::about()
{
    qWarning( "SpectrumDialog::about(): Not implemented yet" );
}

void SpectrumDialog::audioRangeChanged()
{
    qWarning( "SpectrumDialog::audioRangeChanged(): Not implemented yet" );
}

void SpectrumDialog::bpmRangeChanged()
{
    qWarning( "SpectrumDialog::bpmRangeChanged(): Not implemented yet" );
}

void SpectrumDialog::cancel()
{
    qWarning( "SpectrumDialog::cancel(): Not implemented yet" );
}

void SpectrumDialog::incBpm()
{
    qWarning( "SpectrumDialog::incBpm(): Not implemented yet" );
}

void SpectrumDialog::decBpm()
{
    qWarning( "SpectrumDialog::decBpm(): Not implemented yet" );
}

void SpectrumDialog::play()
{
    qWarning( "SpectrumDialog::play(): Not implemented yet" );
}

void SpectrumDialog::reset()
{
    qWarning( "SpectrumDialog::reset(): Not implemented yet" );
}

void SpectrumDialog::startAutomaticCounter()
{
    qWarning( "SpectrumDialog::startAutomaticCounter(): Not implemented yet" );
}

void SpectrumDialog::tap()
{
    qWarning( "SpectrumDialog::tap(): Not implemented yet" );
}

void SpectrumDialog::fetchSpectrum()
{
    qWarning( "SpectrumDialog::fetchSpectrum(): Not implemented yet" );
}

