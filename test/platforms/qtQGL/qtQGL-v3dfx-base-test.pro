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

