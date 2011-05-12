/****************************************************************************
** Form implementation generated from reading ui file 'impulseanalyzer.ui'
**
** Created: Fri Jul 18 01:20:08 2003
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.1.2   edited Dec 19 11:45 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "impulseanalyzer.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qslider.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a ImpulseDialog as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ImpulseDialog::ImpulseDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "ImpulseDialog" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setSizeGripEnabled( TRUE );
    ImpulseDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "ImpulseDialogLayout"); 
    QSpacerItem* spacer = new QSpacerItem( 110, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    ImpulseDialogLayout->addItem( spacer, 1, 1 );

    impulse = new QLabel( this, "impulse" );
    impulse->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)2, 0, 0, impulse->sizePolicy().hasHeightForWidth() ) );
    impulse->setFrameShape( QLabel::Panel );
    impulse->setScaledContents( TRUE );

    ImpulseDialogLayout->addMultiCellWidget( impulse, 0, 0, 0, 3 );

    color = new QSlider( this, "color" );
    color->setMaxValue( 255 );
    color->setPageStep( 5 );
    color->setOrientation( QSlider::Horizontal );

    ImpulseDialogLayout->addMultiCellWidget( color, 2, 2, 1, 3 );

    textLabel1_2 = new QLabel( this, "textLabel1_2" );

    ImpulseDialogLayout->addWidget( textLabel1_2, 2, 0 );

    pushButton2 = new QPushButton( this, "pushButton2" );

    ImpulseDialogLayout->addWidget( pushButton2, 1, 3 );

    pushButton51 = new QPushButton( this, "pushButton51" );

    ImpulseDialogLayout->addWidget( pushButton51, 1, 2 );
    languageChange();
    resize( QSize(674, 437).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pushButton51, SIGNAL( clicked() ), this, SLOT( showPattern() ) );
    connect( pushButton2, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( color, SIGNAL( valueChanged(int) ), this, SLOT( balanceChanged() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
ImpulseDialog::~ImpulseDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ImpulseDialog::languageChange()
{
    setCaption( tr( "Impulse Analyzer" ) );
    textLabel1_2->setText( tr( "Balance" ) );
    pushButton2->setText( tr( "Ok" ) );
    pushButton51->setText( tr( "Update" ) );
}

void ImpulseDialog::slantChanged()
{
    qWarning( "ImpulseDialog::slantChanged(): Not implemented yet" );
}

void ImpulseDialog::showPattern()
{
    qWarning( "ImpulseDialog::showPattern(): Not implemented yet" );
}

void ImpulseDialog::cancel()
{
    qWarning( "ImpulseDialog::cancel(): Not implemented yet" );
}

void ImpulseDialog::finish()
{
    qWarning( "ImpulseDialog::finish(): Not implemented yet" );
}

void ImpulseDialog::startAutomaticCounter()
{
    qWarning( "ImpulseDialog::startAutomaticCounter(): Not implemented yet" );
}

void ImpulseDialog::tap()
{
    qWarning( "ImpulseDialog::tap(): Not implemented yet" );
}

void ImpulseDialog::fixPhases()
{
    qWarning( "ImpulseDialog::fixPhases(): Not implemented yet" );
}

void ImpulseDialog::setTempo()
{
    qWarning( "ImpulseDialog::setTempo(): Not implemented yet" );
}

void ImpulseDialog::balanceChanged()
{
    qWarning( "ImpulseDialog::balanceChanged(): Not implemented yet" );
}

