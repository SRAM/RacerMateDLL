/****************************************************************************
** Meta object code from reading C++ file 'worker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../worker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'worker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_RACERMATE__Worker_t {
    QByteArrayData data[21];
    char stringdata0[285];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RACERMATE__Worker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RACERMATE__Worker_t qt_meta_stringdata_RACERMATE__Worker = {
    {
QT_MOC_LITERAL(0, 0, 17), // "RACERMATE::Worker"
QT_MOC_LITERAL(1, 18, 8), // "finished"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 11), // "resultReady"
QT_MOC_LITERAL(4, 40, 6), // "result"
QT_MOC_LITERAL(5, 47, 27), // "connected_to_trainer_signal"
QT_MOC_LITERAL(6, 75, 11), // "data_signal"
QT_MOC_LITERAL(7, 87, 19), // "RACERMATE::QT_DATA*"
QT_MOC_LITERAL(8, 107, 9), // "ss_signal"
QT_MOC_LITERAL(9, 117, 22), // "RACERMATE::qt_SS::SSD*"
QT_MOC_LITERAL(10, 140, 14), // "rescale_signal"
QT_MOC_LITERAL(11, 155, 5), // "start"
QT_MOC_LITERAL(12, 161, 4), // "stop"
QT_MOC_LITERAL(13, 166, 7), // "timeout"
QT_MOC_LITERAL(14, 174, 12), // "disconnected"
QT_MOC_LITERAL(15, 187, 9), // "hostFound"
QT_MOC_LITERAL(16, 197, 23), // "processPendingDatagrams"
QT_MOC_LITERAL(17, 221, 10), // "bc_timeout"
QT_MOC_LITERAL(18, 232, 25), // "connected_to_trainer_slot"
QT_MOC_LITERAL(19, 258, 11), // "ctlmsg_slot"
QT_MOC_LITERAL(20, 270, 14) // "unsigned char*"

    },
    "RACERMATE::Worker\0finished\0\0resultReady\0"
    "result\0connected_to_trainer_signal\0"
    "data_signal\0RACERMATE::QT_DATA*\0"
    "ss_signal\0RACERMATE::qt_SS::SSD*\0"
    "rescale_signal\0start\0stop\0timeout\0"
    "disconnected\0hostFound\0processPendingDatagrams\0"
    "bc_timeout\0connected_to_trainer_slot\0"
    "ctlmsg_slot\0unsigned char*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RACERMATE__Worker[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x06 /* Public */,
       3,    1,   90,    2, 0x06 /* Public */,
       5,    2,   93,    2, 0x06 /* Public */,
       6,    2,   98,    2, 0x06 /* Public */,
       8,    2,  103,    2, 0x06 /* Public */,
      10,    2,  108,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    0,  113,    2, 0x0a /* Public */,
      12,    0,  114,    2, 0x0a /* Public */,
      13,    0,  115,    2, 0x08 /* Private */,
      14,    0,  116,    2, 0x08 /* Private */,
      15,    0,  117,    2, 0x08 /* Private */,
      16,    0,  118,    2, 0x08 /* Private */,
      17,    0,  119,    2, 0x08 /* Private */,
      18,    2,  120,    2, 0x08 /* Private */,
      19,    3,  125,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    2,    2,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 7,    2,    2,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 9,    2,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    2,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    2,    2,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 20, QMetaType::Int,    2,    2,    2,

       0        // eod
};

void RACERMATE::Worker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Worker *_t = static_cast<Worker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->finished(); break;
        case 1: _t->resultReady((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->connected_to_trainer_signal((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->data_signal((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< RACERMATE::QT_DATA*(*)>(_a[2]))); break;
        case 4: _t->ss_signal((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< RACERMATE::qt_SS::SSD*(*)>(_a[2]))); break;
        case 5: _t->rescale_signal((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: _t->start(); break;
        case 7: _t->stop(); break;
        case 8: _t->timeout(); break;
        case 9: _t->disconnected(); break;
        case 10: _t->hostFound(); break;
        case 11: _t->processPendingDatagrams(); break;
        case 12: _t->bc_timeout(); break;
        case 13: _t->connected_to_trainer_slot((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 14: _t->ctlmsg_slot((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< unsigned char*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Worker::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Worker::finished)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (Worker::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Worker::resultReady)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (Worker::*_t)(QString , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Worker::connected_to_trainer_signal)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (Worker::*_t)(QString , RACERMATE::QT_DATA * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Worker::data_signal)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (Worker::*_t)(QString , RACERMATE::qt_SS::SSD * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Worker::ss_signal)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (Worker::*_t)(QString , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Worker::rescale_signal)) {
                *result = 5;
                return;
            }
        }
    }
}

const QMetaObject RACERMATE::Worker::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_RACERMATE__Worker.data,
      qt_meta_data_RACERMATE__Worker,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *RACERMATE::Worker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RACERMATE::Worker::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_RACERMATE__Worker.stringdata0))
        return static_cast<void*>(const_cast< Worker*>(this));
    return QObject::qt_metacast(_clname);
}

int RACERMATE::Worker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void RACERMATE::Worker::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void RACERMATE::Worker::resultReady(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void RACERMATE::Worker::connected_to_trainer_signal(QString _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void RACERMATE::Worker::data_signal(QString _t1, RACERMATE::QT_DATA * _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void RACERMATE::Worker::ss_signal(QString _t1, RACERMATE::qt_SS::SSD * _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void RACERMATE::Worker::rescale_signal(QString _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
