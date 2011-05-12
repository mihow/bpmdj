/****************************************************************************
** Form implementation generated from reading ui file 'pattern-analyzer.ui'
**
** Created: Sun Apr 18 15:41:32 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.3   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "pattern-analyzer.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qslider.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
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

    tabWidget2 = new QTabWidget( this, "tabWidget2" );

    tab = new QWidget( tabWidget2, "tab" );
    tabLayout = new QGridLayout( tab, 1, 1, 11, 6, "tabLayout"); 

    periodDelta = new QSlider( tab, "periodDelta" );
    periodDelta->setMinValue( -100 );
    periodDelta->setMaxValue( 100 );
    periodDelta->setOrientation( QSlider::Vertical );
    periodDelta->setTickmarks( QSlider::NoMarks );
    periodDelta->setTickInterval( 100 );

    tabLayout->addWidget( periodDelta, 1, 7 );

    periodDelta10 = new QSlider( tab, "periodDelta10" );
    periodDelta10->setMinValue( -1000 );
    periodDelta10->setMaxValue( 1000 );
    periodDelta10->setLineStep( 10 );
    periodDelta10->setPageStep( 100 );
    periodDelta10->setOrientation( QSlider::Vertical );
    periodDelta10->setTickmarks( QSlider::NoMarks );
    periodDelta10->setTickInterval( 100 );

    tabLayout->addWidget( periodDelta10, 1, 8 );

    textLabel1 = new QLabel( tab, "textLabel1" );

    tabLayout->addMultiCellWidget( textLabel1, 2, 2, 0, 1 );

    color = new QSlider( tab, "color" );
    color->setMaxValue( 255 );
    color->setPageStep( 5 );
    color->setOrientation( QSlider::Horizontal );

    tabLayout->addMultiCellWidget( color, 3, 3, 2, 8 );

    textLabel1_2 = new QLabel( tab, "textLabel1_2" );

    tabLayout->addMultiCellWidget( textLabel1_2, 3, 3, 0, 1 );

    pushButton4 = new QPushButton( tab, "pushButton4" );

    tabLayout->addWidget( pushButton4, 2, 5 );

    pushButton2 = new QPushButton( tab, "pushButton2" );

    tabLayout->addMultiCellWidget( pushButton2, 2, 2, 6, 8 );

    updateButton = new QPushButton( tab, "updateButton" );

    tabLayout->addWidget( updateButton, 2, 4 );

    beats = new QSpinBox( tab, "beats" );
    beats->setMaxValue( 32 );
    beats->setMinValue( 1 );
    beats->setValue( 4 );

    tabLayout->addWidget( beats, 2, 2 );
    QSpacerItem* spacer = new QSpacerItem( 239, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    tabLayout->addItem( spacer, 2, 3 );

    textLabel1_3 = new QLabel( tab, "textLabel1_3" );
    textLabel1_3->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );

    tabLayout->addMultiCellWidget( textLabel1_3, 0, 0, 0, 8 );

    pattern = new QLabel( tab, "pattern" );
    pattern->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)2, 0, 0, pattern->sizePolicy().hasHeightForWidth() ) );
    pattern->setFrameShape( QLabel::Panel );
    pattern->setScaledContents( TRUE );

    tabLayout->addMultiCellWidget( pattern, 1, 1, 1, 6 );

    projection = new QLabel( tab, "projection" );
    projection->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)2, 0, 0, projection->sizePolicy().hasHeightForWidth() ) );
    projection->setMaximumSize( QSize( 20, 32767 ) );
    projection->setScaledContents( TRUE );

    tabLayout->addWidget( projection, 1, 0 );
    tabWidget2->insertTab( tab, QString("") );

    TabPage = new QWidget( tabWidget2, "TabPage" );
    TabPageLayout = new QGridLayout( TabPage, 1, 1, 11, 6, "TabPageLayout"); 

    pushButton4_2 = new QPushButton( TabPage, "pushButton4_2" );

    TabPageLayout->addWidget( pushButton4_2, 3, 1 );
    QSpacerItem* spacer_2 = new QSpacerItem( 460, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    TabPageLayout->addItem( spacer_2, 3, 0 );

    pattern_demo = new QLabel( TabPage, "pattern_demo" );
    pattern_demo->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 0, 0, pattern_demo->sizePolicy().hasHeightForWidth() ) );
    pattern_demo->setMinimumSize( QSize( 0, 64 ) );
    pattern_demo->setFrameShape( QLabel::Panel );
    pattern_demo->setScaledContents( TRUE );

    TabPageLayout->addMultiCellWidget( pattern_demo, 2, 2, 0, 1 );

    rythm_pattern = new QLabel( TabPage, "rythm_pattern" );
    rythm_pattern->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)2, 0, 0, rythm_pattern->sizePolicy().hasHeightForWidth() ) );
    rythm_pattern->setFrameShape( QLabel::Panel );
    rythm_pattern->setScaledContents( TRUE );

    TabPageLayout->addMultiCellWidget( rythm_pattern, 0, 0, 0, 1 );

    layout5 = new QHBoxLayout( 0, 0, 6, "layout5"); 

    textLabel2_2_3 = new QLabel( TabPage, "textLabel2_2_3" );
    layout5->addWidget( textLabel2_2_3 );
    QSpacerItem* spacer_3 = new QSpacerItem( 110, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout5->addItem( spacer_3 );

    textLabel2_2_4 = new QLabel( TabPage, "textLabel2_2_4" );
    layout5->addWidget( textLabel2_2_4 );
    QSpacerItem* spacer_4 = new QSpacerItem( 70, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout5->addItem( spacer_4 );

    textLabel2_2 = new QLabel( TabPage, "textLabel2_2" );
    layout5->addWidget( textLabel2_2 );
    QSpacerItem* spacer_5 = new QSpacerItem( 80, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout5->addItem( spacer_5 );

    textLabel2_2_2_2 = new QLabel( TabPage, "textLabel2_2_2_2" );
    layout5->addWidget( textLabel2_2_2_2 );
    QSpacerItem* spacer_6 = new QSpacerItem( 46, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout5->addItem( spacer_6 );

    textLabel2_2_2 = new QLabel( TabPage, "textLabel2_2_2" );
    layout5->addWidget( textLabel2_2_2 );
    QSpacerItem* spacer_7 = new QSpacerItem( 90, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout5->addItem( spacer_7 );

    textLabel2_2_2_3 = new QLabel( TabPage, "textLabel2_2_2_3" );
    layout5->addWidget( textLabel2_2_2_3 );

    TabPageLayout->addMultiCellLayout( layout5, 1, 1, 0, 1 );
    tabWidget2->insertTab( TabPage, QString("") );

    tab_2 = new QWidget( tabWidget2, "tab_2" );
    tabLayout_2 = new QGridLayout( tab_2, 1, 1, 11, 6, "tabLayout_2"); 

    filterImage = new QLabel( tab_2, "filterImage" );
    filterImage->setScaledContents( TRUE );

    tabLayout_2->addMultiCellWidget( filterImage, 0, 0, 0, 3 );

    textLabel1_4 = new QLabel( tab_2, "textLabel1_4" );

    tabLayout_2->addWidget( textLabel1_4, 1, 0 );

    textLabel2 = new QLabel( tab_2, "textLabel2" );

    tabLayout_2->addWidget( textLabel2, 2, 0 );

    textLabel4 = new QLabel( tab_2, "textLabel4" );

    tabLayout_2->addWidget( textLabel4, 2, 2 );

    textLabel3 = new QLabel( tab_2, "textLabel3" );

    tabLayout_2->addWidget( textLabel3, 1, 2 );

    hicut = new QComboBox( FALSE, tab_2, "hicut" );

    tabLayout_2->addWidget( hicut, 2, 1 );

    locut = new QComboBox( FALSE, tab_2, "locut" );

    tabLayout_2->addWidget( locut, 1, 1 );

    order = new QLineEdit( tab_2, "order" );

    tabLayout_2->addWidget( order, 1, 3 );

    stacking = new QSpinBox( tab_2, "stacking" );
    stacking->setMaxValue( 8 );
    stacking->setMinValue( 1 );
    stacking->setValue( 1 );

    tabLayout_2->addWidget( stacking, 2, 3 );
    tabWidget2->insertTab( tab_2, QString("") );

    PatternDialogLayout->addWidget( tabWidget2, 0, 0 );
    languageChange();
    resize( QSize(795, 461).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( updateButton, SIGNAL( clicked() ), this, SLOT( showPattern() ) );
    connect( pushButton2, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( periodDelta, SIGNAL( valueChanged(int) ), this, SLOT( slantChanged() ) );
    connect( pushButton4, SIGNAL( clicked() ), this, SLOT( setTempo() ) );
    connect( periodDelta10, SIGNAL( valueChanged(int) ), this, SLOT( slantChanged() ) );
    connect( color, SIGNAL( valueChanged(int) ), this, SLOT( balanceChanged() ) );
    connect( beats, SIGNAL( valueChanged(int) ), this, SLOT( slantChanged() ) );
    connect( locut, SIGNAL( activated(const QString&) ), this, SLOT( filterChanged() ) );
    connect( hicut, SIGNAL( activated(const QString&) ), this, SLOT( filterChanged() ) );
    connect( order, SIGNAL( returnPressed() ), this, SLOT( filterChanged() ) );
    connect( stacking, SIGNAL( valueChanged(int) ), this, SLOT( filterChanged() ) );
    connect( pushButton4_2, SIGNAL( clicked() ), this, SLOT( calculateRythmPattern() ) );

    // tab order
    setTabOrder( locut, hicut );
    setTabOrder( hicut, order );
    setTabOrder( order, stacking );
    setTabOrder( stacking, tabWidget2 );
    setTabOrder( tabWidget2, periodDelta );
    setTabOrder( periodDelta, periodDelta10 );
    setTabOrder( periodDelta10, color );
    setTabOrder( color, pushButton4 );
    setTabOrder( pushButton4, pushButton2 );
    setTabOrder( pushButton2, updateButton );
    setTabOrder( updateButton, beats );
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
    projection->setText( QString::null );
    tabWidget2->changeTab( tab, tr( "Beat Graph" ) );
    pushButton4_2->setText( tr( "Calculate" ) );
    textLabel2_2_3->setText( tr( "8" ) );
    textLabel2_2_4->setText( tr( "1" ) );
    textLabel2_2->setText( tr( "1/8" ) );
    textLabel2_2_2_2->setText( tr( "1/16" ) );
    textLabel2_2_2->setText( tr( "1/24" ) );
    textLabel2_2_2_3->setText( tr( "1/32" ) );
    tabWidget2->changeTab( TabPage, tr( "Pattern" ) );
    textLabel1_4->setText( tr( "Lo cutoff frequency" ) );
    textLabel2->setText( tr( "Hi cutoff frequency" ) );
    textLabel4->setText( tr( "Stacking" ) );
    textLabel3->setText( tr( "Order" ) );
    hicut->clear();
    hicut->insertItem( tr( "0" ) );
    hicut->insertItem( tr( "100" ) );
    hicut->insertItem( tr( "200" ) );
    hicut->insertItem( tr( "300" ) );
    hicut->insertItem( tr( "400" ) );
    hicut->insertItem( tr( "510" ) );
    hicut->insertItem( tr( "630" ) );
    hicut->insertItem( tr( "770" ) );
    hicut->insertItem( tr( "920" ) );
    hicut->insertItem( tr( "1080" ) );
    hicut->insertItem( tr( "1270" ) );
    hicut->insertItem( tr( "1480" ) );
    hicut->insertItem( tr( "1720" ) );
    hicut->insertItem( tr( "2000" ) );
    hicut->insertItem( tr( "2380" ) );
    hicut->insertItem( tr( "2700" ) );
    hicut->insertItem( tr( "3150" ) );
    hicut->insertItem( tr( "3700" ) );
    hicut->insertItem( tr( "4400" ) );
    hicut->insertItem( tr( "5300" ) );
    hicut->insertItem( tr( "6400" ) );
    hicut->insertItem( tr( "7700" ) );
    hicut->insertItem( tr( "9500" ) );
    hicut->insertItem( tr( "12000" ) );
    hicut->insertItem( tr( "15500" ) );
    hicut->insertItem( tr( "22050" ) );
    hicut->setCurrentItem( 25 );
    locut->clear();
    locut->insertItem( tr( "0" ) );
    locut->insertItem( tr( "100" ) );
    locut->insertItem( tr( "200" ) );
    locut->insertItem( tr( "300" ) );
    locut->insertItem( tr( "400" ) );
    locut->insertItem( tr( "510" ) );
    locut->insertItem( tr( "630" ) );
    locut->insertItem( tr( "770" ) );
    locut->insertItem( tr( "920" ) );
    locut->insertItem( tr( "1080" ) );
    locut->insertItem( tr( "1270" ) );
    locut->insertItem( tr( "1480" ) );
    locut->insertItem( tr( "1720" ) );
    locut->insertItem( tr( "2000" ) );
    locut->insertItem( tr( "2380" ) );
    locut->insertItem( tr( "2700" ) );
    locut->insertItem( tr( "3150" ) );
    locut->insertItem( tr( "3700" ) );
    locut->insertItem( tr( "4400" ) );
    locut->insertItem( tr( "5300" ) );
    locut->insertItem( tr( "6400" ) );
    locut->insertItem( tr( "7700" ) );
    locut->insertItem( tr( "9500" ) );
    locut->insertItem( tr( "12000" ) );
    locut->insertItem( tr( "15500" ) );
    locut->insertItem( tr( "22050" ) );
    locut->setCurrentItem( 0 );
    order->setText( tr( "256" ) );
    tabWidget2->changeTab( tab_2, tr( "Filters" ) );
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

void PatternDialog::filterChanged()
{
    qWarning( "PatternDialog::filterChanged(): Not implemented yet" );
}

void PatternDialog::calculateRythmPattern()
{
    qWarning( "PatternDialog::calculateRythmPattern(): Not implemented yet" );
}

