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
QMAKE_CXXFLAGS_RELEASE += -O2
QMAKE_CXXFLAGS_DEBUG += -O2

win32 {
    ## Windows common
    QMAKE_CXXFLAGS_RELEASE += -std=gnu++0x
    QMAKE_CXXFLAGS_DEBUG += -std=gnu++0x
    !contains(QMAKE_HOST.arch, x86_64) {
        message("windows x86 build")
        ## Windows x86
        LIBS += -lqwt5 \
                -L$$_PRO_FILE_PWD_/Dependencies/include/qwt \
                -L$$_PRO_FILE_PWD_/Dependencies/lib

        INCLUDEPATH += $$_PRO_FILE_PWD_/Dependencies/include/qwt \
                       $$_PRO_FILE_PWD_/Dependencies/lib

    } else {
        message("windows x64 build")
        ## Windows x64
        LIBS += -lqwt5 \
                -L$$_PRO_FILE_PWD_/Dependencies/include/qwt \
                -L$$_PRO_FILE_PWD_/Dependencies/lib

        INCLUDEPATH += $$_PRO_FILE_PWD_/Dependencies/include/qwt \
                       $$_PRO_FILE_PWD_/Dependencies/lib
    }
}

unix {
    message("unix build")
    ## Linux common. (/!\ Mac OS will use the same parameters. There is a special tag for macox in qmake if needed, but it shouldn't be.)
    QMAKE_CXXFLAGS_RELEASE += -std=c++0x
    QMAKE_CXXFLAGS_DEBUG += -std=c++0x
    LIBS += -lqwt \
##CONFIG_SERVER_IICT
##CONFIG_PERSONAL_COMPUTER
        -L/usr/local/qwt-5.2.1/lib \
        -L/usr/include/qwt \
        -L/usr/local/lib
##CONFIG_SERVER_IICT
INCLUDEPATH += /usr/include/qwt \
##CONFIG_PERSONAL_COMPUTER
INCLUDEPATH += /usr/local/qwt-5.2.1/include \
                   /usr/local/lib
}
CONFIG += static \
        qwt

QT += xml \
    script \
    core

TEMPLATE = app

include(libGGA/libGGA.pri)
include(fuzzy/Fuzzy.pri)
include(coev/Coev.pri)

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
