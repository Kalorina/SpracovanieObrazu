/****************************************************************************
** Meta object code from reading C++ file 'ImageViewer.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/ImageViewer.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ImageViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.2. It"
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
struct qt_meta_tag_ZN11ImageViewerE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN11ImageViewerE = QtMocHelpers::stringData(
    "ImageViewer",
    "on_actionOpen_triggered",
    "",
    "on_actionSave_as_triggered",
    "on_actionExit_triggered",
    "on_actionOriginal_triggered",
    "on_actionInvert_triggered",
    "on_actionFSHS_triggered",
    "on_actionEH_triggered",
    "on_actionConvolution_triggered",
    "on_actionLinearHeatEq_Scheme_triggered",
    "on_actionEdge_Detector_triggered",
    "updateImageFromSpinBoxExplicitLH",
    "index"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN11ImageViewerE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   80,    2, 0x08,    1 /* Private */,
       3,    0,   81,    2, 0x08,    2 /* Private */,
       4,    0,   82,    2, 0x08,    3 /* Private */,
       5,    0,   83,    2, 0x08,    4 /* Private */,
       6,    0,   84,    2, 0x08,    5 /* Private */,
       7,    0,   85,    2, 0x08,    6 /* Private */,
       8,    0,   86,    2, 0x08,    7 /* Private */,
       9,    0,   87,    2, 0x08,    8 /* Private */,
      10,    0,   88,    2, 0x08,    9 /* Private */,
      11,    0,   89,    2, 0x08,   10 /* Private */,
      12,    1,   90,    2, 0x08,   11 /* Private */,

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
    QMetaType::Void, QMetaType::Int,   13,

       0        // eod
};

Q_CONSTINIT const QMetaObject ImageViewer::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_ZN11ImageViewerE.offsetsAndSizes,
    qt_meta_data_ZN11ImageViewerE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN11ImageViewerE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ImageViewer, std::true_type>,
        // method 'on_actionOpen_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionSave_as_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionExit_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionOriginal_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionInvert_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionFSHS_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionEH_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionConvolution_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionLinearHeatEq_Scheme_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionEdge_Detector_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateImageFromSpinBoxExplicitLH'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void ImageViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ImageViewer *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->on_actionOpen_triggered(); break;
        case 1: _t->on_actionSave_as_triggered(); break;
        case 2: _t->on_actionExit_triggered(); break;
        case 3: _t->on_actionOriginal_triggered(); break;
        case 4: _t->on_actionInvert_triggered(); break;
        case 5: _t->on_actionFSHS_triggered(); break;
        case 6: _t->on_actionEH_triggered(); break;
        case 7: _t->on_actionConvolution_triggered(); break;
        case 8: _t->on_actionLinearHeatEq_Scheme_triggered(); break;
        case 9: _t->on_actionEdge_Detector_triggered(); break;
        case 10: _t->updateImageFromSpinBoxExplicitLH((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *ImageViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ImageViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN11ImageViewerE.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int ImageViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
