/****************************************************************************
** Meta object code from reading C++ file 'cube3d.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "cube3d.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cube3d.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Cube3d_t {
    QByteArrayData data[19];
    char stringdata0[147];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Cube3d_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Cube3d_t qt_meta_stringdata_Cube3d = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Cube3d"
QT_MOC_LITERAL(1, 7, 5), // "FlipU"
QT_MOC_LITERAL(2, 13, 0), // ""
QT_MOC_LITERAL(3, 14, 5), // "FlipD"
QT_MOC_LITERAL(4, 20, 5), // "FlipL"
QT_MOC_LITERAL(5, 26, 5), // "FlipR"
QT_MOC_LITERAL(6, 32, 5), // "FlipF"
QT_MOC_LITERAL(7, 38, 5), // "FlipB"
QT_MOC_LITERAL(8, 44, 5), // "Flipu"
QT_MOC_LITERAL(9, 50, 5), // "Flipd"
QT_MOC_LITERAL(10, 56, 5), // "Flipl"
QT_MOC_LITERAL(11, 62, 5), // "Flipr"
QT_MOC_LITERAL(12, 68, 5), // "Flipf"
QT_MOC_LITERAL(13, 74, 5), // "Flipb"
QT_MOC_LITERAL(14, 80, 4), // "flip"
QT_MOC_LITERAL(15, 85, 13), // "FlipForDegbug"
QT_MOC_LITERAL(16, 99, 16), // "CaptureColorStop"
QT_MOC_LITERAL(17, 116, 17), // "CaptureColorStart"
QT_MOC_LITERAL(18, 134, 12) // "setCubeColor"

    },
    "Cube3d\0FlipU\0\0FlipD\0FlipL\0FlipR\0FlipF\0"
    "FlipB\0Flipu\0Flipd\0Flipl\0Flipr\0Flipf\0"
    "Flipb\0flip\0FlipForDegbug\0CaptureColorStop\0"
    "CaptureColorStart\0setCubeColor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Cube3d[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   99,    2, 0x0a /* Public */,
       3,    0,  100,    2, 0x0a /* Public */,
       4,    0,  101,    2, 0x0a /* Public */,
       5,    0,  102,    2, 0x0a /* Public */,
       6,    0,  103,    2, 0x0a /* Public */,
       7,    0,  104,    2, 0x0a /* Public */,
       8,    0,  105,    2, 0x0a /* Public */,
       9,    0,  106,    2, 0x0a /* Public */,
      10,    0,  107,    2, 0x0a /* Public */,
      11,    0,  108,    2, 0x0a /* Public */,
      12,    0,  109,    2, 0x0a /* Public */,
      13,    0,  110,    2, 0x0a /* Public */,
      14,    0,  111,    2, 0x0a /* Public */,
      15,    0,  112,    2, 0x0a /* Public */,
      16,    0,  113,    2, 0x0a /* Public */,
      17,    0,  114,    2, 0x0a /* Public */,
      18,    0,  115,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Cube3d::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Cube3d *_t = static_cast<Cube3d *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->FlipU(); break;
        case 1: _t->FlipD(); break;
        case 2: _t->FlipL(); break;
        case 3: _t->FlipR(); break;
        case 4: _t->FlipF(); break;
        case 5: _t->FlipB(); break;
        case 6: _t->Flipu(); break;
        case 7: _t->Flipd(); break;
        case 8: _t->Flipl(); break;
        case 9: _t->Flipr(); break;
        case 10: _t->Flipf(); break;
        case 11: _t->Flipb(); break;
        case 12: _t->flip(); break;
        case 13: _t->FlipForDegbug(); break;
        case 14: _t->CaptureColorStop(); break;
        case 15: _t->CaptureColorStart(); break;
        case 16: _t->setCubeColor(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Cube3d::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Cube3d.data,
      qt_meta_data_Cube3d,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Cube3d::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Cube3d::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Cube3d.stringdata0))
        return static_cast<void*>(const_cast< Cube3d*>(this));
    return QWidget::qt_metacast(_clname);
}

int Cube3d::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
