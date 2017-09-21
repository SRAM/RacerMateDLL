/****************************************************************************
** Meta object code from reading C++ file 'udp_client.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../udp_client.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'udp_client.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_RACERMATE__UDPClient_t {
    QByteArrayData data[10];
    char stringdata0[137];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RACERMATE__UDPClient_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RACERMATE__UDPClient_t qt_meta_stringdata_RACERMATE__UDPClient = {
    {
QT_MOC_LITERAL(0, 0, 20), // "RACERMATE::UDPClient"
QT_MOC_LITERAL(1, 21, 27), // "connected_to_trainer_signal"
QT_MOC_LITERAL(2, 49, 0), // ""
QT_MOC_LITERAL(3, 50, 13), // "ctlmsg_signal"
QT_MOC_LITERAL(4, 64, 14), // "unsigned char*"
QT_MOC_LITERAL(5, 79, 11), // "data_signal"
QT_MOC_LITERAL(6, 91, 8), // "QT_DATA*"
QT_MOC_LITERAL(7, 100, 9), // "ss_signal"
QT_MOC_LITERAL(8, 110, 11), // "qt_SS::SSD*"
QT_MOC_LITERAL(9, 122, 14) // "rescale_signal"

    },
    "RACERMATE::UDPClient\0connected_to_trainer_signal\0"
    "\0ctlmsg_signal\0unsigned char*\0data_signal\0"
    "QT_DATA*\0ss_signal\0qt_SS::SSD*\0"
    "rescale_signal"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RACERMATE__UDPClient[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x06 /* Public */,
       3,    3,   44,    2, 0x06 /* Public */,
       5,    2,   51,    2, 0x06 /* Public */,
       7,    2,   56,    2, 0x06 /* Public */,
       9,    2,   61,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    2,    2,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 4, QMetaType::Int,    2,    2,    2,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 6,    2,    2,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 8,    2,    2,
    QMetaType::Int, QMetaType::QString, QMetaType::Int,    2,    2,

       0        // eod
};

void RACERMATE::UDPClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        UDPClient *_t = static_cast<UDPClient *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->connected_to_trainer_signal((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->ctlmsg_signal((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< unsigned char*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: _t->data_signal((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QT_DATA*(*)>(_a[2]))); break;
        case 3: _t->ss_signal((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< qt_SS::SSD*(*)>(_a[2]))); break;
        case 4: { int _r = _t->rescale_signal((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (UDPClient::*_t)(QString , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&UDPClient::connected_to_trainer_signal)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (UDPClient::*_t)(QString , unsigned char * , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&UDPClient::ctlmsg_signal)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (UDPClient::*_t)(QString , QT_DATA * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&UDPClient::data_signal)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (UDPClient::*_t)(QString , qt_SS::SSD * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&UDPClient::ss_signal)) {
                *result = 3;
                return;
            }
        }
        {
            typedef int (UDPClient::*_t)(QString , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&UDPClient::rescale_signal)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject RACERMATE::UDPClient::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_RACERMATE__UDPClient.data,
      qt_meta_data_RACERMATE__UDPClient,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *RACERMATE::UDPClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RACERMATE::UDPClient::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_RACERMATE__UDPClient.stringdata0))
        return static_cast<void*>(const_cast< UDPClient*>(this));
    return QObject::qt_metacast(_clname);
}

int RACERMATE::UDPClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void RACERMATE::UDPClient::connected_to_trainer_signal(QString _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void RACERMATE::UDPClient::ctlmsg_signal(QString _t1, unsigned char * _t2, int _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void RACERMATE::UDPClient::data_signal(QString _t1, QT_DATA * _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void RACERMATE::UDPClient::ss_signal(QString _t1, qt_SS::SSD * _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
int RACERMATE::UDPClient::rescale_signal(QString _t1, int _t2)
{
    int _t0 = int();
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(&_t0)), const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
    return _t0;
}
QT_END_MOC_NAMESPACE
