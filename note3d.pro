QT += core gui sql serialport widgets xml charts svg concurrent

CONFIG += c++17 console
CONFIG -= app_bundle

QMAKE_CXXFLAGS += /arch:AVX
QMAKE_CXXFLAGS += /MP /utf-8 /bigobj
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CFLAGS_RELEASE += -Zi
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -Zi
QMAKE_LFLAGS_RELEASE -= /INCREMENTAL:NO
QMAKE_LFLAGS_RELEASE += /DEBUG

TEMPLATE = lib
TARGET = note3d

DEFINES += LIB_NOTE3_EXPORT  # lib
DEFINES += QT_DEPRECATED_WARNINGS

include(../../CtPackage.pri)
include(../algorithm/algorithm.pri)

FORMS += \
    Note3ItemListWidget.ui \
    Note3VolumeViewer.ui \
    tools/Note3ArbSurfaceToolWidget.ui \
    tools/Note3DotToolWidget.ui \
    tools/Note3LineToolWidget.ui \
    tools/Note3BallToolWidget.ui \
    tools/Note3ShapeDiameterToolWidget.ui \
    tools/Note3CircleToolWidget.ui \
    tools/Note3ConeToolWidget.ui \
    tools/Note3CylinderToolWidget.ui \
    tools/Note3PanelToolWidget.ui

HEADERS += \
    Lib_Note3_export.h \
    Note3ItemInterface.h \
    Note3ItemListWidget.h \
    Note3VolumeInteractorStyle.h \
    Note3VolumeViewer.h \
    tools/Note3ArbSurfaceToolWidget.h \
    tools/Note3DotToolWidget.h \
    tools/Note3LineToolWidget.h \
    tools/Note3BallToolWidget.h \
    tools/Note3ShapeDiameterToolWidget.h \
    tools/Note3CircleToolWidget.h \
    tools/Note3ConeToolWidget.h \
    tools/Note3CylinderToolWidget.h \
    tools/Note3PanelToolWidget.h \
    items/Note3ConicityItem.h \
    items/Note3CylindricityItem.h \
    items/Note3ElevationCalculation.h \
    items/Note3SurfaceAreaVolume.h \
    items/Note3TriangleQuality.h \
    items/Note3ArbSurfaceItem.h \
    items/Note3CurvatureItem.h \
    items/Note3DotItem.h \
    items/Note3LineItem.h \
    items/Note3BallItem.h \
    items/Note3ShapeDiameterItem.h \
    items/Note3SurfaceRoughnessItem.h \
    items/Note3LineRoughnessItem.h \
    items/Note3LinearityItem.h \
    items/Note3PlanarityItem.h \
    items/Note3SurfaceStatisticsItem.h \
    items/Note3RoundnessItem.h \
    items/Note3CircleItem.h \
    items/Note3ConeItem.h \
    items/Note3CylinderItem.h

SOURCES += \
    Note3ItemInterface.cpp \
    Note3ItemListWidget.cpp \
    Note3VolumeInteractorStyle.cpp \
    Note3VolumeViewer.cpp \
    tools/Note3ArbSurfaceToolWidget.cpp \
    tools/Note3DotToolWidget.cpp \
    tools/Note3LineToolWidget.cpp \
    tools/Note3BallToolWidget.cpp \
    tools/Note3ShapeDiameterToolWidget.cpp \
    tools/Note3CircleToolWidget.cpp \
    tools/Note3ConeToolWidget.cpp \
    tools/Note3CylinderToolWidget.cpp \
    tools/Note3PanelToolWidget.cpp \
    items/Note3ArbSurfaceItem.cpp \
    items/Note3CurvatureItem.cpp \
    items/Note3DotItem.cpp \
    items/Note3LineItem.cpp \
    items/Note3BallItem.cpp \
    items/Note3ShapeDiameterItem.cpp \
    items/Note3SurfaceRoughnessItem.cpp \
    items/Note3LinearityItem.cpp \
    items/Note3LineRoughnessItem.cpp \
    items/Note3PlanarityItem.cpp \
    items/Note3SurfaceStatisticsItem.cpp \
    items/Note3RoundnessItem.cpp \
    items/Note3CircleItem.cpp \
    items/Note3ConeItem.cpp \
    items/Note3CylinderItem.cpp \
    items/Note3ConicityItem.cpp \
    items/Note3CylindricityItem.cpp \
    items/Note3ElevationCalculation.cpp \
    items/Note3SurfaceAreaVolume.cpp \
    items/Note3TriangleQuality.cpp

DESTDIR = $$PWD/../../installs/note3d

EXPORT_HEADERS.files += \
    $$PWD/Note3VolumeViewer.h \
    $$PWD/Lib_Note3_export.h

EXPORT_HEADERS.path = $$DESTDIR/include

EXPORT_PRI.files += \
    note3d.pri

EXPORT_PRI.path += \
    $$DESTDIR

INSTALLS += \
    EXPORT_HEADERS  \
    EXPORT_PRI

DISTFILES += \
    $$PWD/note3d.pri
