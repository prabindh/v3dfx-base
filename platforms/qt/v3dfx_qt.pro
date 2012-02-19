#****************************************************************************
# *   Preliminary Qt implementation for v3dfxbase
#****************************************************************************

VERSION = 0.2

TEMPLATE = lib
TARGET = v3dfxqt
CONFIG += dll warn_on

HEADERS += v3dfx_qt.h

QT += opengl

LIBS += -L$$QT_INSTALL_DIR/lib

INCLUDEPATH += $$QT_INSTALL_DIR/include/QtGui
INCLUDEPATH += $$QT_INSTALL_DIR/include/QtOpenGL
INCLUDEPATH += $$QT_INSTALL_DIR/include

SOURCES += v3dfx_qt.cpp

INCLUDEPATH += $$QMAKE_INCDIR_EGL

DESTDIR = $$QMAKE_LIBDIR_QT
target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target


################### V3DFX-IMGSTREAM SPECIFIC ADDITIONS ################
DEFINES += _ENABLE_CMEM
LIBS += ../../../../ti-dvsdk_dm3730-evm_04_03_00_06/linuxutils_2_26_02_05/packages/ti/sdo/linuxutils/cmem/lib/cmem.a470MV
LIBS += ../../api/build/v3dfx-base.a
INCLUDEPATH += "../../api/include"
INCLUDEPATH += $$SGX_SDK_ROOT/GFX_Linux_KM/include4
INCLUDEPATH += $$SGX_SDK_ROOT/GFX_Linux_KM/services4/3rdparty/bufferclass_ti
INCLUDEPATH += ../../../../ti-dvsdk_dm3730-evm_04_03_00_06/linuxutils_2_26_02_05/packages/ti/sdo/linuxutils/cmem/include
SOURCES += v3dfx_qt_imgstream.cpp
SOURCES += ../../test/imgstream/sgxperf_test8.cpp

