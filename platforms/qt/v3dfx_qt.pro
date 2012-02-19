#****************************************************************************
# *   Preliminary Qt implementation for v3dfxbase
#****************************************************************************

VERSION = 0.2

TEMPLATE = lib
TARGET = v3dfxqt
CONFIG += dll warn_on

HEADERS += v3dfx_qt.h

QT += opengl

LIBS += "-L/home/prabindh/work1/arm-qt/qt-everywhere-opensource-src-4.8.0/lib"
INCLUDEPATH += "/home/prabindh/work1/arm-qt/qt-everywhere-opensource-src-4.8.0/include/QtGui"
INCLUDEPATH += "/home/prabindh/work1/arm-qt/qt-everywhere-opensource-src-4.8.0/include/QtOpenGL"
INCLUDEPATH += "/home/prabindh/work1/arm-qt/qt-everywhere-opensource-src-4.8.0/include"

SOURCES += v3dfx_qt.cpp

INCLUDEPATH += $$QMAKE_INCDIR_EGL

DESTDIR = $$QMAKE_LIBDIR_QT
target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target

