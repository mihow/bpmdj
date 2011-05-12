/****************************************************************************
** Form implementation generated from reading ui file 'mixinformation.ui'
**
** Created: Tue Oct 21 14:25:38 2003
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "mixinformation.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a MixInformation as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
MixInformation::MixInformation( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "MixInformation" );
    MixInformationLayout = new QGridLayout( this, 1, 1, 11, 6, "MixInformationLayout"); 

    textLabel5 = new QLabel( this, "textLabel5" );
    textLabel5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)5, 0, 0, textLabel5->sizePolicy().hasHeightForWidth() ) );

    MixInformationLayout->addWidget( textLabel5, 2, 0 );

    commentEdit = new QLineEdit( this, "commentEdit" );

    MixInformationLayout->addMultiCellWidget( commentEdit, 2, 2, 1, 3 );

    line1 = new QFrame( this, "line1" );
    line1->setFrameShape( QFrame::HLine );
    line1->setFrameShadow( QFrame::Sunken );
    line1->setFrameShape( QFrame::HLine );

    MixInformationLayout->addMultiCellWidget( line1, 3, 3, 0, 3 );

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)5, 0, 0, textLabel1->sizePolicy().hasHeightForWidth() ) );

    MixInformationLayout->addWidget( textLabel1, 0, 0 );

    textLabel2 = new QLabel( this, "textLabel2" );
    textLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)5, 0, 0, textLabel2->sizePolicy().hasHeightForWidth() ) );

    MixInformationLayout->addWidget( textLabel2, 1, 0 );

    toLabel = new QLabel( this, "toLabel" );
    toLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)5, 0, 0, toLabel->sizePolicy().hasHeightForWidth() ) );

    MixInformationLayout->addMultiCellWidget( toLabel, 1, 1, 1, 3 );

    fromLabel = new QLabel( this, "fromLabel" );
    fromLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)5, 0, 0, fromLabel->sizePolicy().hasHeightForWidth() ) );

    MixInformationLayout->addMultiCellWidget( fromLabel, 0, 0, 1, 3 );
    QSpacerItem* spacer = new QSpacerItem( 290, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    MixInformationLayout->addMultiCell( spacer, 4, 4, 0, 1 );

    pushButton2 = new QPushButton( this, "pushButton2" );

    MixInformationLayout->addWidget( pushButton2, 4, 2 );

    pushButton1 = new QPushButton( this, "pushButton1" );

    MixInformationLayout->addWidget( pushButton1, 4, 3 );
    languageChange();
    resize( QSize(447, 146).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pushButton1, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( pushButton2, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( commentEdit, SIGNAL( returnPressed() ), this, SLOT( accept() ) );

    // tab order
    setTabOrder( commentEdit, pushButton1 );
    setTabOrder( pushButton1, pushButton2 );
}

/*
 *  Destroys the object and frees any allocated resources
 */
MixInformation::~MixInformation()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MixInformation::languageChange()
{
    setCaption( tr( "Mix Information" ) );
    textLabel5->setText( tr( "Comment :" ) );
    textLabel1->setText( tr( "From :" ) );
    textLabel2->setText( tr( "To :" ) );
    toLabel->setText( tr( "textLabel4" ) );
    fromLabel->setText( tr( "textLabel3" ) );
    pushButton2->setText( tr( "&Cancel" ) );
    pushButton2->setAccel( QKeySequence( tr( "Alt+C" ) ) );
    pushButton1->setText( tr( "&Ok" ) );
    pushButton1->setAccel( QKeySequence( tr( "Alt+O" ) ) );
}

