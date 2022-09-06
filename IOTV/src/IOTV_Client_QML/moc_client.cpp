/****************************************************************************
** Meta object code from reading C++ file 'client.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "client.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'client.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Client_t {
    const uint offsetsAndSize[66];
    char stringdata0[450];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_Client_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_Client_t qt_meta_stringdata_Client = {
    {
QT_MOC_LITERAL(0, 6), // "Client"
QT_MOC_LITERAL(7, 15), // "signalConnected"
QT_MOC_LITERAL(23, 0), // ""
QT_MOC_LITERAL(24, 18), // "signalDisconnected"
QT_MOC_LITERAL(43, 14), // "addressChanged"
QT_MOC_LITERAL(58, 11), // "portChanged"
QT_MOC_LITERAL(70, 18), // "countDeviceChanged"
QT_MOC_LITERAL(89, 19), // "onlineDeviceChanged"
QT_MOC_LITERAL(109, 22), // "stateConnectionChanged"
QT_MOC_LITERAL(132, 18), // "autoConnectChanged"
QT_MOC_LITERAL(151, 17), // "signalConnectWait"
QT_MOC_LITERAL(169, 13), // "connectToHost"
QT_MOC_LITERAL(183, 18), // "disconnectFromHost"
QT_MOC_LITERAL(202, 13), // "slotConnected"
QT_MOC_LITERAL(216, 16), // "slotDisconnected"
QT_MOC_LITERAL(233, 14), // "slotReciveData"
QT_MOC_LITERAL(248, 13), // "slotQueryRead"
QT_MOC_LITERAL(262, 14), // "slotQueryState"
QT_MOC_LITERAL(277, 16), // "slotQueryDevList"
QT_MOC_LITERAL(294, 9), // "slotError"
QT_MOC_LITERAL(304, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(333, 5), // "error"
QT_MOC_LITERAL(339, 15), // "slotConnectWait"
QT_MOC_LITERAL(355, 7), // "devList"
QT_MOC_LITERAL(363, 15), // "QList<QObject*>"
QT_MOC_LITERAL(379, 12), // "deviceByName"
QT_MOC_LITERAL(392, 4), // "name"
QT_MOC_LITERAL(397, 4), // "addr"
QT_MOC_LITERAL(402, 4), // "port"
QT_MOC_LITERAL(407, 11), // "totalDevice"
QT_MOC_LITERAL(419, 12), // "onlineDevice"
QT_MOC_LITERAL(432, 5), // "state"
QT_MOC_LITERAL(438, 11) // "autoConnect"

    },
    "Client\0signalConnected\0\0signalDisconnected\0"
    "addressChanged\0portChanged\0"
    "countDeviceChanged\0onlineDeviceChanged\0"
    "stateConnectionChanged\0autoConnectChanged\0"
    "signalConnectWait\0connectToHost\0"
    "disconnectFromHost\0slotConnected\0"
    "slotDisconnected\0slotReciveData\0"
    "slotQueryRead\0slotQueryState\0"
    "slotQueryDevList\0slotError\0"
    "QAbstractSocket::SocketError\0error\0"
    "slotConnectWait\0devList\0QList<QObject*>\0"
    "deviceByName\0name\0addr\0port\0totalDevice\0"
    "onlineDevice\0state\0autoConnect"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Client[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      21,   14, // methods
       6,  169, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  140,    2, 0x06,    7 /* Public */,
       3,    0,  141,    2, 0x06,    8 /* Public */,
       4,    1,  142,    2, 0x06,    9 /* Public */,
       5,    1,  145,    2, 0x06,   11 /* Public */,
       6,    0,  148,    2, 0x06,   13 /* Public */,
       7,    0,  149,    2, 0x06,   14 /* Public */,
       8,    0,  150,    2, 0x06,   15 /* Public */,
       9,    0,  151,    2, 0x06,   16 /* Public */,
      10,    0,  152,    2, 0x06,   17 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      11,    0,  153,    2, 0x0a,   18 /* Public */,
      12,    0,  154,    2, 0x0a,   19 /* Public */,
      13,    0,  155,    2, 0x0a,   20 /* Public */,
      14,    0,  156,    2, 0x0a,   21 /* Public */,
      15,    0,  157,    2, 0x08,   22 /* Private */,
      16,    0,  158,    2, 0x08,   23 /* Private */,
      17,    0,  159,    2, 0x08,   24 /* Private */,
      18,    0,  160,    2, 0x08,   25 /* Private */,
      19,    1,  161,    2, 0x08,   26 /* Private */,
      22,    0,  164,    2, 0x08,   28 /* Private */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      23,    0,  165,    2, 0x02,   29 /* Public */,
      25,    1,  166,    2, 0x02,   30 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::ULongLong,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 20,   21,
    QMetaType::Void,

 // methods: parameters
    0x80000000 | 24,
    QMetaType::QObjectStar, QMetaType::QString,   26,

 // properties: name, type, flags
      27, QMetaType::QString, 0x00015003, uint(2), 0,
      28, QMetaType::UShort, 0x00015103, uint(3), 0,
      29, QMetaType::UShort, 0x00015001, uint(4), 0,
      30, QMetaType::UShort, 0x00015001, uint(5), 0,
      31, QMetaType::Bool, 0x00015001, uint(6), 0,
      32, QMetaType::Bool, 0x00015103, uint(7), 0,

       0        // eod
};

void Client::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Client *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->signalConnected(); break;
        case 1: _t->signalDisconnected(); break;
        case 2: _t->addressChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->portChanged((*reinterpret_cast< quint64(*)>(_a[1]))); break;
        case 4: _t->countDeviceChanged(); break;
        case 5: _t->onlineDeviceChanged(); break;
        case 6: _t->stateConnectionChanged(); break;
        case 7: _t->autoConnectChanged(); break;
        case 8: _t->signalConnectWait(); break;
        case 9: _t->connectToHost(); break;
        case 10: _t->disconnectFromHost(); break;
        case 11: _t->slotConnected(); break;
        case 12: _t->slotDisconnected(); break;
        case 13: _t->slotReciveData(); break;
        case 14: _t->slotQueryRead(); break;
        case 15: _t->slotQueryState(); break;
        case 16: _t->slotQueryDevList(); break;
        case 17: _t->slotError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 18: _t->slotConnectWait(); break;
        case 19: { QList<QObject*> _r = _t->devList();
            if (_a[0]) *reinterpret_cast< QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 20: { QObject* _r = _t->deviceByName((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QObject**>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 17:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Client::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::signalConnected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Client::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::signalDisconnected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Client::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::addressChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Client::*)(quint64 );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::portChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Client::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::countDeviceChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Client::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::onlineDeviceChanged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (Client::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::stateConnectionChanged)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (Client::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::autoConnectChanged)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (Client::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Client::signalConnectWait)) {
                *result = 8;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<Client *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->address(); break;
        case 1: *reinterpret_cast< quint16*>(_v) = _t->port(); break;
        case 2: *reinterpret_cast< quint16*>(_v) = _t->countDevices(); break;
        case 3: *reinterpret_cast< quint16*>(_v) = _t->countDeviceOnline(); break;
        case 4: *reinterpret_cast< bool*>(_v) = _t->stateConnection(); break;
        case 5: *reinterpret_cast< bool*>(_v) = _t->autoConnect(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<Client *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setAddress(*reinterpret_cast< QString*>(_v)); break;
        case 1: _t->setPort(*reinterpret_cast< quint16*>(_v)); break;
        case 5: _t->setAutoConnect(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject Client::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Client.offsetsAndSize,
    qt_meta_data_Client,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_Client_t
, QtPrivate::TypeAndForceComplete<QString, std::true_type>, QtPrivate::TypeAndForceComplete<quint16, std::true_type>, QtPrivate::TypeAndForceComplete<quint16, std::true_type>, QtPrivate::TypeAndForceComplete<quint16, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<bool, std::true_type>, QtPrivate::TypeAndForceComplete<Client, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<quint64, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QAbstractSocket::SocketError, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>
, QtPrivate::TypeAndForceComplete<QList<QObject*>, std::false_type>, QtPrivate::TypeAndForceComplete<QObject *, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>

>,
    nullptr
} };


const QMetaObject *Client::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Client::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Client.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Client::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Client::signalConnected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Client::signalDisconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Client::addressChanged(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Client::portChanged(quint64 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Client::countDeviceChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void Client::onlineDeviceChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void Client::stateConnectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void Client::autoConnectChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void Client::signalConnectWait()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
