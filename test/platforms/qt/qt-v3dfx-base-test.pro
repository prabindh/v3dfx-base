#****************************************************************************
# *   Preliminary Qt test code for v3dfxbase
# To generate Makefile, perform
# ~/work1/arm-qt/qt-everywhere-opensource-src-4.8.0/bin/qmake -o Makefile qt-v3dfx-base-test.pro
#****************************************************************************

TEMPLATE = app

SOURCES += main.cpp qt-v3dfx-test.cpp
LIBS += "-L/home/prabindh/work1/arm-qt/qt-everywhere-opensource-src-4.8.0/lib"
LIBS += "-L../../../platforms/qt"
LIBS += "-lv3dfxqt"
INCLUDEPATH += "/home/prabindh/work1/arm-qt/qt-everywhere-opensource-src-4.8.0/include/QtGui"
INCLUDEPATH += "/home/prabindh/work1/arm-qt/qt-everywhere-opensource-src-4.8.0/include/QtOpenGL"
INCLUDEPATH += "/home/prabindh/work1/arm-qt/qt-everywhere-opensource-src-4.8.0/include"
INCLUDEPATH += "../../../platforms/qt"

HEADERS += ../../../platforms/qt/v3dfx_qt.h
HEADERS += qt-v3dfx-test.h

contains(QT_CONFIG, opengl):QT += opengl

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

# install
target.path = $$[QT_INSTALL_DEMOS]/qt-v3dfx-base-test
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.png *.pro *.html *.doc images
sources.path = $$[QT_INSTALL_DEMOS]/qt-v3dfx-base-test
INSTALLS += target sources

symbian: include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
