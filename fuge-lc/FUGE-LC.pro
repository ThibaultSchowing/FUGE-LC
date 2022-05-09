# the original application seems to have been written for Qt 4.7 and Qwt 5.2.1 on Unix
#   assumingly by jean-philippe.meylan_at_heig-vd.ch
# this version was successfully compiled and run with Qt 4.8.7 on Linux and Win32
#   with only the minimal required changes done by me@rochus-keller.ch on 2022-05-04
#   on behalf of thibault.schowing@heig-vd.ch
# it is recommended to subject the original source code to a design review, as there
#   seem to be unnecessary copy operations and likely memory leaks

TARGET = FUGE-LC
TEMPLATE = app

QT += xml \
    script \
    core gui widgets

include(libGGA/libGGA.pri)
include(fuzzy/Fuzzy.pri)
include(coev/Coev.pri)
include(Dependencies/qwt/qwt.pri)

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

