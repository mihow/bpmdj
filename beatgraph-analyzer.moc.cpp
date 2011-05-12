/****************************************************************************
** Meta object code from reading C++ file 'beatgraph-analyzer.h'
**
** Created: Thu May 12 18:30:23 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "beatgraph-analyzer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'beatgraph-analyzer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BeatGraphAnalyzer[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,
      33,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      47,   18,   18,   18, 0x0a,
      58,   18,   18,   18, 0x0a,
      71,   18,   18,   18, 0x0a,
      85,   18,   18,   18, 0x0a,
     100,   18,   18,   18, 0x0a,
     111,   18,   18,   18, 0x0a,
     133,   18,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_BeatGraphAnalyzer[] = {
    "BeatGraphAnalyzer\0\0targetTempo()\0"
    "normalTempo()\0activate()\0deactivate()\0"
    "showPattern()\0slantChanged()\0setTempo()\0"
    "changeVisualisation()\0cuesChanged()\0"
};

const QMetaObject BeatGraphAnalyzer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_BeatGraphAnalyzer,
      qt_meta_data_BeatGraphAnalyzer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BeatGraphAnalyzer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BeatGraphAnalyzer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BeatGraphAnalyzer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BeatGraphAnalyzer))
        return static_cast<void*>(const_cast< BeatGraphAnalyzer*>(this));
    if (!strcmp(_clname, "Ui::BeatGraph"))
        return static_cast< Ui::BeatGraph*>(const_cast< BeatGraphAnalyzer*>(this));
    return QWidget::qt_metacast(_clname);
}

int BeatGraphAnalyzer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: targetTempo(); break;
        case 1: normalTempo(); break;
        case 2: activate(); break;
        case 3: deactivate(); break;
        case 4: showPattern(); break;
        case 5: slantChanged(); break;
        case 6: setTempo(); break;
        case 7: changeVisualisation(); break;
        case 8: cuesChanged(); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void BeatGraphAnalyzer::targetTempo()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void BeatGraphAnalyzer::normalTempo()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
