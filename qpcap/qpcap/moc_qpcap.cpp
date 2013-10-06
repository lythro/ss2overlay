/****************************************************************************
** Meta object code from reading C++ file 'qpcap.h'
**
** Created: Sun Oct 6 21:04:01 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qpcap.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qpcap.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QPcap[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x05,
      28,   21,    6,    6, 0x05,
      68,   54,    6,    6, 0x05,

 // slots: signature, parameters, type, tag, flags
     106,    6,    6,    6, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QPcap[] = {
    "QPcap\0\0packetReady()\0packet\0"
    "packetReady(const uchar*)\0header,packet\0"
    "packetReady(QPcapHeader,const uchar*)\0"
    "dataAvailable()\0"
};

void QPcap::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QPcap *_t = static_cast<QPcap *>(_o);
        switch (_id) {
        case 0: _t->packetReady(); break;
        case 1: _t->packetReady((*reinterpret_cast< const uchar*(*)>(_a[1]))); break;
        case 2: _t->packetReady((*reinterpret_cast< QPcapHeader(*)>(_a[1])),(*reinterpret_cast< const uchar*(*)>(_a[2]))); break;
        case 3: _t->dataAvailable(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QPcap::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QPcap::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QPcap,
      qt_meta_data_QPcap, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QPcap::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QPcap::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QPcap::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QPcap))
        return static_cast<void*>(const_cast< QPcap*>(this));
    return QObject::qt_metacast(_clname);
}

int QPcap::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void QPcap::packetReady()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QPcap::packetReady(const uchar * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QPcap::packetReady(QPcapHeader _t1, const uchar * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
