include(Genotype/Genotype.pri)

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += $$PWD/individual.cpp \
    $$PWD/popentity.cpp \
    $$PWD/representative.cpp
    
HEADERS += $$PWD/individual.h \
    $$PWD/popentity.h \
    $$PWD/representative.h
    
