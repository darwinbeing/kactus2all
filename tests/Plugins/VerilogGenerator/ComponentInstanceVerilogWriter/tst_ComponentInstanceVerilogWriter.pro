#-----------------------------------------------------------------------------
# File: tst_ComponentInstanceVerilogWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.7.2014
#
# Description:
# Qt project file for running unit tests for ComponentInstanceVerilogWriter.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ComponentInstanceVerilogWriter

QT += core xml gui testlib
CONFIG += testcase console

LIBS += -L$$PWD/../../../../executable/Plugins -lVerilogGeneratorPlugin

win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodelsd
    DESTDIR = ./debug
}
else:unix {
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}

INCLUDEPATH += $$PWD/../../../../
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../../../
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_ComponentInstanceVerilogWriter.pri)
