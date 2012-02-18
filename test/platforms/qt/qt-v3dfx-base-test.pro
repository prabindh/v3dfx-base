#****************************************************************************
# *   Preliminary Qt test code for v3dfxbase
#****************************************************************************

SOURCES += main.cpp qt-v3dfx-test.cpp
LIBS += "-L/home/prabindh/work1/arm-qt/qt-everywhere-opensource-src-4.8.0/lib"
INCLUDEPATH += "/home/prabindh/work1/arm-qt/qt-everywhere-opensource-src-4.8.0/include/QtGui"
INCLUDEPATH += "/home/prabindh/work1/arm-qt/qt-everywhere-opensource-src-4.8.0/include"

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
