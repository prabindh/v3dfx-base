#****************************************************************************
# *   Preliminary Qt test code for v3dfxbase (QGLWidget based)
# To generate Makefile, perform
# qmake -o Makefile qtQGL-v3dfx-base-test.pro
#****************************************************************************

TEMPLATE = app

SOURCES += v3dfxtest_window.cpp main.cpp videowidget.cpp
LIBS += -L$$QT_INSTALL_DIR/lib
LIBS += "-L../../../platforms/qtQGL"
LIBS += "-lv3dfx_qtqgl"
LIBS += ../../../api/build/v3dfx-base.a

INCLUDEPATH += $$QT_INSTALL_DIR/include/QtGui
INCLUDEPATH += $$QT_INSTALL_DIR/include/QtOpenGL
INCLUDEPATH += $$QT_INSTALL_DIR/include
INCLUDEPATH += $$QT_INSTALL_DIR/include/QtCore
INCLUDEPATH += "../../../platforms/qtQGL"

HEADERS += ../../../platforms/qtQGL/v3dfx_qtqgl.h
HEADERS += v3dfxtest_window.h
HEADERS += videowidget.h

contains(QT_CONFIG, opengl):QT += opengl

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

# install
target.path = $$[QT_INSTALL_DEMOS]/qtQGL-v3dfx-base-test
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.png *.pro *.html *.doc images
sources.path = $$[QT_INSTALL_DEMOS]/qtQGL-v3dfx-base-test
INSTALLS += target sources


################### V3DFX-IMGSTREAM SPECIFIC ADDITIONS ################
DEFINES += _ENABLE_CMEM
LIBS += ../../../../../ti-dvsdk_dm3730-evm_04_03_00_06/linuxutils_2_26_02_05/packages/ti/sdo/linuxutils/cmem/lib/cmem.a470MV
LIBS += ../../../api/build/v3dfx-base.a
INCLUDEPATH += "../../../api/include"
INCLUDEPATH += $$SGX_SDK_ROOT/GFX_Linux_KM/include4
INCLUDEPATH += $$SGX_SDK_ROOT/GFX_Linux_KM/services4/3rdparty/bufferclass_ti
INCLUDEPATH += ../../../../../ti-dvsdk_dm3730-evm_04_03_00_06/linuxutils_2_26_02_05/packages/ti/sdo/linuxutils/cmem/include
SOURCES += ../../../platforms/qt/v3dfx_qt_imgstream.cpp
SOURCES += ../../../test/imgstream/sgxperf_test8.cpp

