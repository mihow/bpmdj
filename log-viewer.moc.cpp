/****************************************************************************
** Meta object code from reading C++ file 'log-viewer.h'
**
** Created: Tue Jul 24 21:47:08 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "log-viewer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'log-viewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_LogViewer[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      13,   11,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_LogViewer[] = {
    "LogViewer\0\0,\0"
    "on_list_currentItemChanged(QListWidgetItem*,QListWidgetItem*)\0"
};

const QMetaObject LogViewer::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_LogViewer,
      qt_meta_data_LogViewer, 0 }
};

const QMetaObject *LogViewer::metaObject() const
{
    return &staticMetaObject;
}

void *LogViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LogViewer))
	return static_cast<void*>(const_cast< LogViewer*>(this));
    if (!strcmp(_clname, "Ui::LogViewer"))
	return static_cast< Ui::LogViewer*>(const_cast< LogViewer*>(this));
    if (!strcmp(_clname, "DirectoryScanner"))
	return static_cast< DirectoryScanner*>(const_cast< LogViewer*>(this));
    return QDialog::qt_metacast(_clname);
}

int LogViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_list_currentItemChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        }
        _id -= 1;
    }
    return _id;
}
