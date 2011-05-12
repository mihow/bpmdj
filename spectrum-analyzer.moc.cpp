/****************************************************************************
** Meta object code from reading C++ file 'spectrum-analyzer.h'
**
** Created: Thu May 12 14:46:13 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "spectrum-analyzer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spectrum-analyzer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SpectrumAnalyzer[] = {

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
      18,   17,   17,   17, 0x0a,
      41,   17,   17,   17, 0x0a,
      65,   17,   17,   17, 0x0a,
      75,   17,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SpectrumAnalyzer[] = {
    "SpectrumAnalyzer\0\0fetchSpectrum_normal()\0"
    "fetchSpectrum_wavelet()\0analyze()\0"
    "on_fetchSpectrum_clicked()\0"
};

const QMetaObject SpectrumAnalyzer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SpectrumAnalyzer,
      qt_meta_data_SpectrumAnalyzer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SpectrumAnalyzer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SpectrumAnalyzer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SpectrumAnalyzer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SpectrumAnalyzer))
        return static_cast<void*>(const_cast< SpectrumAnalyzer*>(this));
    if (!strcmp(_clname, "Ui::SpectrumAnalyzerForm"))
        return static_cast< Ui::SpectrumAnalyzerForm*>(const_cast< SpectrumAnalyzer*>(this));
    if (!strcmp(_clname, "Analyzer"))
        return static_cast< Analyzer*>(const_cast< SpectrumAnalyzer*>(this));
    return QWidget::qt_metacast(_clname);
}

int SpectrumAnalyzer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: fetchSpectrum_normal(); break;
        case 1: fetchSpectrum_wavelet(); break;
        case 2: analyze(); break;
        case 3: on_fetchSpectrum_clicked(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
