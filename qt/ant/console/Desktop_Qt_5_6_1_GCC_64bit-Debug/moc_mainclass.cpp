/****************************************************************************
** Meta object code from reading C++ file 'mainclass.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../mainclass.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainclass.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainClass_t {
    QByteArrayData data[8];
    char stringdata0[74];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainClass_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainClass_t qt_meta_stringdata_MainClass = {
    {
QT_MOC_LITERAL(0, 0, 9), // "MainClass"
QT_MOC_LITERAL(1, 10, 8), // "finished"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 3), // "run"
QT_MOC_LITERAL(4, 24, 14), // "aboutToQuitApp"
QT_MOC_LITERAL(5, 39, 19), // "OnConsoleKeyPressed"
QT_MOC_LITERAL(6, 59, 1), // "c"
QT_MOC_LITERAL(7, 61, 12) // "timeout_slot"

    },
    "MainClass\0finished\0\0run\0aboutToQuitApp\0"
    "OnConsoleKeyPressed\0c\0timeout_slot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainClass[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   40,    2, 0x0a /* Public */,
       4,    0,   41,    2, 0x0a /* Public */,
       5,    1,   42,    2, 0x08 /* Private */,
       7,    0,   45,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Char,    6,
    QMetaType::Void,

       0        // eod
};

void MainClass::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainClass *_t = static_cast<MainClass *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->finished(); break;
        case 1: _t->run(); break;
        case 2: _t->aboutToQuitApp(); break;
        case 3: _t->OnConsoleKeyPressed((*reinterpret_cast< char(*)>(_a[1]))); break;
        case 4: _t->timeout_slot(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MainClass::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainClass::finished)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject MainClass::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MainClass.data,
      qt_meta_data_MainClass,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MainClass::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainClass::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MainClass.stringdata0))
        return static_cast<void*>(const_cast< MainClass*>(this));
    return QObject::qt_metacast(_clname);
}

int MainClass::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void MainClass::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
