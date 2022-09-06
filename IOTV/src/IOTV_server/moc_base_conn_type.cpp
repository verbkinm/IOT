/****************************************************************************
** Meta object code from reading C++ file 'base_conn_type.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "connection_type/base_conn_type.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'base_conn_type.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Base_conn_type_t {
    const uint offsetsAndSize[12];
    char stringdata0[82];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_Base_conn_type_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_Base_conn_type_t qt_meta_stringdata_Base_conn_type = {
    {
QT_MOC_LITERAL(0, 14), // "Base_conn_type"
QT_MOC_LITERAL(15, 15), // "signalConnected"
QT_MOC_LITERAL(31, 0), // ""
QT_MOC_LITERAL(32, 18), // "signalDisconnected"
QT_MOC_LITERAL(51, 17), // "signalDataRiceved"
QT_MOC_LITERAL(69, 12) // "slotReadData"

    },
    "Base_conn_type\0signalConnected\0\0"
    "signalDisconnected\0signalDataRiceved\0"
    "slotReadData"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Base_conn_type[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   38,    2, 0x06,    1 /* Public */,
       3,    0,   39,    2, 0x06,    2 /* Public */,
       4,    1,   40,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    0,   43,    2, 0x09,    5 /* Protected */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    2,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void Base_conn_type::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Base_conn_type *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->signalConnected(); break;
        case 1: _t->signalDisconnected(); break;
        case 2: _t->signalDataRiceved((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 3: _t->slotReadData(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Base_conn_type::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Base_conn_type::signalConnected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Base_conn_type::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Base_conn_type::signalDisconnected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Base_conn_type::*)(QByteArray );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Base_conn_type::signalDataRiceved)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject Base_conn_type::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Base_conn_type.offsetsAndSize,
    qt_meta_data_Base_conn_type,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_Base_conn_type_t
, QtPrivate::TypeAndForceComplete<Base_conn_type, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QByteArray, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *Base_conn_type::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Base_conn_type::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Base_conn_type.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Base_conn_type::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Base_conn_type::signalConnected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Base_conn_type::signalDisconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Base_conn_type::signalDataRiceved(QByteArray _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
