include(ReproductionMethod/ReproductionMethod.pri)
include(SelectionMethod/SelectionMethod.pri)
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

INCLUDEPATH += EvolutionEngine

SOURCES += evolutionengine.cpp

HEADERS += evolutionengine.h

