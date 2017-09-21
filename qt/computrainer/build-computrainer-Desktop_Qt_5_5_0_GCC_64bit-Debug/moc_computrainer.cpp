/****************************************************************************
** Meta object code from reading C++ file 'computrainer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../computrainer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'computrainer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_RACERMATE__Computrainer_t {
    QByteArrayData data[23];
    char stringdata0[381];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RACERMATE__Computrainer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RACERMATE__Computrainer_t qt_meta_stringdata_RACERMATE__Computrainer = {
    {
QT_MOC_LITERAL(0, 0, 23), // "RACERMATE::Computrainer"
QT_MOC_LITERAL(1, 24, 26), // "connected_to_server_signal"
QT_MOC_LITERAL(2, 51, 0), // ""
QT_MOC_LITERAL(3, 52, 27), // "connected_to_trainer_signal"
QT_MOC_LITERAL(4, 80, 20), // "spinscan_data_signal"
QT_MOC_LITERAL(5, 101, 11), // "qt_SS::SSD*"
QT_MOC_LITERAL(6, 113, 11), // "data_signal"
QT_MOC_LITERAL(7, 125, 8), // "QT_DATA*"
QT_MOC_LITERAL(8, 134, 14), // "rescale_signal"
QT_MOC_LITERAL(9, 149, 14), // "connected_slot"
QT_MOC_LITERAL(10, 164, 17), // "disconnected_slot"
QT_MOC_LITERAL(11, 182, 17), // "bytesWritten_slot"
QT_MOC_LITERAL(12, 200, 5), // "bytes"
QT_MOC_LITERAL(13, 206, 14), // "readyRead_slot"
QT_MOC_LITERAL(14, 221, 17), // "state_change_slot"
QT_MOC_LITERAL(15, 239, 28), // "QAbstractSocket::SocketState"
QT_MOC_LITERAL(16, 268, 13), // "_socket_state"
QT_MOC_LITERAL(17, 282, 10), // "timer_slot"
QT_MOC_LITERAL(18, 293, 10), // "error_slot"
QT_MOC_LITERAL(19, 304, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(20, 333, 12), // "socketEerror"
QT_MOC_LITERAL(21, 346, 17), // "gradechanged_slot"
QT_MOC_LITERAL(22, 364, 16) // "windchanged_slot"

    },
    "RACERMATE::Computrainer\0"
    "connected_to_server_signal\0\0"
    "connected_to_trainer_signal\0"
    "spinscan_data_signal\0qt_SS::SSD*\0"
    "data_signal\0QT_DATA*\0rescale_signal\0"
    "connected_slot\0disconnected_slot\0"
    "bytesWritten_slot\0bytes\0readyRead_slot\0"
    "state_change_slot\0QAbstractSocket::SocketState\0"
    "_socket_state\0timer_slot\0error_slot\0"
    "QAbstractSocket::SocketError\0socketEerror\0"
    "gradechanged_slot\0windchanged_slot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RACERMATE__Computrainer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   84,    2, 0x06 /* Public */,
       3,    2,   87,    2, 0x06 /* Public */,
       4,    2,   92,    2, 0x06 /* Public */,
       6,    2,   97,    2, 0x06 /* Public */,
       8,    2,  102,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,  107,    2, 0x08 /* Private */,
      10,    0,  108,    2, 0x08 /* Private */,
      11,    1,  109,    2, 0x08 /* Private */,
      13,    0,  112,    2, 0x08 /* Private */,
      14,    1,  113,    2, 0x08 /* Private */,
      17,    0,  116,    2, 0x08 /* Private */,
      18,    1,  117,    2, 0x08 /* Private */,
      21,    1,  120,    2, 0x0a /* Public */,
      22,    1,  123,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    2,    2,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 5,    2,    2,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 7,    2,    2,
    QMetaType::Int, QMetaType::Int, QMetaType::Int,    2,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong,   12,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 19,   20,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Float,    2,

       0        // eod
};

void RACERMATE::Computrainer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Computrainer *_t = static_cast<Computrainer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->connected_to_server_signal((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->connected_to_trainer_signal((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->spinscan_data_signal((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< qt_SS::SSD*(*)>(_a[2]))); break;
        case 3: _t->data_signal((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QT_DATA*(*)>(_a[2]))); break;
        case 4: { int _r = _t->rescale_signal((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 5: _t->connected_slot(); break;
        case 6: _t->disconnected_slot(); break;
        case 7: _t->bytesWritten_slot((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 8: _t->readyRead_slot(); break;
        case 9: _t->state_change_slot((*reinterpret_cast< QAbstractSocket::SocketState(*)>(_a[1]))); break;
        case 10: _t->timer_slot(); break;
        case 11: _t->error_slot((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 12: _t->gradechanged_slot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->windchanged_slot((*reinterpret_cast< float(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketState >(); break;
            }
            break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Computrainer::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Computrainer::connected_to_server_signal)) {
                *result = 0;
            }
        }
        {
            typedef void (Computrainer::*_t)(int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Computrainer::connected_to_trainer_signal)) {
                *result = 1;
            }
        }
        {
            typedef void (Computrainer::*_t)(int , qt_SS::SSD * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Computrainer::spinscan_data_signal)) {
                *result = 2;
            }
        }
        {
            typedef void (Computrainer::*_t)(int , QT_DATA * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Computrainer::data_signal)) {
                *result = 3;
            }
        }
        {
            typedef int (Computrainer::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Computrainer::rescale_signal)) {
                *result = 4;
            }
        }
    }
}

const QMetaObject RACERMATE::Computrainer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_RACERMATE__Computrainer.data,
      qt_meta_data_RACERMATE__Computrainer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *RACERMATE::Computrainer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RACERMATE::Computrainer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_RACERMATE__Computrainer.stringdata0))
        return static_cast<void*>(const_cast< Computrainer*>(this));
    return QObject::qt_metacast(_clname);
}

int RACERMATE::Computrainer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void RACERMATE::Computrainer::connected_to_server_signal(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void RACERMATE::Computrainer::connected_to_trainer_signal(int _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void RACERMATE::Computrainer::spinscan_data_signal(int _t1, qt_SS::SSD * _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void RACERMATE::Computrainer::data_signal(int _t1, QT_DATA * _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
int RACERMATE::Computrainer::rescale_signal(int _t1, int _t2)
{
    int _t0 = int();
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(&_t0)), const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
    return _t0;
}
QT_END_MOC_NAMESPACE
