/****
 BpmDj: Free Dj Tools
 Copyright (C) 2001-2007 Werner Van Belle

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****/
using namespace std;
#line 1 "qvectorview.c++"
// A QVectorView suitable for BpmDj.
// This is a huge stripdown from trolltechs QListView class.
// TODO: move totalheight from QVectorViewData to QVectorView
#include <iostream>
#include <qtimer.h>
#include <qheader.h>
#include <qscrollview.h>
#include <qpainter.h>
#include <qcursor.h>
#include <qptrstack.h>
#include <qptrlist.h>
#include <qapplication.h>
#include <qbitmap.h>
#include <qdatetime.h>
#include <qptrdict.h>
#include <qptrvector.h>
#include <qiconset.h>
#include <qpixmapcache.h>
#include <qpopupmenu.h>
#include <qtl.h>
#include <qdragobject.h>
#include <qlineedit.h>
#include <qvbox.h>
#include <qstyle.h>
#include <assert.h>
#include <stdlib.h>
#include "qvectorview.h"
#include "vector-iterator.h"

class ItemColumnInfo
{
public:
  ItemColumnInfo(): next( 0 ), truncated( FALSE ), dirty( FALSE ), width( 0 ) {}
  ~ItemColumnInfo() { delete next; }
  ItemColumnInfo * next;
  uint truncated : 1;
  uint dirty : 1;
  int width;
};

class ViewColumnInfo 
{
public:
  ViewColumnInfo(): align(Qt::AlignAuto), sortable(TRUE), next( 0 ) {}
  ~ViewColumnInfo() { delete next; }
  int align;
  bool sortable;
  ViewColumnInfo * next;
};

QVectorViewData::QVectorViewData( )
{
    init();
}

void QVectorViewData::init()
{
    ownHeight = 0;
    configured = FALSE;
}

QVectorViewData::~QVectorViewData()
{
}

void QVectorViewData::setup(QVectorView * v)
{
  configured = true;
  int ph = 0;
  int h;
  h = QMAX( v->d_fontMetricsHeight, ph ) + 2 * v->itemMargin();
  h = QMAX( h, QApplication::globalStrut().height());
  if ( h % 2 > 0 ) h++;
  ownHeight = h;
}

void QVectorViewData::paintCell(QVectorView* lv, int number, QPainter * p, const QColorGroup & cg, int column, int width, int align )
{
  if ( !p ) return;
  QFontMetrics fm( p->fontMetrics() );
  QString t = text( number, column );
  int marg = lv->itemMargin();
  const BackgroundMode bgmode = lv->viewport()->backgroundMode();
  const QColorGroup::ColorRole crole = QPalette::backgroundRoleFromMode( bgmode );
  if ( cg.brush( crole ) != lv->colorGroup().brush( crole ) )
    p->fillRect( 0, 0, width, height(), cg.brush( crole ) );
  else
    lv->paintEmptyArea( p, QRect( 0, 0, width, height() ) );
  if ( isSelected(number) ) 
    {
      p->fillRect( 0 , 0, width, height(), cg.brush( QColorGroup::Highlight ) );
      p->setPen( cg.highlightedText() );
    } 
  else 
    p->setPen( cg.text() );
  if ( !t.isEmpty() ) {
    if ( !(align & AlignTop || align & AlignBottom) )
      align |= AlignVCenter;
    p->drawText(marg , 0, width, height(), align, t );
  }  
}

QVectorView::QVectorView( QWidget * parent, QVectorViewData * container, WFlags f)
    : QScrollView( parent, "VectorView", f | WStaticContents | WNoAutoErase )
{
  item_container = container;
  init();
  setupItemContainer();
}

void QVectorView::setupItemContainer()
{
  item_container->setup(this);
  totalHeight = item_container->ownHeight * item_container->vectorSize();
}

void QVectorView::vectorChanged()
{
  setupItemContainer();
  updateGeometries();
  triggerUpdate();
}

void QVectorView::init()
{
    d_vci = 0;
    d_timer = new QTimer( this );
    d_h = new QHeader( this, "list view header" );
    d_h->installEventFilter( this );
    d_focusItem = -1;
    d_oldFocusItem = -1;
    dirty_items.clear();
    d_dirtyItemTimer = new QTimer( this );
    d_visibleTimer = new QTimer( this );
    d_margin = 1;
    d_fontMetricsHeight = fontMetrics().height();
    d_h->setTracking(TRUE);
    d_buttonDown = FALSE;
    d_ignoreDoubleClick = FALSE;
    d_column.setAutoDelete( TRUE );
    d_scrollTimer = 0;
    d_sortIndicator = FALSE;
    d_clearing = FALSE;
    d_minLeftBearing = fontMetrics().minLeftBearing();
    d_minRightBearing = fontMetrics().minRightBearing();
    d_ellipsisWidth = fontMetrics().width( "..." ) * 2;
    d_highlighted = -1;
    d_pressedItem = -1;
    d_selectAnchor = -1;
    d_select = TRUE;
    d_updateHeader = FALSE;
    d_fullRepaintOnComlumnChange = FALSE;
    d_resizeMode = NoColumn;
    d_pressedEmptyArea = FALSE;
    d_inMenuMode = FALSE;
    setMouseTracking( TRUE );
    viewport()->setMouseTracking( TRUE );
    connect( d_timer, SIGNAL(timeout()), this, SLOT(updateContents()) );
    connect( d_dirtyItemTimer, SIGNAL(timeout()), this, SLOT(updateDirtyItems()) );
    connect( d_visibleTimer, SIGNAL(timeout()), this, SLOT(makeVisible()) );
    connect( d_h, SIGNAL(sizeChange( int, int, int )), this, SLOT(handleSizeChange( int, int, int )) );
    connect( d_h, SIGNAL(moved( int, int )), this, SLOT(triggerUpdate()) );
    connect( d_h, SIGNAL(sectionClicked( int )), this, SLOT(changeSortColumn( int )) );
    connect( d_h, SIGNAL(sectionHandleDoubleClicked( int )), this, SLOT(adjustColumn( int )) );
    connect( horizontalScrollBar(), SIGNAL(sliderMoved(int)), d_h, SLOT(setOffset(int)) );
    connect( horizontalScrollBar(), SIGNAL(valueChanged(int)), d_h, SLOT(setOffset(int)) );
    viewport()->setFocusProxy( this );
    viewport()->setFocusPolicy( WheelFocus );
    viewport()->setBackgroundMode( PaletteBase );
    setBackgroundMode( PaletteBackground, PaletteBase );
}

void QVectorView::setShowSortIndicator( bool show )
{
  d_sortIndicator = show;
}

bool QVectorView::showSortIndicator() const
{
  return d_sortIndicator;
}

void QVectorView::setResizeMode( ResizeMode m )
{
    d_resizeMode = m;
    if ( m == NoColumn )
	header()->setStretchEnabled( FALSE );
    else if ( m == AllColumns )
	header()->setStretchEnabled( TRUE );
    else
	header()->setStretchEnabled( TRUE, header()->count() - 1 );
}

QVectorView::ResizeMode QVectorView::resizeMode() const
{
    return d_resizeMode;
}

/*!
    Destroys the list view, deleting all its items, and frees up all
    allocated resources.
*/

QVectorView::~QVectorView()
{
  d_focusItem = -1;
  dirty_items.clear();
  delete d_vci;
  d_vci = 0;
}

void QVectorView::drawContentsOffset( QPainter * p, int ox, int oy,
				      int cx, int cy, int cw, int ch )
{
  if ( columns() == 0 ) 
    {
      paintEmptyArea( p, QRect( cx, cy, cw, ch ) );
      return;
    }
  
  buildDrawableList();
  
  if ( dirty_items.size()>0 ) 
    {
      QRect br( cx - ox, cy - oy, cw, ch );
      for(unsigned int j = 0; j < dirty_items.size() ; j++)
	{
	  int i = dirty_items[j];
	  QRect ir = itemRect( i ).intersect( viewport()->rect() );
	  // if the rectangle of the dirty item is outside the viewport
	  // or if it is within the area we will paint anyway, then we
	  // simply forget about it
	  if ( ir.isEmpty() || br.contains( ir ) )
	    {
	      // removal consists of swapping with the last
	      // and decreasing the number of items
	      assert(dirty_items.size()>=0);
	      vector<int>::iterator le=--dirty_items.end();
	      dirty_items[j]=*le;
	      dirty_items.erase(le);
	    }
	}
      if (dirty_items.size()>0) 
	// there are still dirty items left which we will not paint now
	// however we restart the timer
	d_dirtyItemTimer->start( 0, TRUE );
      else 
	// we covered all dirty items so we can stop the timer
	d_dirtyItemTimer->stop();
    }
  
  p->setFont( font() );
  
  QRect r;
  int fx = -1, x, fc = 0, lc = 0;
  int tx = -1;
  int ih = item_container->height();
  if (ih>0)
    for(int current = top_line_number ; current <=bottom_line_number ; current++)
      {
	int c;
	int cs;
	int y = itemPos(current);
	if ( y < cy+ch && y+ih >= cy )  // need to paint current?
	  {
	    item_container->preparePaint(this, current);
	    if ( fx < 0 ) 
	      {
		// find first interesting column, once
		x = 0;
		c = 0;
		cs = d_h->cellSize( 0 );
		while ( x + cs <= cx && c < d_h->count() ) 
		  {
		    x += cs;
		    c++;
		    if ( c < d_h->count() )
		      cs = d_h->cellSize( c );
		  }
		fx = x;
		fc = c;
		while( x < cx + cw && c < d_h->count() ) 
		  {
		    x += cs;
		    c++;
		    if ( c < d_h->count() )
		      cs = d_h->cellSize( c );
		  }
		lc = c;
	      }
	    
	    x = fx;
	    c = fc;
	    // draw to last interesting column
	    bool drawActiveSelection = hasFocus() || d_inMenuMode ||
	      !style().styleHint( QStyle::SH_ItemView_ChangeHighlightOnFocus, this );
	    const QColorGroup &cg = ( drawActiveSelection ? colorGroup() : palette().inactive() );
      
	    while ( c < lc )
	      {
		// int i = d_h->mapToLogical( c );
		cs = d_h->cellSize( c );
		r.setRect( x - ox, y - oy, cs, ih );
		p->save();
		// No need to paint if the cell isn't technically visible
		if ( !( r.width() == 0 || r.height() == 0 ) ) 
		  {
		    p->translate( r.left(), r.top() );
		    int ac = d_h->mapToLogical( c );
		    // map to Left currently. This should change once we can really reverse the listview.
		    int align = columnAlignment( ac );
		    if ( align == AlignAuto ) align = AlignLeft;
		    item_container->paintCell(this, current, p, cg, ac, r.width(), align);
		  }
		p->restore();
		x += cs;
		c++;
	      }	    
	  }
	
	const int cell = d_h->mapToActual( 0 );
	
	if ( tx < 0 )
	  tx = d_h->cellPos( cell );
      }
  if ( totalHeight < cy + ch )
    paintEmptyArea( p, QRect( cx - ox, totalHeight - oy, cw, cy + ch - totalHeight ) );
  int c = d_h->count()-1;
  if ( c >= 0 &&
       d_h->cellPos( c ) + d_h->cellSize( c ) < cx + cw ) {
    c = d_h->cellPos( c ) + d_h->cellSize( c );
    paintEmptyArea( p, QRect( c - ox, cy - oy, cx + cw - c, ch ) );
  }
}

void QVectorView::paintEmptyArea( QPainter * p, const QRect & rect )
{
  QStyleOption opt( 0, 0 ); // ### hack; in 3.1, add a property in QVectorView and QHeader
  QStyle::SFlags how = QStyle::Style_Default;
  if ( isEnabled() )
    how |= QStyle::Style_Enabled;
  
  style().drawComplexControl( QStyle::CC_ListView,
			      p, this, rect, colorGroup(),
			      how, QStyle::SC_ListView, QStyle::SC_None,
			      opt );
}

void QVectorView::buildDrawableList()
{
  // could mess with cy and ch in order to speed up vertical scrolling
  int cy = contentsY();
  int ch = ((QVectorView *)this)->visibleHeight();
  d_topPixel = cy + ch;   // one below bottom
  d_bottomPixel = cy - 1; // one above top
  int ih = item_container->height();
  top_line_number = d_bottomPixel / ih;
  bottom_line_number = 1 + d_topPixel / ih;
  int total_lines = item_container -> vectorSize();
  if (bottom_line_number >= total_lines)
    bottom_line_number = total_lines - 1;
  if (top_line_number >= total_lines)
    bottom_line_number = total_lines - 1;
}

void QVectorView::setContentsPos( int x, int y )
{
    updateGeometries();
    QScrollView::setContentsPos( x, y );
}

int QVectorView::addColumn( const QString &label, int width )
{
  int c = d_h->addLabel( label, width );
  d_column.resize( c+1 );
  d_column.insert( c, new Column );
  d_column[c]->wmode = width >=0 ? Manual : Maximum;
  updateGeometries();
  updateGeometry();
  return c;
}

int QVectorView::columns() const
{
    return d_column.count();
}

void QVectorView::setColumnText( int column, const QString &label )
{
  if ( column < d_h->count() ) 
    {
      d_h->setLabel( column, label );
      updateGeometries();
      updateGeometry();
    }
}

void QVectorView::setColumnText( int column, const QIconSet& iconset, const QString &label )
{
  if ( column < d_h->count() ) 
    {
      d_h->setLabel( column, iconset, label );
      updateGeometries();
    }
}

void QVectorView::setColumnWidth( int column, int w )
{
  if ( column < d_h->count() && d_h->sectionSize( column ) != w ) 
    {
      d_h->resizeSection( column, w );
      viewport()->update();
    }
}

void QVectorView::setSectionPos( int column, int index )
{
  d_h->moveSection(column,index);
  viewport()->update();
}

QString QVectorView::columnText( int c ) const
{
    return d_h->label(c);
}

int QVectorView::columnWidth( int c ) const
{
    int actual = d_h->mapToActual( c );
    return d_h->cellSize( actual );
}

void QVectorView::setColumnWidthMode( int c, WidthMode mode )
{
    if ( c < d_h->count() )
	 d_column[c]->wmode = mode;
}

QVectorView::WidthMode QVectorView::columnWidthMode( int c ) const
{
    if ( c < d_h->count() )
	return d_column[c]->wmode;
    else
	return Manual;
}

void QVectorView::setColumnAlignment( int column, int align )
{
  if ( column < 0 )
    return;
  if ( !d_vci )
    d_vci = new ViewColumnInfo;
  ViewColumnInfo * l = d_vci;
  while( column ) 
    {
      if ( !l->next )
	l->next = new ViewColumnInfo;
      l = l->next;
      column--;
    }
  if ( l->align == align )
    return;
  l->align = align;
  triggerUpdate();
}

int QVectorView::columnAlignment( int column ) const
{
  if ( column < 0 || !d_vci )
    return AlignAuto;
  ViewColumnInfo * l = d_vci;
  while( column ) {
    if ( !l->next )
      l->next = new ViewColumnInfo;
    l = l->next;
    column--;
  }
  return l ? l->align : AlignAuto;
}

void QVectorView::show()
{
  // Reimplemented to setx the correct background mode and viewed
  // area size.
  if ( !isVisible() ) 
    {
      reconfigureItems();
      updateGeometries();
    }
  QScrollView::show();
}

void QVectorView::updateContents()
{
    if ( d_updateHeader )
	header()->adjustHeaderSize();
    d_updateHeader = FALSE;
    if ( !isVisible() ) {
	// Not in response to a setText/setPixmap any more.
	return;
    }
    viewport()->setUpdatesEnabled( FALSE );
    updateGeometries();
    viewport()->setUpdatesEnabled( TRUE );
    viewport()->repaint( FALSE );
}


void QVectorView::updateGeometries()
{
  int th = totalHeight; 
  int tw = d_h->headerWidth();
  if ( d_h->offset() &&
       tw < d_h->offset() + d_h->width() )
    horizontalScrollBar()->setValue( tw - QVectorView::d_h->width() );
  verticalScrollBar()->raise();
  resizeContents( tw, th );
  if ( d_h->isHidden() ) 
    {
      setMargins( 0, 0, 0, 0 );
    } 
  else 
    {
      QSize hs( d_h->sizeHint() );
      setMargins( 0, hs.height(), 0, 0 );
      d_h->setGeometry( viewport()->x(), viewport()->y()-hs.height(),
			 visibleWidth(), hs.height() );
    }
}

void QVectorView::handleSizeChange( int section, int os, int ns )
{
    bool upe = viewport()->isUpdatesEnabled();
    viewport()->setUpdatesEnabled( FALSE );
    int sx = horizontalScrollBar()->value();
    bool sv = horizontalScrollBar()->isVisible();
    updateGeometries();
    bool fullRepaint = d_fullRepaintOnComlumnChange || sx != horizontalScrollBar()->value()
		       || sv != horizontalScrollBar()->isVisible();
    d_fullRepaintOnComlumnChange = FALSE;
    viewport()->setUpdatesEnabled( upe );

    if ( fullRepaint ) {
	viewport()->repaint( FALSE );
	return;
    }

    int actual = d_h->mapToActual( section );
    int dx = ns - os;
    int left = d_h->cellPos( actual ) - contentsX() + d_h->cellSize( actual );
    if ( dx > 0 )
	left -= dx;
    if ( left < visibleWidth() )
	viewport()->scroll( dx, 0, QRect( left, 0, visibleWidth() - left, visibleHeight() ) );
    viewport()->repaint( left - 4 - d_ellipsisWidth, 0, 4 + d_ellipsisWidth,
			 visibleHeight(), FALSE ); // border between the items and ellipses width

    // map auto to left for now. Need to fix this once we support
    // reverse layout on the listview.
    int align = columnAlignment( section );
    if ( align == AlignAuto ) align = AlignLeft;
    if ( align != AlignAuto && align != AlignLeft )
	viewport()->repaint( d_h->cellPos( actual ) - contentsX(), 0,
			     d_h->cellSize( actual ), visibleHeight() );
}

void QVectorView::updateDirtyItems()
{
  if ( d_timer->isActive() || !dirty_items.size() ) return;
  QRect ir; // rectangle to be repainted
  for(unsigned int j = 0 ; j < dirty_items.size() ; j ++)
    {
      int i = dirty_items[j];
      ir = ir.unite( itemRect(i) );
    }
  if ( !ir.isEmpty() ) 
    {		     
      if ( ir.x() < 0 ) ir.moveBy( -ir.x(), 0 );
      viewport()->repaint( ir, FALSE );
    }
}

void QVectorView::makeVisible()
{
  if ( d_focusItem>=0 )
    ensureItemVisible( d_focusItem );
}

void QVectorView::resizeEvent( QResizeEvent *e )
{
  QScrollView::resizeEvent( e );
  d_fullRepaintOnComlumnChange = TRUE;
  d_h->resize( visibleWidth(), d_h->height() );
}

void QVectorView::viewportResizeEvent( QResizeEvent *e )
{
  QScrollView::viewportResizeEvent( e );
  d_h->resize( visibleWidth(), d_h->height() );
}

void QVectorView::triggerUpdate()
{
  if ( !isVisible() || !isUpdatesEnabled() ) return; 
  d_timer->start( 0, TRUE );
}

bool QVectorView::eventFilter( QObject * o, QEvent * e )
{
  if ( o == d_h &&
       e->type() >= QEvent::MouseButtonPress &&
       e->type() <= QEvent::MouseMove ) 
    {
      QMouseEvent * me = (QMouseEvent *)e;
      QMouseEvent me2( me->type(),
		       QPoint( me->pos().x(),
			       me->pos().y() - d_h->height() ),
		       me->button(), me->state() );
      switch( me2.type() )
	{
	case QEvent::MouseButtonDblClick:
	  if ( me2.button() == RightButton )
	    return TRUE;
	  break;
	case QEvent::MouseMove:
	  if ( me2.state() & RightButton ) 
	    {
	      viewportMouseMoveEvent( &me2 );
	      return TRUE;
	    }
	  break;
	default:
	  break;
	}
    } 
  else if ( o == viewport() )
    {
      QFocusEvent * fe = (QFocusEvent *)e;
      switch( e->type() ) 
	{
	case QEvent::FocusIn:
	  focusInEvent( fe );
	  return TRUE;
	case QEvent::FocusOut:
	  focusOutEvent( fe );
	  return TRUE;
	default:
	  // nothing
	  break;
	}
    } 
  return QScrollView::eventFilter( o, e );
}

int QVectorViewData::height() const
{
    assert ( configured );
    return ownHeight;
}

void QVectorView::contentsMousePressEvent( QMouseEvent * e )
{
    contentsMousePressEventEx( e );
}

void QVectorView::contentsMousePressEventEx( QMouseEvent * e )
{
  if ( !e ) return;
  QPoint vp = contentsToViewport( e->pos() );
  
  d_ignoreDoubleClick = FALSE;
  d_buttonDown = TRUE;
  
  int i = itemAt( vp );
  d_pressedEmptyArea = e->y() > contentsHeight();
  int oldCurrent = currentItem();
  
  if ( e->button() == RightButton)
    {
      // if the element is not selected, then select it
      if (i>=0 && !isSelected(i))
	setSelected(i,true);
      goto emit_signals;
    }
  
  if ( i<0 ) 
    {
      if ( !( e->state() & ControlButton ) )
	clearSelection();
      goto emit_signals;
    } 
  else 
    {
      // No new anchor when using shift
      if ( !(e->state() & ShiftButton) )
	d_selectAnchor = i;
    }
  
  d_select = TRUE;
  
  if ( i != d_focusItem )
    setCurrentItem( i );
  else
    repaintItem( i );
  
  d_pressedSelected = i>=0 && isSelected(i);
  
  {
    bool changed = FALSE;
    if ( !(e->state() & (ControlButton | ShiftButton)) ) 
      {
	if ( !isSelected(i) ) 
	  {
	    bool blocked = signalsBlocked();
	    blockSignals( TRUE );
	    clearSelection();
	    blockSignals( blocked );
	    setSelected(i, TRUE );
	    changed = TRUE;
	  }
      } 
    else 
      {
	if ( e->state() & ShiftButton )
	  d_pressedSelected = FALSE;
	if ( (e->state() & ControlButton) && !(e->state() & ShiftButton) && i>=0 ) 
	  {
	    setSelected(i,!isSelected(i) );
	    changed = TRUE;
	    d_pressedSelected = FALSE;
	  } 
	else if ( oldCurrent<0 || i<0 || oldCurrent == i ) 
	  {
	    if ( isSelected(i) != d_select ) {
	      changed = TRUE;
	      setSelected(i, d_select );
	    }
	    // Shift pressed in Extended mode ---
	  } 
	else 
	  changed = selectRange( i, oldCurrent, d_selectAnchor );
      }
    if ( changed ) 
      {
	triggerUpdate();
	emit selectionChanged();
      }
  }
  
 emit_signals:
  
  if (i<0) return;
  if (!d_buttonDown && vp.x() + contentsX() < itemMargin() )
    i = -1;
  d_pressedItem = i;
  
  int c = i>=0 ? d_h->mapToLogical( d_h->cellAt( vp.x() ) ) : -1;
  emit pressed( i );
  emit pressed( i, viewport()->mapToGlobal( vp ), c );
  emit mouseButtonPressed( e->button(), i, viewport()->mapToGlobal( vp ), c );
  if ( e->button() == RightButton && i == d_pressedItem ) 
    emit rightButtonPressed( i, viewport()->mapToGlobal( vp ), c );
}

void QVectorView::contentsMouseReleaseEvent( QMouseEvent * e )
{
    contentsMouseReleaseEventEx( e );
}

void QVectorView::contentsMouseReleaseEventEx( QMouseEvent * e )
{
  bool emitClicked = !d_pressedItem || d_buttonDown;
  d_buttonDown = FALSE;
  // delete and disconnect autoscroll timer, if we have one
  if ( d_scrollTimer ) 
    {
      disconnect( d_scrollTimer, SIGNAL(timeout()), this, SLOT(doAutoScroll()) );
      //printf("Disconnected scrolling timer\n");
      d_scrollTimer->stop();
      delete d_scrollTimer;
      d_scrollTimer = 0;
    }

  if ( !e ) return;

  if ( d_focusItem == d_pressedItem &&
       d_pressedSelected && d_focusItem &&
       e->button() == LeftButton) 
    {
      bool block = signalsBlocked();
      blockSignals( TRUE );
      clearSelection();
      blockSignals( block );
      setSelected(d_focusItem, TRUE );
      emit selectionChanged();
    }
  
  QPoint vp = contentsToViewport(e->pos());
  int i = itemAt( vp );

  if ( i>=0 && vp.x() + contentsX() < itemMargin())
    i = -1;
  emitClicked = emitClicked && d_pressedItem == i;
  d_pressedItem = 0;
  
  if ( emitClicked ) 
    {
      emit clicked( i );
      emit clicked( i, viewport()->mapToGlobal( vp ), d_h->mapToLogical( d_h->cellAt( vp.x() ) ) );
      emit mouseButtonClicked( e->button(), i, viewport()->mapToGlobal( vp ), i ? d_h->mapToLogical( d_h->cellAt( vp.x() ) ) : -1 );
      if ( e->button() == RightButton ) {
	if ( i<0 ) 
	  {
	    if ( !(e->state() & ControlButton) )
	      clearSelection();
	    emit rightButtonClicked( 0, viewport()->mapToGlobal( vp ), -1 );
	    return;
	  }
	
	int c = d_h->mapToLogical( d_h->cellAt( vp.x() ) );
	emit rightButtonClicked( i, viewport()->mapToGlobal( vp ), c );
      }
    }
}

void QVectorView::contentsMouseDoubleClickEvent( QMouseEvent * e )
{
  if ( !e || e->button() != LeftButton )
    return;
  
  // ensure that the following mouse moves and eventual release is
  // ignored.
  d_buttonDown = FALSE;
  
  if ( d_ignoreDoubleClick ) 
    {
      d_ignoreDoubleClick = FALSE;
      return;
    }
  
  QPoint vp = contentsToViewport(e->pos());

  int i = itemAt( vp );
  
  // we emit doubleClicked when the item is null (or enabled) to be consistent with
  // rightButtonClicked etc.
  int c = d_h->mapToLogical( d_h->cellAt( vp.x() ) );
  emit doubleClicked( i, viewport()->mapToGlobal( vp ), c );

  // we emit the 'old' obsolete doubleClicked only if the item is not null and enabled
  emit doubleClicked( i );
}

void QVectorView::contentsMouseMoveEvent( QMouseEvent * e )
{
  if ( !e ) return;
  bool needAutoScroll = FALSE;
  QPoint vp = contentsToViewport(e->pos());
  int i = itemAt( vp );
  if ( i != d_highlighted ) 
    {
      if ( i>=0 ) 
	emit onItem( i );
      else 
	emit onViewport();
      d_highlighted = i;
    }
  
  if ( !d_buttonDown ||
       ( ( e->state() & LeftButton ) != LeftButton &&
	 ( e->state() & MidButton ) != MidButton &&
	 ( e->state() & RightButton ) != RightButton ) )
    return;
  
  // check, if we need to scroll
  if ( vp.y() > visibleHeight() || vp.y() < 0 )
    needAutoScroll = TRUE;
  
  // if we need to scroll and no autoscroll timer is started,
  // connect the timer
  if ( needAutoScroll && !d_scrollTimer ) 
    {
      d_scrollTimer = new QTimer( this );
      connect( d_scrollTimer, SIGNAL(timeout()),this, SLOT(doAutoScroll()) );
      d_scrollTimer->start( 100, FALSE );
      //printf("Connected scrolling timer\n");

      // call it once manually
      doAutoScroll( vp );
    }

  // if we don't need to autoscroll
  if ( !needAutoScroll ) 
    {
      // if there is a autoscroll timer, delete it
      if ( d_scrollTimer ) 
	{
	  disconnect( d_scrollTimer, SIGNAL(timeout()), this, SLOT(doAutoScroll()) );
	  d_scrollTimer->stop();
	  //printf("Disconnected scrolling timer\n");
	  delete d_scrollTimer;
	  d_scrollTimer = 0;
	}
      // call this to select an item ( using the pos from the event )
      //printf("yeah ");
      doAutoScroll( vp );
    }
}

void QVectorView::doAutoScroll()
{
  //printf(" !!! ");
  doAutoScroll( QPoint() );
}

void QVectorView::doAutoScroll( const QPoint &cursorPos )
{
  //printf(" ... autoscroll ...\n");
  QPoint pos = cursorPos.isNull() ? viewport()->mapFromGlobal( QCursor::pos() ) :  cursorPos;
  if ( d_focusItem<0 || ( d_pressedEmptyArea && pos.y() > contentsHeight() ) ) 
    {
      //printf("aborted %d\n",d_focusItem);
      return;
    }
  bool down = pos.y() > itemRect( d_focusItem ).y();
  //  if (down) 
  //printf("  down  ");
  //else
  //printf("  up    ");
  int g = pos.y() + contentsY();
  if ( down && pos.y() > height()  )
    g = height() + contentsY();
  else if ( pos.y() < 0 )
    g = contentsY();
  
  int c = d_focusItem, old = -1;
  int oldCurrent = c;
  
  // if ( c == 0 || d_focusItem == 0)
  //printf("Step 1: %d , %d\n",d_focusItem,c);
  
  if ( down ) 
    {
      int y = itemRect( d_focusItem ).y() + contentsY();
      while( y + item_container->height() <= g ) 
	{
	  y += item_container->height();
	  old = c++;
	}
      if ( c >= item_container->vectorSize()) c = old;
    } 
  else 
    {
      int y = itemRect( d_focusItem ).y() + contentsY();
      while( c>=0 && y >= g ) 
	{
	  old = c--;
	  if ( c>=0 ) y -= item_container->height();
	}
      if (c<0) c = old;
    }
  
  if ( c<0 || c == d_focusItem ) 
    return;
    
  if ( d_focusItem>=0 ) 
    {
      if ( selectRange( c, oldCurrent, d_selectAnchor ) ) 
	{
	  triggerUpdate();
	  emit selectionChanged();
	}
    }
  
  setCurrentItem( c );
  if (!d_visibleTimer->isActive())
    d_visibleTimer->start( 1, TRUE );
}

void QVectorView::focusInEvent( QFocusEvent* )
{
  d_inMenuMode = FALSE;
  if ( d_focusItem >=0 ) 
    {
      repaintItem( d_focusItem );
    } 
  else if (QFocusEvent::reason() != QFocusEvent::Mouse ) 
    {
      d_focusItem = -1;
      emit currentChanged( d_focusItem );
      repaintItem( d_focusItem );
    }
  if ( style().styleHint( QStyle::SH_ItemView_ChangeHighlightOnFocus, this ) ) 
    {
      viewport()->repaint( FALSE );
    }
  QRect mfrect = itemRect( d_focusItem );
  if ( mfrect.isValid() ) 
    {
      if ( header() && header()->isVisible() )
	setMicroFocusHint( mfrect.x(), mfrect.y()+header()->height(), mfrect.width(), mfrect.height(), FALSE );
      else
	setMicroFocusHint( mfrect.x(), mfrect.y(), mfrect.width(), mfrect.height(), FALSE );
    }
}

void QVectorView::focusOutEvent( QFocusEvent* )
{
  if ( QFocusEvent::reason() == QFocusEvent::Popup && d_buttonDown )
    d_buttonDown = FALSE;
  if ( style().styleHint( QStyle::SH_ItemView_ChangeHighlightOnFocus, this ) ) 
    {
      d_inMenuMode =
	QFocusEvent::reason() == QFocusEvent::Popup
	|| (qApp->focusWidget() && qApp->focusWidget()->inherits("QMenuBar"));
      if ( !d_inMenuMode ) 
	{
	  viewport()->repaint( FALSE );
	}
    }
  
  if ( d_focusItem>=0 )
    repaintItem( d_focusItem );
}

void QVectorView::keyPressEvent( QKeyEvent * e )
{
  if ( !e || item_container->vectorSize()==0 ) 
    {
      assert(e!=NULL);
      e->ignore();
      return; // subclass bug
    }
  int oldCurrent = currentItem();
  if ( oldCurrent<0 ) 
    {
      setCurrentItem( 0 );
      return;
    }
  int i = currentItem();
  int old = i;
  if ( e->ascii() == ' ' )
    {
      setSelected( i, !isSelected(i));
      return;
    }
  
  QRect r( itemRect( i ) );
  int i2;
  bool singleStep = FALSE;
  bool selectCurrent = TRUE;
  switch( e->key() ) 
    {
    case Key_Backspace:
    case Key_Delete:
      break;
    case Key_Enter:
    case Key_Return:
      if ( e->state() & ControlButton ) 
	{
	  e->ignore();
	  emit ctrlReturnPressed( currentItem() );
	}
      else
	{
	  e->ignore();
	  emit returnPressed( currentItem() );
	}
      // do NOT accept.  QDialog.
      return;
    case Key_Down:
      selectCurrent = FALSE;
      i++;
      singleStep = TRUE;
      break;
    case Key_Up:
      selectCurrent = FALSE;
      i--;
      singleStep = TRUE;
      break;
    case Key_Home:
      selectCurrent = FALSE;
      i = 0;
      break;
    case Key_End:
      selectCurrent = FALSE;
      i = item_container->vectorSize()-1;
      break;
    case Key_Next:
      selectCurrent = FALSE;
      i2 = itemAt( QPoint( 0, visibleHeight()-1 ) );
      if ( i2 == i || !r.isValid() ||
	   visibleHeight() <= itemRect( i ).bottom() ) 
	{
	  if ( i2 )
	    i = i2;
	  int left = visibleHeight();
	  while( (i2 = i+1) != 0 && left > item_container->height() ) 
	    {
	      left -= item_container->height();
	      i = i2;
	    }
	} 
      else {
	if ( !i2 ) 
	  {
	    // list is shorter than the view, goto last item
	    while( (i2 = i+1) != 0 )
	      i = i2;
	  } 
	else 
	  {
	    i = i2;
	  }
      }
      break;
    case Key_Prior:
      selectCurrent = FALSE;
      i2 = itemAt( QPoint( 0, 0 ) );
      if ( i == i2 || !r.isValid() || r.top() <= 0 ) 
	{
	  if ( i2 )
	    i = i2;
	  int left = visibleHeight();
	  while( (i2 = i-1) >= 0 && left > item_container->height() ) 
	    {
	      left -= item_container->height();
	      i = i2;
	    }
	}
      else 
	{
	  i = i2;
	}
      break;
    case Key_Plus: return;
    case Key_Right:
      if ( contentsX() + visibleWidth() < contentsWidth() ) 
	horizontalScrollBar()->addLine();
      return;
    case Key_Minus: return;
    case Key_Left:
      if ( contentsX() ) 
	horizontalScrollBar()->subtractLine();
      return;
    case Key_Space:
      emit spacePressed( currentItem() );
      break;
    case Key_Escape:
      e->ignore(); // For QDialog
      return;
    case Key_F2:
    default:
      {
	if ( e->state() & ControlButton ) 
	  {
	    switch ( e->key() ) 
	      {
	      case Key_A:
		selectAll( TRUE );
		break;
	      }
	  }
	e->ignore();
	return;
      }
    }
  
  if ( i<0 || i>=item_container->vectorSize() ) return;
  
  if ( !( e->state() & ShiftButton ) || d_selectAnchor<0 )
    d_selectAnchor = i;
  
  setCurrentItem( i );
  handleItemChange( old, e->state() & ShiftButton, e->state() & ControlButton );
  
  if ( singleStep )
    {
      if (!d_visibleTimer->isActive())
	d_visibleTimer->start( 1, TRUE );
    }
  else
    ensureItemVisible( i );
}

int QVectorView::itemAt( const QPoint & viewPos )
{
  if ( viewPos.x() > contentsWidth() - contentsX() ) 
    return 0;
  buildDrawableList();
  int l = top_line_number;
  int y = itemPos(l);
  int g = viewPos.y() + contentsY();
  
  while( l <= bottom_line_number && y + item_container->height() <= g  )
    {
      l++;
      y += item_container->height();
    }
  
  if (l<item_container->vectorSize())
    return l;
  return -1;
}

int QVectorView::itemPos( int item )
{
  return item>=0 ? item * item_container->height() : 0;
}

void QVectorView::setSelected( int item, bool selected )
{
  if ( item<0 || 
       item >= item_container->vectorSize() || 
       isSelected(item)== selected)
    return;
  
  bool emitHighlighted = FALSE;

  item_container->setSelected(item,selected);
  repaintItem( item );
  
  emit selectionChanged();
  if ( emitHighlighted )
    emit currentChanged( d_focusItem );
}
 
void QVectorView::setSelectionAnchor( int item )
{
  d_selectAnchor = item;
}

void QVectorView::clearSelection()
{
  selectAll( FALSE );
}

void QVectorView::selectAll( bool select )
{
  bool b = signalsBlocked();
  blockSignals( TRUE );
  bool anything = FALSE;
  for(int i = item_container->vectorSize() ; i>=0 ; i--)
    if ( isSelected(i) != select ) 
      {
	setSelected( i,select );
	anything = TRUE;
      }
  blockSignals( b );
  if ( anything ) 
    {
      emit selectionChanged();
      triggerUpdate();
    }
}

void QVectorView::invertSelection()
{
  bool b = signalsBlocked();
  blockSignals( TRUE );
  for (int i = item_container->vectorSize() ; i>=0 ; i--)
    setSelected(i,!isSelected(i));
  blockSignals( b );
  emit selectionChanged();
  triggerUpdate();
}

bool QVectorView::isSelected( int i ) const
{
  if (i<0) return false;
  if (i>=item_container->vectorSize()) return false;
  return item_container->isSelected(i);
}

void QVectorView::setCurrentItem( int i )
{
  if ( i<0 || i>=item_container->vectorSize() || d_focusItem == i)
    return;
  
  int prev = d_focusItem;
  d_focusItem = i;
  
  QRect mfrect = itemRect( i );
  if ( mfrect.isValid() ) {
    if ( header() && header()->isVisible() )
      setMicroFocusHint( mfrect.x(), mfrect.y()+header()->height(), mfrect.width(), mfrect.height(), FALSE );
    else
      setMicroFocusHint( mfrect.x(), mfrect.y(), mfrect.width(), mfrect.height(), FALSE );
  }
  
  if ( i != prev ) 
    {
      if ( i )
	repaintItem( i );
      if ( prev )
	repaintItem( prev );
      emit currentChanged( i );
    }
}

int QVectorView::currentItem() const
{
  return d_focusItem;
}

QRect QVectorView::itemRect( int i ) const
{
  int y = i*item_container->height() - contentsY();
  if ( y + item_container->height() >= 0 &&
       y < ((QVectorView *)this)->visibleHeight() ) 
    return QRect( -contentsX(), y, d_h->width(), item_container->height() );
  return QRect(0,0,-1,-1);
}

void QVectorView::setSorting( int column, bool asc )
{
  if ( d_sortIndicator )
    d_h->setSortIndicator( column,asc );
  item_container->sort(column,asc);
  triggerUpdate();
}

void QVectorView::changeSortColumn( int column )
{
  int lcol = d_h->mapToLogical( column );
  setSorting( lcol, d_h->sortIndicatorOrder() != Qt::Ascending);
}

void QVectorView::setItemMargin( int m )
{
  if ( d_margin == m )
    return;
  d_margin = m;
  triggerUpdate();
}

int QVectorView::itemMargin() const
{
    return d_margin;
}

void QVectorView::styleChange( QStyle& old )
{
    QScrollView::styleChange( old );
    reconfigureItems();
}

void QVectorView::setFont( const QFont & f )
{
    QScrollView::setFont( f );
    reconfigureItems();
}

void QVectorView::setPalette( const QPalette & p )
{
    QScrollView::setPalette( p );
    reconfigureItems();
}

void QVectorView::reconfigureItems()
{
  d_fontMetricsHeight = fontMetrics().height();
  d_minLeftBearing = fontMetrics().minLeftBearing();
  d_minRightBearing = fontMetrics().minRightBearing();
  d_ellipsisWidth = fontMetrics().width( "..." ) * 2;
}

void QVectorView::widthChanged( int c )
{
}

int QVectorView::firstChild() const
{
  return item_container->vectorSize() ? 0 : -1;
}

int QVectorView::lastItem() const
{
  return item_container->vectorSize() -1;
}

void QVectorView::repaintItem( int item )
{
  if ( item<0 || item >= item_container->vectorSize() ) return;
  d_dirtyItemTimer->start( 0, TRUE );
  vectorIterator<int> i(dirty_items); ITERATE_OVER(i)
    if (i.val()==item) return; // its already there
  }
  dirty_items.push_back(item);
}

QSize QVectorView::sizeHint() const
{
  if ( cachedSizeHint().isValid() )
    return cachedSizeHint();
  constPolish();
  // buildDrawableList();
  QSize s( d_h->sizeHint() );
  s.setWidth( s.width() + style().pixelMetric(QStyle::PM_ScrollBarExtent) );
  s += QSize(frameWidth()*2,frameWidth()*2);
  s.setHeight( s.height() + 10 * item_container->height() );
  if ( s.width() > s.height() * 3 )
    s.setHeight( s.width() / 3 );
  else if ( s.width() *3 < s.height() )
    s.setHeight( s.width() * 3 );
  setCachedSizeHint( s );
  return s;
}


QSize QVectorView::minimumSizeHint() const
{
    return QScrollView::minimumSizeHint();
}

void QVectorView::setRootIsDecorated( bool enable )
{
}

bool QVectorView::rootIsDecorated() const
{
  return false;
}

void QVectorView::ensureItemVisible( int i )
{
  if ( i<0 ) return;
  int y = itemPos( i );
  int h = item_container->height();
  if ( isVisible() && y + h > contentsY() + visibleHeight() )
    setContentsPos( contentsX(), y - visibleHeight() + h );
  else if ( !isVisible() || y < contentsY() )
    setContentsPos( contentsX(), y );
}

QHeader * QVectorView::header() const
{
  return d_h;
}

void QVectorView::showEvent( QShowEvent * )
{
  dirty_items.clear();
  d_dirtyItemTimer->stop();
  d_fullRepaintOnComlumnChange = TRUE;
  updateGeometries();
}

void QVectorView::handleItemChange( int old, bool shift, bool control )
{
  if ( shift ) 
    {
      selectRange( d_selectAnchor>=0 ? d_selectAnchor : old,
		   d_focusItem, FALSE, TRUE, d_selectAnchor>=0 ? TRUE : FALSE );
    }
  else if ( !control ) 
    {
      bool block = signalsBlocked();
      blockSignals( TRUE );
      selectAll( FALSE );
      blockSignals( block );
      setSelected( d_focusItem, TRUE );
    }
} 

/* unselects items from to, including children, returns TRUE if any items were unselected */
bool QVectorView::clearRange( int from, int to, bool includeFirst )
{
  if ( from<0 || to<0 ) return FALSE;
  if ( from > to )   // Swap ?
    {
      int temp = from;
      from = to;
      to = temp;
    }
  
  if ( !includeFirst ) // Start on second?
    {
      int below = (from == to) ? from : from+1;
      if ( below < item_container->vectorSize())
	from = below;
    }
  
  bool changed = FALSE; // Clear items <from, to>
  for(int i = from ; i <=to ; i ++)
    if ( isSelected(i)) 
      {
	setSelected(i, FALSE );
	changed = TRUE;
      }
  
  // NOTE! This function does _not_ emit
  // any signals about selection changed
  return changed;
}

void QVectorView::selectRange(int from, int to, bool invert, bool includeFirst, bool clearSel )
{
  if ( from<0 || to<0 ) return;
  if ( from == to && !includeFirst ) return;
  if ( to < from ) 
    {
      int i = from;
      from = to;
      to = i;
      if ( !includeFirst ) to--;
    } 
  else 
    {
      if ( !includeFirst ) from++;
    }
  
  bool changed = FALSE;
  if ( clearSel ) 
    {
      for(int clearing = 0 ; clearing < item_container->vectorSize() ; clearing ++)
	{
	  if ( isSelected(clearing)) 
	    {
	      setSelected(clearing,FALSE);
	      changed = TRUE;
	    }
	}
    }
  
  if (invert)
    for ( int i = from; i <= to; i++ ) 
      {
	bool sel = !isSelected(i);
	if ( (bool)isSelected(i) != sel && sel) 
	  {
	    setSelected(i,sel);
	    changed = TRUE;
	  }
      }
  else
    for ( int i = from; i <= to; i++ ) 
      if ( !isSelected(i)) 
	{
	  setSelected(i,TRUE);
	  changed = TRUE;
	}
  
  if ( changed ) 
    {
      triggerUpdate();
      emit selectionChanged();
    }
}

/* clears selection from anchor to old, selects from anchor to new, does not emit selectionChanged on change */
bool QVectorView::selectRange( int newItem, int oldItem, int anchorItem )
{
  if ( newItem<0 || oldItem<0 || !anchorItem<0 )
    return FALSE;
  
  int anchorPos = anchorItem>=0 ? itemPos(anchorItem) : 0;
  int oldPos    = oldItem>=0 ? itemPos(oldItem) : 0;
  int newPos    = itemPos(newItem);
  int top=-1, bottom=-1;
  if ( anchorPos > newPos ) 
    {
      top = newItem;
      bottom = anchorItem;
    } 
  else 
    {
      top = anchorItem;
      bottom = newItem;
    }
  
  // removes the parts of the old selection that will no longer be selected
  bool changed = FALSE;
  int topPos    = top>=0 ? itemPos(top) : 0;
  int bottomPos = bottom>=0 ? itemPos(bottom) : 0;
  if ( !(oldPos > topPos && oldPos < bottomPos) ) {
    if ( oldPos < topPos )
      changed = clearRange( oldItem, top );
    else
      changed = clearRange( bottom, oldItem );
  }
  
  // selects the new (not already selected) items
  assert(top<=bottom);
  for (int i = top ; i<=bottom; i++ ) 
    {
      if ( isSelected(i) != d_select ) 
	{
	  setSelected(i,d_select);
	  changed = TRUE;
	}
    }
  return changed;
}

void QVectorView::windowActivationChange( bool oldActive )
{
  if ( oldActive && d_scrollTimer )
    d_scrollTimer->stop();
  if ( palette().active() != palette().inactive() )
    viewport()->update();
  QScrollView::windowActivationChange( oldActive );
}

void QVectorView::hideColumn( int column )
{
  setColumnWidth( column, 0 );
}

void QVectorView::adjustColumn( int col )
{
  d_h->isStretchEnabled( col );
}
