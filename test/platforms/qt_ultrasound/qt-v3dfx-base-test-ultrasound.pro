#****************************************************************************
# *   Preliminary Qt test code for v3dfxbase (Ultrasound)
# To generate Makefile, perform
# ~/work1/arm-qt/qt-everywhere-opensource-src-4.8.0/bin/qmake
#****************************************************************************

TEMPLATE = app

SOURCES += main.cpp qt-v3dfx-ultrasound.cpp ultrasound_renderer.cpp
LIBS += "-L/home/prabindh/work1/arm-qt/qt-everywhere-opensource-src-4.8.0/lib"
LIBS += "-L../../../platforms/qt"
LIBS += "-lv3dfxqt"
LIBS += ../../../api/build/v3dfx-base.a

INCLUDEPATH += "/home/prabindh/work1/arm-qt/qt-everywhere-opensource-src-4.8.0/include/QtGui"
INCLUDEPATH += "/home/prabindh/work1/arm-qt/qt-everywhere-opensource-src-4.8.0/include/QtOpenGL"
INCLUDEPATH += "/home/prabindh/work1/arm-qt/qt-everywhere-opensource-src-4.8.0/include/QtCore"
INCLUDEPATH += "/home/prabindh/work1/arm-qt/qt-everywhere-opensource-src-4.8.0/include"
INCLUDEPATH += "../../../platforms/qt"

HEADERS += ../../../platforms/qt/v3dfx_qt.h
HEADERS += qt-v3dfx-test.h

INCLUDEPATH += ../../../platforms/qt

contains(QT_CONFIG, opengl):QT += opengl

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

# install
target.path = $$[QT_INSTALL_DEMOS]/qt-v3dfx-base-test-ultrasound
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.png *.pro *.html *.doc images
sources.path = $$[QT_INSTALL_DEMOS]/qt-v3dfx-base-test-ultrasound
INSTALLS += target sources

################### V3DFX-IMGSTREAM SPECIFIC ADDITIONS ################
LIBS += ../../../../../ti-dvsdk_dm3730-evm_04_03_00_06/linuxutils_2_26_02_05/packages/ti/sdo/linuxutils/cmem/lib/cmem.a470MV
LIBS += ../../../api/build/v3dfx-base.a
INCLUDEPATH += "../../../api/include"
INCLUDEPATH += $$SGX_SDK_ROOT/GFX_Linux_KM/include4
INCLUDEPATH += $$SGX_SDK_ROOT/GFX_Linux_KM/services4/3rdparty/bufferclass_ti
INCLUDEPATH += ../../../../../ti-dvsdk_dm3730-evm_04_03_00_06/linuxutils_2_26_02_05/packages/ti/sdo/linuxutils/cmem/include

