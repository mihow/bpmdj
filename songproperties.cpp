/****************************************************************************
** Form implementation generated from reading ui file 'songproperties.ui'
**
** Created: Mon Oct 20 11:54:15 2003
**      by: The User Interface Compiler ($Id: songproperties.cpp,v 1.1 2003/11/14 07:04:09 krubbens Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "songproperties.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qframe.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a AnalyzeDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
AnalyzeDialog::AnalyzeDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "AnalyzeDialog" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setSizeGripEnabled( TRUE );
    AnalyzeDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "AnalyzeDialogLayout"); 

    BusyToggle = new QPushButton( this, "BusyToggle" );
    BusyToggle->setEnabled( FALSE );
    BusyToggle->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, BusyToggle->sizePolicy().hasHeightForWidth() ) );
    BusyToggle->setFocusPolicy( QPushButton::NoFocus );
    BusyToggle->setToggleButton( TRUE );
    BusyToggle->setOn( FALSE );
    BusyToggle->setFlat( TRUE );

    AnalyzeDialogLayout->addWidget( BusyToggle, 0, 0 );

    Layout21 = new QHBoxLayout( 0, 0, 6, "Layout21"); 

    Layout20 = new QVBoxLayout( 0, 0, 6, "Layout20"); 

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    Layout20->addWidget( TextLabel1 );

    TextLabel5_2 = new QLabel( this, "TextLabel5_2" );
    TextLabel5_2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    Layout20->addWidget( TextLabel5_2 );
    Layout21->addLayout( Layout20 );

    Layout18 = new QVBoxLayout( 0, 0, 6, "Layout18"); 

    SongLabel = new QLabel( this, "SongLabel" );
    SongLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, 0, 0, SongLabel->sizePolicy().hasHeightForWidth() ) );
    Layout18->addWidget( SongLabel );

    Md5Label = new QLabel( this, "Md5Label" );
    Md5Label->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, 0, 0, Md5Label->sizePolicy().hasHeightForWidth() ) );
    Layout18->addWidget( Md5Label );
    Layout21->addLayout( Layout18 );

    AnalyzeDialogLayout->addMultiCellLayout( Layout21, 0, 0, 0, 2 );

    Layout20_2 = new QVBoxLayout( 0, 0, 6, "Layout20_2"); 

    TextLabel2_2 = new QLabel( this, "TextLabel2_2" );
    TextLabel2_2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    Layout20_2->addWidget( TextLabel2_2 );

    TextLabel3 = new QLabel( this, "TextLabel3" );
    TextLabel3->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    Layout20_2->addWidget( TextLabel3 );

    AnalyzeDialogLayout->addLayout( Layout20_2, 1, 0 );

    layout20 = new QVBoxLayout( 0, 0, 6, "layout20"); 

    TagEdit = new QLineEdit( this, "TagEdit" );
    layout20->addWidget( TagEdit );

    layout19 = new QHBoxLayout( 0, 0, 6, "layout19"); 

    IdxEdit = new QLineEdit( this, "IdxEdit" );
    IdxEdit->setEnabled( FALSE );
    layout19->addWidget( IdxEdit );
    layout20->addLayout( layout19 );

    AnalyzeDialogLayout->addMultiCellLayout( layout20, 1, 1, 1, 2 );

    pushButton13 = new QPushButton( this, "pushButton13" );

    AnalyzeDialogLayout->addWidget( pushButton13, 6, 2 );

    line1 = new QFrame( this, "line1" );
    line1->setFrameShape( QFrame::HLine );
    line1->setFrameShadow( QFrame::Sunken );
    line1->setFrameShape( QFrame::HLine );

    AnalyzeDialogLayout->addMultiCellWidget( line1, 5, 5, 0, 2 );

    pushButton7 = new QPushButton( this, "pushButton7" );

    AnalyzeDialogLayout->addMultiCellWidget( pushButton7, 2, 2, 0, 1 );

    pushButton8 = new QPushButton( this, "pushButton8" );

    AnalyzeDialogLayout->addMultiCellWidget( pushButton8, 3, 3, 0, 1 );

    pushButton11 = new QPushButton( this, "pushButton11" );

    AnalyzeDialogLayout->addWidget( pushButton11, 2, 2 );

    pushButton9 = new QPushButton( this, "pushButton9" );

    AnalyzeDialogLayout->addWidget( pushButton9, 3, 2 );

    pushButton10 = new QPushButton( this, "pushButton10" );

    AnalyzeDialogLayout->addMultiCellWidget( pushButton10, 4, 4, 0, 1 );
    QSpacerItem* spacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    AnalyzeDialogLayout->addItem( spacer, 4, 2 );
    languageChange();
    resize( QSize(396, 402).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pushButton13, SIGNAL( clicked() ), this, SLOT( accept() ) );

    // tab order
    setTabOrder( TagEdit, IdxEdit );
    setTabOrder( IdxEdit, pushButton7 );
    setTabOrder( pushButton7, pushButton8 );
    setTabOrder( pushButton8, pushButton10 );
    setTabOrder( pushButton10, pushButton11 );
    setTabOrder( pushButton11, pushButton9 );
    setTabOrder( pushButton9, pushButton13 );
    setTabOrder( pushButton13, BusyToggle );
}

/*
 *  Destroys the object and frees any allocated resources
 */
AnalyzeDialog::~AnalyzeDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void AnalyzeDialog::languageChange()
{
    setCaption( tr( "Edit / Analyzers" ) );
    BusyToggle->setText( QString::null );
    TextLabel1->setText( tr( "song =" ) );
    TextLabel5_2->setText( tr( "md5sum =" ) );
    SongLabel->setText( tr( "..." ) );
    Md5Label->setText( tr( "..." ) );
    TextLabel2_2->setText( tr( "tags =" ) );
    TextLabel3->setText( tr( ".idx =" ) );
    pushButton13->setText( tr( "Ok" ) );
    pushButton7->setText( tr( "Bpm Counter" ) );
    pushButton8->setText( tr( "Period Analyzer" ) );
    pushButton11->setText( tr( "Spectrum Analyzer" ) );
    pushButton9->setText( tr( "Tempo Line Analyzer" ) );
    pushButton10->setText( tr( "Ambience Analyzer" ) );
}

