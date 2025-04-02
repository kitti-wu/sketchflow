/****************************************************************************
** Meta object code from reading C++ file 'tool.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../core/tools/tool.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tool.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSToolENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSToolENDCLASS = QtMocHelpers::stringData(
    "Tool",
    "updateFrame",
    "",
    "toggled",
    "on",
    "pressed",
    "EventInfo",
    "info",
    "moved",
    "released",
    "doublepressed",
    "wheel",
    "WheelEventInfo",
    "keyPressed",
    "QKeyEvent*",
    "event",
    "keyReleased",
    "draw",
    "QPainter&",
    "painter",
    "VectorKeyFrame*",
    "key",
    "contextMenu",
    "QMenu&",
    "ToolType",
    "Pen",
    "DrawEndKeyframe",
    "Eraser",
    "Hand",
    "Select",
    "RigidDeform",
    "Warp",
    "StrokeDeform",
    "RegistrationLasso",
    "Scribble",
    "Traj",
    "DrawTraj",
    "TrajTangent",
    "Lasso",
    "Correspondence",
    "FillGrid",
    "DirectMatching",
    "MoveFrames",
    "Halves",
    "SimplifySpacing",
    "NoTool"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSToolENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       1,  113, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   80,    2, 0x06,    2 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       3,    1,   81,    2, 0x0a,    3 /* Public */,
       5,    1,   84,    2, 0x0a,    5 /* Public */,
       8,    1,   87,    2, 0x0a,    7 /* Public */,
       9,    1,   90,    2, 0x0a,    9 /* Public */,
      10,    1,   93,    2, 0x0a,   11 /* Public */,
      11,    1,   96,    2, 0x0a,   13 /* Public */,
      13,    1,   99,    2, 0x0a,   15 /* Public */,
      16,    1,  102,    2, 0x0a,   17 /* Public */,
      17,    2,  105,    2, 0x0a,   19 /* Public */,
      22,    1,  110,    2, 0x0a,   22 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 12,    7,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void, 0x80000000 | 18, 0x80000000 | 20,   19,   21,
    QMetaType::Void, 0x80000000 | 23,   22,

 // enums: name, alias, flags, count, data
      24,   24, 0x0,   21,  118,

 // enum data: key, value
      25, uint(Tool::Pen),
      26, uint(Tool::DrawEndKeyframe),
      27, uint(Tool::Eraser),
      28, uint(Tool::Hand),
      29, uint(Tool::Select),
      30, uint(Tool::RigidDeform),
      31, uint(Tool::Warp),
      32, uint(Tool::StrokeDeform),
      33, uint(Tool::RegistrationLasso),
      34, uint(Tool::Scribble),
      35, uint(Tool::Traj),
      36, uint(Tool::DrawTraj),
      37, uint(Tool::TrajTangent),
      38, uint(Tool::Lasso),
      39, uint(Tool::Correspondence),
      40, uint(Tool::FillGrid),
      41, uint(Tool::DirectMatching),
      42, uint(Tool::MoveFrames),
      43, uint(Tool::Halves),
      44, uint(Tool::SimplifySpacing),
      45, uint(Tool::NoTool),

       0        // eod
};

Q_CONSTINIT const QMetaObject Tool::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSToolENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSToolENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSToolENDCLASS_t,
        // enum 'ToolType'
        QtPrivate::TypeAndForceComplete<Tool::ToolType, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Tool, std::true_type>,
        // method 'updateFrame'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toggled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'pressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const EventInfo &, std::false_type>,
        // method 'moved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const EventInfo &, std::false_type>,
        // method 'released'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const EventInfo &, std::false_type>,
        // method 'doublepressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const EventInfo &, std::false_type>,
        // method 'wheel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const WheelEventInfo &, std::false_type>,
        // method 'keyPressed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QKeyEvent *, std::false_type>,
        // method 'keyReleased'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QKeyEvent *, std::false_type>,
        // method 'draw'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QPainter &, std::false_type>,
        QtPrivate::TypeAndForceComplete<VectorKeyFrame *, std::false_type>,
        // method 'contextMenu'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QMenu &, std::false_type>
    >,
    nullptr
} };

void Tool::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Tool *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateFrame(); break;
        case 1: _t->toggled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->pressed((*reinterpret_cast< std::add_pointer_t<EventInfo>>(_a[1]))); break;
        case 3: _t->moved((*reinterpret_cast< std::add_pointer_t<EventInfo>>(_a[1]))); break;
        case 4: _t->released((*reinterpret_cast< std::add_pointer_t<EventInfo>>(_a[1]))); break;
        case 5: _t->doublepressed((*reinterpret_cast< std::add_pointer_t<EventInfo>>(_a[1]))); break;
        case 6: _t->wheel((*reinterpret_cast< std::add_pointer_t<WheelEventInfo>>(_a[1]))); break;
        case 7: _t->keyPressed((*reinterpret_cast< std::add_pointer_t<QKeyEvent*>>(_a[1]))); break;
        case 8: _t->keyReleased((*reinterpret_cast< std::add_pointer_t<QKeyEvent*>>(_a[1]))); break;
        case 9: _t->draw((*reinterpret_cast< std::add_pointer_t<QPainter&>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<VectorKeyFrame*>>(_a[2]))); break;
        case 10: _t->contextMenu((*reinterpret_cast< std::add_pointer_t<QMenu&>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Tool::*)();
            if (_t _q_method = &Tool::updateFrame; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *Tool::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Tool::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSToolENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Tool::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void Tool::updateFrame()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
