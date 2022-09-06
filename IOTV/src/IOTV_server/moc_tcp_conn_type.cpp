/****************************************************************************
** Meta object code from reading C++ file 'tcp_conn_type.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "connection_type/tcp_conn_type.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tcp_conn_type.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TCP_conn_type_t {
    const uint offsetsAndSize[20];
    char stringdata0[165];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_TCP_conn_type_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_TCP_conn_type_t qt_meta_stringdata_TCP_conn_type = {
    {
QT_MOC_LITERAL(0, 13), // "TCP_conn_type"
QT_MOC_LITERAL(14, 17), // "slotNewConnection"
QT_MOC_LITERAL(32, 0), // ""
QT_MOC_LITERAL(33, 22), // "slotSocketDisconnected"
QT_MOC_LITERAL(56, 22), // "slotSocketStateChanged"
QT_MOC_LITERAL(79, 28), // "QAbstractSocket::SocketState"
QT_MOC_LITERAL(108, 11), // "socketState"
QT_MOC_LITERAL(120, 9), // "slotError"
QT_MOC_LITERAL(130, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(159, 5) // "error"

    },
    "TCP_conn_type\0slotNewConnection\0\0"
    "slotSocketDisconnected\0slotSocketStateChanged\0"
    "QAbstractSocket::SocketState\0socketState\0"
    "slotError\0QAbstractSocket::SocketError\0"
    "error"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TCP_conn_type[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   38,    2, 0x08,    1 /* Private */,
       3,    0,   39,    2, 0x08,    2 /* Private */,
       4,    1,   40,    2, 0x08,    3 /* Private */,
       7,    1,   43,    2, 0x08,    5 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 8,    9,

       0        // eod
};

void TCP_conn_type::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TCP_conn_type *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->slotNewConnection(); break;
        case 1: _t->slotSocketDisconnected(); break;
        case 2: _t->slotSocketStateChanged((*reinterpret_cast< QAbstractSocket::SocketState(*)>(_a[1]))); break;
        case 3: _t->slotError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketState >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
}

const QMetaObject TCP_conn_type::staticMetaObject = { {
    QMetaObject::SuperData::link<Base_conn_type::staticMetaObject>(),
    qt_meta_stringdata_TCP_conn_type.offsetsAndSize,
    qt_meta_data_TCP_conn_type,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_TCP_conn_type_t
, QtPrivate::TypeAndForceComplete<TCP_conn_type, std::true_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QAbstractSocket::SocketState, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QAbstractSocket::SocketError, std::false_type>


>,
    nullptr
} };


const QMetaObject *TCP_conn_type::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TCP_conn_type::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TCP_conn_type.stringdata0))
        return static_cast<void*>(this);
    return Base_conn_type::qt_metacast(_clname);
}

int TCP_conn_type::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Base_conn_type::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
