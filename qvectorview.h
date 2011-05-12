/****
 BpmDj v3.8: Free Dj Tools
 Copyright (C) 2001-2009 Werner Van Belle

 http://bpmdj.yellowcouch.org/

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
#ifndef __loaded__qvectorview_h__
#define __loaded__qvectorview_h__
using namespace std;
#line 1 "qvectorview.h++"
// A QVectorView suitable for BpmDj.
// This is a stripdown from trolltechs Qt3 QListView class. 
#include <Qt3Support/q3ptrvector.h>
#include <Qt3Support/q3scrollview.h>
#include <vector>

class QFont;
class Q3Header;
class QVectorView;

class QVectorViewData
{
 public:
  int ownHeight;
  int height() const
  {
    return ownHeight;
  }
  QVectorViewData( );
  virtual void setup(QVectorView *v);
  virtual ~QVectorViewData();
  virtual void setSelected( int i, bool ) = 0;
  virtual bool isSelected(int row) const = 0;
  virtual int  vectorSize() const = 0;
  virtual QString text( int row, int col) const = 0;
  virtual void sort(int col, bool ascending) = 0;
  virtual void paintCell( QVectorView*, int number, QPainter *, const QColorGroup & cg, int column, int width, int alignment );
 private:
  friend class QVectorView;
};

class ViewColumnInfo;
class ItemColumnInfo;

class QVectorView: public Q3ScrollView
{
  friend class QVectorViewData;
  Q_OBJECT ;
  Q_ENUMS( ResizeMode RenameAction ) ;
  Q_PROPERTY( int columns READ columns ) ;
  Q_PROPERTY( bool showSortIndicator READ showSortIndicator WRITE setShowSortIndicator ) ;
  Q_PROPERTY( int itemMargin READ itemMargin WRITE setItemMargin ) ;
  Q_PROPERTY( bool rootIsDecorated READ rootIsDecorated WRITE setRootIsDecorated ) ;
  Q_PROPERTY( ResizeMode resizeMode READ resizeMode WRITE setResizeMode ) ;
  QVectorViewData *item_container;
  int totalHeight;
 public:
  // Constructor
  QVectorView( QWidget* parent, QVectorViewData * container, Qt::WindowFlags f = 0 );
  void setupItemContainer();
  void vectorChanged();
 signals:
  void doubleClicked( int i );
  void returnPressed( int i );
  void ctrlReturnPressed( int i);
  void rightButtonPressed( int, const QPoint&, int );
 public:
  ~QVectorView();
  Q3Header * header() const;
  virtual int addColumn( const QString &label, int size = -1);
  virtual void setColumnText( int column, const QString &label );
  //  virtual void setColumnText( int column, const QIconSet& iconset, const QString &label );
  QString columnText( int column ) const;
  virtual void setColumnWidth( int column, int width );
  int columnWidth( int column ) const;
  enum WidthMode { Manual, Maximum };
  virtual void setColumnWidthMode( int column, WidthMode );
  WidthMode columnWidthMode( int column ) const;
  int columns() const;
  void setSectionPos( int column, int index );
  void show();
  int itemAt( const QPoint & screenPos );
  QRect itemRect( int i ) const;
  int itemPos( int i );
  void ensureItemVisible( int i );
  void repaintItem( int i );
  virtual void clearSelection();
  virtual void setSelected( int i, bool );
  void setSelectionAnchor( int );
  bool isSelected( int ) const;
  virtual void setCurrentItem( int );
  int currentItem() const;
  int firstChild() const;
  int lastItem() const;
  virtual void setItemMargin( int );
  int itemMargin() const;
  virtual void setRootIsDecorated( bool );
  bool rootIsDecorated() const;
  virtual void setSorting( int column, bool ascending = TRUE );
  void setSortColumn( int column );
  void setSortOrder( Qt::SortOrder order );
  virtual void setFont( const QFont & );
  virtual void setPalette( const QPalette & );
  bool eventFilter( QObject * o, QEvent * );
  QSize sizeHint() const;
  QSize minimumSizeHint() const;
  virtual void setShowSortIndicator( bool show );
  bool showSortIndicator() const;
  enum ResizeMode { NoColumn, AllColumns, LastColumn };
  virtual void setResizeMode( ResizeMode m );
  ResizeMode resizeMode() const;
  void hideColumn( int column );
 public slots:
  virtual void invertSelection();
  virtual void selectAll( bool select );
  void triggerUpdate();
  void setContentsPos( int x, int y );
  void adjustColumn( int col );
 signals:
  void selectionChanged();
  void selectionChanged( int );
  void currentChanged( int );
  void clicked( int );
  void clicked( int, const QPoint &, int );
  void pressed( int );
  void pressed( int, const QPoint &, int );
  void doubleClicked( int, const QPoint&, int );
  void spacePressed( int );
  void rightButtonClicked( int, const QPoint&, int );
  void mouseButtonPressed( int, int, const QPoint& , int );
  void mouseButtonClicked( int, int,  const QPoint&, int );
  void mouseHover(int);
  void onItem( int item );
  void onViewport();
 protected:
  void contentsMousePressEvent( QMouseEvent * e );
  void contentsMouseReleaseEvent( QMouseEvent * e );
  void contentsMouseMoveEvent( QMouseEvent * e );
  void contentsMouseDoubleClickEvent( QMouseEvent * e );
  void focusInEvent( QFocusEvent * e );
  void focusOutEvent( QFocusEvent * e );
  void keyPressEvent( QKeyEvent *e );
  void resizeEvent( QResizeEvent *e );
  void viewportResizeEvent( QResizeEvent *e );
  void showEvent( QShowEvent * );
  void drawContentsOffset( QPainter *, int ox, int oy,int cx, int cy, int cw, int ch );
  virtual void paintEmptyArea( QPainter *, const QRect & );
  void styleChange( QStyle& );
  void windowActivationChange( bool );
 protected slots:
  void updateContents();
  void doAutoScroll();
 private slots:
  void changeSortColumn( int );
  void updateDirtyItems();
  void makeVisible();
  void handleSizeChange( int, int, int );
 private:
  void contentsMousePressEventEx( QMouseEvent * e );
  void contentsMouseReleaseEventEx( QMouseEvent * e );
  void init();
  void updateGeometries();
  void buildDrawableList();
  void reconfigureItems();
  void widthChanged(int c);
  void handleItemChange( int old, bool shift, bool control );
  void selectRange( int from, int to, bool invert, bool includeFirst, bool clearSel = FALSE );
  bool selectRange( int newItem, int oldItem, int anchorItem );
  bool clearRange( int from, int to, bool includeFirst = TRUE );
  void doAutoScroll( const QPoint &cursorPos );
 private:	// Disabled copy constructor and operator
  vector<int> dirty_items; // items that need repainting
  int bottom_line_number;       // the lowest linenumber to on screen (the highest number)
  int top_line_number;          // the highest linenumber to on screen (the lowest number)
 public:
  void get_visible_range(int &lo, int &hi) {lo=top_line_number; hi=bottom_line_number;};
 private:
  ViewColumnInfo * d_vci;
  Q3Header * d_h;
  int d_margin;
  int d_focusItem, d_highlighted, d_oldFocusItem;
  QTimer * d_timer;
  QTimer * d_dirtyItemTimer;
  QTimer * d_visibleTimer;
  int d_topPixel;
  int d_bottomPixel;
  struct Column 
    { QVectorView::WidthMode wmode; };
  Q3PtrVector<Column> d_column;
  int d_fontMetricsHeight;
  int d_minLeftBearing, d_minRightBearing;
  int d_ellipsisWidth;
  int d_pressedItem, d_selectAnchor;
  QTimer *d_scrollTimer;
  bool d_sortIndicator		:1;
  // whether to select or deselect during this mouse press.
  bool d_select			:1;
  // TRUE if the widget should take notice of mouseReleaseEvent
  bool d_buttonDown		:1;
  // TRUE if the widget should ignore a double-click
  bool d_ignoreDoubleClick	:1;
  bool d_clearing		:1;
  bool d_pressedSelected	:1;
  bool d_pressedEmptyArea 	:1;
  bool d_fullRepaintOnComlumnChange:1;
  bool d_updateHeader		:1;
  bool d_inMenuMode :1;
  int  d_pressedColumn;
  ResizeMode d_resizeMode;
};
#endif // __loaded__qvectorview_h__
