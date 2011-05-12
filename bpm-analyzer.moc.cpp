/****************************************************************************
** Meta object code from reading C++ file 'bpm-analyzer.h'
**
** Created: Thu May 12 18:30:54 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "bpm-analyzer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bpm-analyzer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BpmAnalyzerDialog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x0a,
      37,   18,   18,   18, 0x0a,
      46,   18,   18,   18, 0x0a,
      52,   18,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_BpmAnalyzerDialog[] = {
    "BpmAnalyzerDialog\0\0startStopButton()\0"
    "finish()\0tap()\0reset()\0"
};

const QMetaObject BpmAnalyzerDialog::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_BpmAnalyzerDialog,
      qt_meta_data_BpmAnalyzerDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BpmAnalyzerDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BpmAnalyzerDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BpmAnalyzerDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BpmAnalyzerDialog))
        return static_cast<void*>(const_cast< BpmAnalyzerDialog*>(this));
    if (!strcmp(_clname, "Ui_CountDialog"))
        return static_cast< Ui_CountDialog*>(const_cast< BpmAnalyzerDialog*>(this));
    if (!strcmp(_clname, "Analyzer"))
        return static_cast< Analyzer*>(const_cast< BpmAnalyzerDialog*>(this));
    return QWidget::qt_metacast(_clname);
}

int BpmAnalyzerDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: startStopButton(); break;
        case 1: finish(); break;
        case 2: tap(); break;
        case 3: reset(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
