/****************************************************************************
** Meta object code from reading C++ file 'music-scanner.h'
**
** Created: Tue Jul 24 21:45:58 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "music-scanner.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'music-scanner.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_MusicScanner[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_MusicScanner[] = {
    "MusicScanner\0"
};

const QMetaObject MusicScanner::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MusicScanner,
      qt_meta_data_MusicScanner, 0 }
};

const QMetaObject *MusicScanner::metaObject() const
{
    return &staticMetaObject;
}

void *MusicScanner::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MusicScanner))
	return static_cast<void*>(const_cast< MusicScanner*>(this));
    if (!strcmp(_clname, "DirectoryScanner"))
	return static_cast< DirectoryScanner*>(const_cast< MusicScanner*>(this));
    if (!strcmp(_clname, "Ui::ImportProgress"))
	return static_cast< Ui::ImportProgress*>(const_cast< MusicScanner*>(this));
    return QDialog::qt_metacast(_clname);
}

int MusicScanner::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
