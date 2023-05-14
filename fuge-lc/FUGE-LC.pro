# -------------------------------------------------
# Project created by QtCreator 2009-06-15T13:25:19
# -------------------------------------------------
TARGET = FUGE-LC

# Profiling
#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg

# Agressive flags for release
# (Edit : Yvan Da Silva, I do not recommend using 03, it can produce unexpected behaviors)
# (Take a look at the end of this file for more explanations)
TARGET = FUGE-LC
TEMPLATE = app

QT += core gui widgets xml charts

include(libGGA/libGGA.pri)
include(fuzzy/Fuzzy.pri)
include(fuzzyLearning/FuzzyLearning.pri)
include(coev/Coev.pri)
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

OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
DESTDIR = bin

RESOURCES += fuzzyResources.qrc

#From GCC developers discussion :
#The -O3 optimization level may increase the speed of the resulting executable, but can also increase its size.
#Under some circumstances where these optimizations are not favorable, this option might actually make a program slower.
#In fact it should not be used system-wide with gcc 4.x. The behavior of gcc has changed significantly since version 3.x.
#In 3.x, -O3 has been shown to lead to marginally faster execution times over -O2, but this is no longer the case with gcc 4.x.
#Compiling all your packages with -O3 will result in larger binaries that require more memory,
#and will significantly increase the odds of compilation failure or unexpected program behavior (including errors).
#The downsides outweigh the benefits; remember the principle of diminishing returns. Using -O3 is not recommended for gcc 4.x.
