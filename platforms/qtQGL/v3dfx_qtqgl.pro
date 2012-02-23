#****************************************************************************
# *   Preliminary Qt implementation for v3dfxbase (QGLWidget based)
#****************************************************************************

TEMPLATE = lib
TARGET = v3dfx_qtqgl
CONFIG += dll warn_on

HEADERS += v3dfx_qtqgl.h

QT += opengl

LIBS += -L$$QT_INSTALL_DIR/lib

INCLUDEPATH += $$QT_INSTALL_DIR/include/QtGui
INCLUDEPATH += $$QT_INSTALL_DIR/include/QtOpenGL
INCLUDEPATH += $$QT_INSTALL_DIR/include

SOURCES += v3dfx_qtqgl.cpp

INCLUDEPATH += $$QMAKE_INCDIR_EGL

DESTDIR = $$QMAKE_LIBDIR_QT
target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target

################### V3DFX-IMGSTREAM SPECIFIC ADDITIONS ################
DEFINES += _ENABLE_CMEM
LIBS += ../../api/build/v3dfx-base.a
INCLUDEPATH += "../../api/include"
INCLUDEPATH += $$SGX_SDK_ROOT/GFX_Linux_KM/include4
INCLUDEPATH += $$SGX_SDK_ROOT/GFX_Linux_KM/services4/3rdparty/bufferclass_ti
HEADERS += "../../api/include/v3dfxbase.h"
