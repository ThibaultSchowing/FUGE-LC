include(ReproductionMethod/ReproductionMethod.pri)
include(SelectionMethod/SelectionMethod.pri)
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

INCLUDEPATH += $$PWD/EvolutionEngine

SOURCES += $$PWD/evolutionengine.cpp

HEADERS += $$PWD/evolutionengine.h

