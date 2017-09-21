/****************************************************************************
** Meta object code from reading C++ file 'ctsrv.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ctsrv.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ctsrv.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_RACERMATE__Ctsrv_t {
    QByteArrayData data[21];
    char stringdata0[254];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RACERMATE__Ctsrv_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RACERMATE__Ctsrv_t qt_meta_stringdata_RACERMATE__Ctsrv = {
    {
QT_MOC_LITERAL(0, 0, 16), // "RACERMATE::Ctsrv"
QT_MOC_LITERAL(1, 17, 9), // "ss_signal"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 22), // "RACERMATE::qt_SS::SSD*"
QT_MOC_LITERAL(4, 51, 11), // "data_signal"
QT_MOC_LITERAL(5, 63, 19), // "RACERMATE::QT_DATA*"
QT_MOC_LITERAL(6, 83, 14), // "rescale_signal"
QT_MOC_LITERAL(7, 98, 27), // "connected_to_trainer_signal"
QT_MOC_LITERAL(8, 126, 13), // "handleResults"
QT_MOC_LITERAL(9, 140, 1), // "s"
QT_MOC_LITERAL(10, 142, 25), // "connected_to_trainer_slot"
QT_MOC_LITERAL(11, 168, 7), // "_udpkey"
QT_MOC_LITERAL(12, 176, 9), // "data_slot"
QT_MOC_LITERAL(13, 186, 4), // "_key"
QT_MOC_LITERAL(14, 191, 8), // "QT_DATA*"
QT_MOC_LITERAL(15, 200, 5), // "_data"
QT_MOC_LITERAL(16, 206, 7), // "ss_slot"
QT_MOC_LITERAL(17, 214, 11), // "qt_SS::SSD*"
QT_MOC_LITERAL(18, 226, 4), // "_ssd"
QT_MOC_LITERAL(19, 231, 12), // "rescale_slot"
QT_MOC_LITERAL(20, 244, 9) // "_maxforce"

    },
    "RACERMATE::Ctsrv\0ss_signal\0\0"
    "RACERMATE::qt_SS::SSD*\0data_signal\0"
    "RACERMATE::QT_DATA*\0rescale_signal\0"
    "connected_to_trainer_signal\0handleResults\0"
    "s\0connected_to_trainer_slot\0_udpkey\0"
    "data_slot\0_key\0QT_DATA*\0_data\0ss_slot\0"
    "qt_SS::SSD*\0_ssd\0rescale_slot\0_maxforce"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RACERMATE__Ctsrv[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   59,    2, 0x06 /* Public */,
       4,    2,   64,    2, 0x06 /* Public */,
       6,    2,   69,    2, 0x06 /* Public */,
       7,    2,   74,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   79,    2, 0x08 /* Private */,
      10,    2,   82,    2, 0x08 /* Private */,
      12,    2,   87,    2, 0x08 /* Private */,
      16,    2,   92,    2, 0x08 /* Private */,
      19,    2,   97,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, 0x80000000 | 3,    2,    2,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 5,    2,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    2,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   11,    2,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 14,   13,   15,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 17,   13,   18,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   13,   20,

       0        // eod
};

void RACERMATE::Ctsrv::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Ctsrv *_t = static_cast<Ctsrv *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ss_signal((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< RACERMATE::qt_SS::SSD*(*)>(_a[2]))); break;
        case 1: _t->data_signal((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< RACERMATE::QT_DATA*(*)>(_a[2]))); break;
        case 2: _t->rescale_signal((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->connected_to_trainer_signal((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->handleResults((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->connected_to_trainer_slot((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 6: _t->data_slot((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QT_DATA*(*)>(_a[2]))); break;
        case 7: _t->ss_slot((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< qt_SS::SSD*(*)>(_a[2]))); break;
        case 8: _t->rescale_slot((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Ctsrv::*_t)(QString , RACERMATE::qt_SS::SSD * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Ctsrv::ss_signal)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (Ctsrv::*_t)(QString , RACERMATE::QT_DATA * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Ctsrv::data_signal)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (Ctsrv::*_t)(QString , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Ctsrv::rescale_signal)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (Ctsrv::*_t)(QString , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Ctsrv::connected_to_trainer_signal)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject RACERMATE::Ctsrv::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_RACERMATE__Ctsrv.data,
      qt_meta_data_RACERMATE__Ctsrv,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *RACERMATE::Ctsrv::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RACERMATE::Ctsrv::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_RACERMATE__Ctsrv.stringdata0))
        return static_cast<void*>(const_cast< Ctsrv*>(this));
    return QObject::qt_metacast(_clname);
}

int RACERMATE::Ctsrv::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void RACERMATE::Ctsrv::ss_signal(QString _t1, RACERMATE::qt_SS::SSD * _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void RACERMATE::Ctsrv::data_signal(QString _t1, RACERMATE::QT_DATA * _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void RACERMATE::Ctsrv::rescale_signal(QString _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void RACERMATE::Ctsrv::connected_to_trainer_signal(QString _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
