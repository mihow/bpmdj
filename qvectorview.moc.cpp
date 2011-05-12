/****************************************************************************
** Meta object code from reading C++ file 'qvectorview.h'
**
** Created: Tue Jul 24 21:45:50 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qvectorview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qvectorview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QVectorView[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      30,   10, // methods
       5,  160, // properties
       1,  175, // enums/sets

 // signals: signature, parameters, type, tag, flags
      15,   13,   12,   12, 0x05,
      34,   13,   12,   12, 0x05,
      53,   13,   12,   12, 0x05,
      79,   76,   12,   12, 0x05,
     114,   12,   12,   12, 0x05,
     133,   12,   12,   12, 0x05,
     155,   12,   12,   12, 0x05,
     175,   12,   12,   12, 0x05,
     188,   76,   12,   12, 0x05,
     212,   12,   12,   12, 0x05,
     225,   76,   12,   12, 0x05,
     249,   76,   12,   12, 0x05,
     279,   12,   12,   12, 0x05,
     297,   76,   12,   12, 0x05,
     336,  332,   12,   12, 0x05,
     375,  332,   12,   12, 0x05,
     414,   12,   12,   12, 0x05,
     435,  430,   12,   12, 0x05,
     447,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     460,   12,   12,   12, 0x0a,
     485,  478,   12,   12, 0x0a,
     501,   12,   12,   12, 0x0a,
     521,  517,   12,   12, 0x0a,
     549,  545,   12,   12, 0x0a,
     567,   12,   12,   12, 0x09,
     584,   12,   12,   12, 0x09,
     599,   12,   12,   12, 0x08,
     621,   12,   12,   12, 0x08,
     640,   12,   12,   12, 0x08,
     654,   76,   12,   12, 0x08,

 // properties: name, type, flags
     688,  684, 0x02095001,
     701,  696, 0x01095103,
     719,  684, 0x02095103,
     730,  696, 0x01095103,
     757,  746, 0x0009510b,

 // enums: name, flags, count, data
     746, 0x0,    3,  179,

 // enum data: key, value
     768, uint(QVectorView::NoColumn),
     777, uint(QVectorView::AllColumns),
     788, uint(QVectorView::LastColumn),

       0        // eod
};

static const char qt_meta_stringdata_QVectorView[] = {
    "QVectorView\0\0i\0doubleClicked(int)\0"
    "returnPressed(int)\0ctrlReturnPressed(int)\0"
    ",,\0rightButtonPressed(int,QPoint,int)\0"
    "selectionChanged()\0selectionChanged(int)\0"
    "currentChanged(int)\0clicked(int)\0"
    "clicked(int,QPoint,int)\0pressed(int)\0"
    "pressed(int,QPoint,int)\0"
    "doubleClicked(int,QPoint,int)\0"
    "spacePressed(int)\0rightButtonClicked(int,QPoint,int)\0"
    ",,,\0mouseButtonPressed(int,int,QPoint,int)\0"
    "mouseButtonClicked(int,int,QPoint,int)\0"
    "mouseHover(int)\0item\0onItem(int)\0"
    "onViewport()\0invertSelection()\0select\0"
    "selectAll(bool)\0triggerUpdate()\0x,y\0"
    "setContentsPos(int,int)\0col\0"
    "adjustColumn(int)\0updateContents()\0"
    "doAutoScroll()\0changeSortColumn(int)\0"
    "updateDirtyItems()\0makeVisible()\0"
    "handleSizeChange(int,int,int)\0int\0"
    "columns\0bool\0showSortIndicator\0"
    "itemMargin\0rootIsDecorated\0ResizeMode\0"
    "resizeMode\0NoColumn\0AllColumns\0"
    "LastColumn\0"
};

const QMetaObject QVectorView::staticMetaObject = {
    { &Q3ScrollView::staticMetaObject, qt_meta_stringdata_QVectorView,
      qt_meta_data_QVectorView, 0 }
};

const QMetaObject *QVectorView::metaObject() const
{
    return &staticMetaObject;
}

void *QVectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QVectorView))
	return static_cast<void*>(const_cast< QVectorView*>(this));
    return Q3ScrollView::qt_metacast(_clname);
}

int QVectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3ScrollView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: doubleClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: returnPressed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: ctrlReturnPressed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: rightButtonPressed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 4: selectionChanged(); break;
        case 5: selectionChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: clicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: clicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 9: pressed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: pressed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 11: doubleClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 12: spacePressed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: rightButtonClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 14: mouseButtonPressed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QPoint(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 15: mouseButtonClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QPoint(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 16: mouseHover((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: onItem((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: onViewport(); break;
        case 19: invertSelection(); break;
        case 20: selectAll((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 21: triggerUpdate(); break;
        case 22: setContentsPos((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 23: adjustColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 24: updateContents(); break;
        case 25: doAutoScroll(); break;
        case 26: changeSortColumn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 27: updateDirtyItems(); break;
        case 28: makeVisible(); break;
        case 29: handleSizeChange((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        }
        _id -= 30;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = columns(); break;
        case 1: *reinterpret_cast< bool*>(_v) = showSortIndicator(); break;
        case 2: *reinterpret_cast< int*>(_v) = itemMargin(); break;
        case 3: *reinterpret_cast< bool*>(_v) = rootIsDecorated(); break;
        case 4: *reinterpret_cast< ResizeMode*>(_v) = resizeMode(); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 1: setShowSortIndicator(*reinterpret_cast< bool*>(_v)); break;
        case 2: setItemMargin(*reinterpret_cast< int*>(_v)); break;
        case 3: setRootIsDecorated(*reinterpret_cast< bool*>(_v)); break;
        case 4: setResizeMode(*reinterpret_cast< ResizeMode*>(_v)); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QVectorView::doubleClicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QVectorView::returnPressed(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QVectorView::ctrlReturnPressed(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QVectorView::rightButtonPressed(int _t1, const QPoint & _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QVectorView::selectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void QVectorView::selectionChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QVectorView::currentChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QVectorView::clicked(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void QVectorView::clicked(int _t1, const QPoint & _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void QVectorView::pressed(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void QVectorView::pressed(int _t1, const QPoint & _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void QVectorView::doubleClicked(int _t1, const QPoint & _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void QVectorView::spacePressed(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void QVectorView::rightButtonClicked(int _t1, const QPoint & _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void QVectorView::mouseButtonPressed(int _t1, int _t2, const QPoint & _t3, int _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void QVectorView::mouseButtonClicked(int _t1, int _t2, const QPoint & _t3, int _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void QVectorView::mouseHover(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void QVectorView::onItem(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}

// SIGNAL 18
void QVectorView::onViewport()
{
    QMetaObject::activate(this, &staticMetaObject, 18, 0);
}
