/****************************************************************************
** Form implementation generated from reading ui file 'tempolineanalyzer.ui'
**
** Created: Thu Jul 24 05:00:31 2003
**      by: The User Interface Compiler ($Id: tempolineanalyzer.cpp,v 1.1 2003/10/10 08:50:55 krubbens Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "tempolineanalyzer.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qslider.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a TempoLineDialog as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
TempoLineDialog::TempoLineDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "TempoLineDialog" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setSizeGripEnabled( TRUE );
    TempoLineDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "TempoLineDialogLayout"); 
    QSpacerItem* spacer = new QSpacerItem( 110, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    TempoLineDialogLayout->addItem( spacer, 1, 1 );

    tempoline = new QLabel( this, "tempoline" );
    tempoline->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)2, 0, 0, tempoline->sizePolicy().hasHeightForWidth() ) );
    tempoline->setFrameShape( QLabel::Panel );
    tempoline->setScaledContents( TRUE );

    TempoLineDialogLayout->addMultiCellWidget( tempoline, 0, 0, 0, 3 );

    color = new QSlider( this, "color" );
    color->setMaxValue( 255 );
    color->setPageStep( 5 );
    color->setOrientation( QSlider::Horizontal );

    TempoLineDialogLayout->addMultiCellWidget( color, 2, 2, 1, 3 );

    textLabel1_2 = new QLabel( this, "textLabel1_2" );

    TempoLineDialogLayout->addWidget( textLabel1_2, 2, 0 );

    pushButton2 = new QPushButton( this, "pushButton2" );

    TempoLineDialogLayout->addWidget( pushButton2, 1, 3 );

    pushButton51 = new QPushButton( this, "pushButton51" );

    TempoLineDialogLayout->addWidget( pushButton51, 1, 2 );
    languageChange();
    resize( QSize(674, 437).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pushButton51, SIGNAL( clicked() ), this, SLOT( showTempoLine() ) );
    connect( pushButton2, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( color, SIGNAL( valueChanged(int) ), this, SLOT( balanceChanged() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
TempoLineDialog::~TempoLineDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void TempoLineDialog::languageChange()
{
    setCaption( tr( "Tempo Line Analyzer" ) );
    textLabel1_2->setText( tr( "Balance" ) );
    pushButton2->setText( tr( "Ok" ) );
    pushButton51->setText( tr( "Update" ) );
}

void TempoLineDialog::cancel()
{
    qWarning( "TempoLineDialog::cancel(): Not implemented yet" );
}

void TempoLineDialog::finish()
{
    qWarning( "TempoLineDialog::finish(): Not implemented yet" );
}

void TempoLineDialog::startAutomaticCounter()
{
    qWarning( "TempoLineDialog::startAutomaticCounter(): Not implemented yet" );
}

void TempoLineDialog::balanceChanged()
{
    qWarning( "TempoLineDialog::balanceChanged(): Not implemented yet" );
}

void TempoLineDialog::showTempoLine()
{
    qWarning( "TempoLineDialog::showTempoLine(): Not implemented yet" );
}

