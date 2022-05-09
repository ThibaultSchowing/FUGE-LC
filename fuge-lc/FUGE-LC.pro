# the original application seems to have been written for Qt 4.7 and Qwt 5.2.1 on Unix
#   assumingly by jean-philippe.meylan_at_heig-vd.ch
# this version was successfully compiled and run with Qt 4.8.7 on Linux and Win32
#   with only the minimal required changes done by me@rochus-keller.ch on 2022-05-04
#   on behalf of thibault.schowing@heig-vd.ch
# it is recommended to subject the original source code to a design review, as there
#   seem to be unnecessary copy operations and likely memory leaks
# this version also compiles with Qt 5.x, but there are crashes with Qt 5.x on all
#   platforms (Linux i386, Mac x64, Windows x86) which don't occur with Qt 4.x.

TARGET = FUGE-LC
TEMPLATE = app

QT += core gui widgets xml

include(libGGA/libGGA.pri)
include(fuzzy/Fuzzy.pri)
include(coev/Coev.pri)
include(Dependencies/qwt/qwt.pri)
include(Dependencies/duktape/duktape.pri)

SOURCES += main.cpp \
    fugemain.cpp \
    aboutdialog.cpp \
    errordialog.cpp \
    fuzzymembershipssingle.cpp \
    statsplot.cpp \
    computethread.cpp \
    systemparameters.cpp \
    editparamsdialog.cpp \
    evalplot.cpp \
    infodialog.cpp \
    fuzzyeditor.cpp \
    scriptmanager.cpp \
    helpdialog.cpp

HEADERS += fugemain.h \
    aboutdialog.h \
    errordialog.h \
    fuzzymembershipssingle.h \
    statsplot.h \
    computethread.h \
    systemparameters.h \
    editparamsdialog.h \
    evalplot.h \
    infodialog.h \
    fuzzyeditor.h \
    scriptmanager.h \
    helpdialog.h

FORMS += fugemain.ui \
    aboutdialog.ui \
    errordialog.ui \
    statsplot.ui \
    editparamsdialog.ui \
    evalplot.ui \
    infodialog.ui \
    evalplotframe.ui \
    fuzzyeditor.ui \
    helpdialog.ui

RESOURCES += fuzzyResources.qrc

