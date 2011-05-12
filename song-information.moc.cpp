/****************************************************************************
** Meta object code from reading C++ file 'song-information.h'
**
** Created: Thu May 12 18:27:25 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "song-information.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'song-information.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SongInformation[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SongInformation[] = {
    "SongInformation\0\0on_save_clicked()\0"
};

const QMetaObject SongInformation::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SongInformation,
      qt_meta_data_SongInformation, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SongInformation::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SongInformation::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SongInformation::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SongInformation))
        return static_cast<void*>(const_cast< SongInformation*>(this));
    if (!strcmp(_clname, "Ui::SongInformation"))
        return static_cast< Ui::SongInformation*>(const_cast< SongInformation*>(this));
    return QWidget::qt_metacast(_clname);
}

int SongInformation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_save_clicked(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
