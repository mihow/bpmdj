/****************************************************************************
** Form implementation generated from reading ui file 'song-information.ui'
**
** Created: Tue Oct 21 22:48:51 2003
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "song-information.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qframe.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a SongInformation as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SongInformation::SongInformation( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "SongInformation" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setSizeGripEnabled( TRUE );
    SongInformationLayout = new QGridLayout( this, 1, 1, 11, 6, "SongInformationLayout"); 

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

    idxLabel = new QLabel( this, "idxLabel" );
    idxLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, 0, 0, idxLabel->sizePolicy().hasHeightForWidth() ) );
    Layout18->addWidget( idxLabel );

    md5Label = new QLabel( this, "md5Label" );
    md5Label->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, 0, 0, md5Label->sizePolicy().hasHeightForWidth() ) );
    Layout18->addWidget( md5Label );
    Layout21->addLayout( Layout18 );

    SongInformationLayout->addMultiCellLayout( Layout21, 0, 0, 0, 2 );

    layout10 = new QHBoxLayout( 0, 0, 6, "layout10"); 

    layout8 = new QVBoxLayout( 0, 0, 6, "layout8"); 

    textLabel1 = new QLabel( this, "textLabel1" );
    textLabel1->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    layout8->addWidget( textLabel1 );

    textLabel2 = new QLabel( this, "textLabel2" );
    textLabel2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    layout8->addWidget( textLabel2 );

    textLabel3 = new QLabel( this, "textLabel3" );
    textLabel3->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    layout8->addWidget( textLabel3 );

    textLabel4 = new QLabel( this, "textLabel4" );
    textLabel4->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    layout8->addWidget( textLabel4 );

    TextLabel2_2 = new QLabel( this, "TextLabel2_2" );
    TextLabel2_2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    layout8->addWidget( TextLabel2_2 );
    layout10->addLayout( layout8 );

    layout9 = new QVBoxLayout( 0, 0, 6, "layout9"); 

    titleEdit = new QLineEdit( this, "titleEdit" );
    layout9->addWidget( titleEdit );

    authorEdit = new QLineEdit( this, "authorEdit" );
    layout9->addWidget( authorEdit );

    remixEdit = new QLineEdit( this, "remixEdit" );
    layout9->addWidget( remixEdit );

    versionEdit = new QLineEdit( this, "versionEdit" );
    layout9->addWidget( versionEdit );

    tagEdit = new QLineEdit( this, "tagEdit" );
    layout9->addWidget( tagEdit );
    layout10->addLayout( layout9 );

    SongInformationLayout->addMultiCellLayout( layout10, 1, 1, 0, 2 );

    line1 = new QFrame( this, "line1" );
    line1->setFrameShape( QFrame::HLine );
    line1->setFrameShadow( QFrame::Sunken );
    line1->setFrameShape( QFrame::HLine );

    SongInformationLayout->addMultiCellWidget( line1, 2, 2, 0, 2 );
    QSpacerItem* spacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    SongInformationLayout->addItem( spacer, 3, 0 );

    pushButton10 = new QPushButton( this, "pushButton10" );

    SongInformationLayout->addWidget( pushButton10, 3, 1 );

    pushButton13 = new QPushButton( this, "pushButton13" );

    SongInformationLayout->addWidget( pushButton13, 3, 2 );
    languageChange();
    resize( QSize(460, 278).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pushButton13, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( pushButton10, SIGNAL( clicked() ), this, SLOT( reject() ) );

    // tab order
    setTabOrder( titleEdit, tagEdit );
    setTabOrder( tagEdit, pushButton13 );
    setTabOrder( pushButton13, authorEdit );
    setTabOrder( authorEdit, remixEdit );
    setTabOrder( remixEdit, versionEdit );
    setTabOrder( versionEdit, pushButton10 );
}

/*
 *  Destroys the object and frees any allocated resources
 */
SongInformation::~SongInformation()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void SongInformation::languageChange()
{
    setCaption( tr( "Song Information" ) );
    TextLabel1->setText( tr( ".idx =" ) );
    TextLabel5_2->setText( tr( "md5sum =" ) );
    idxLabel->setText( tr( "..." ) );
    md5Label->setText( tr( "..." ) );
    textLabel1->setText( tr( "title =" ) );
    textLabel2->setText( tr( "author =" ) );
    textLabel3->setText( tr( "remix =" ) );
    textLabel4->setText( tr( "version =" ) );
    TextLabel2_2->setText( tr( "tags =" ) );
    pushButton10->setText( tr( "Cancel" ) );
    pushButton13->setText( tr( "Ok" ) );
}

