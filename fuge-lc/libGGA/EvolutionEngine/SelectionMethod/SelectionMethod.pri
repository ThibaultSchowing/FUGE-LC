INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += $$PWD/elitism.cpp \
    $$PWD/entityselection.cpp \
    $$PWD/generationalreplacement.cpp \
    $$PWD/proportionalselection.cpp \
    $$PWD/rankbasedselection.cpp \
    $$PWD/roulettewheel.cpp \
    $$PWD/selectionpressure.cpp \
    $$PWD/tournamentselection.cpp \
    $$PWD/truncatedrankbasedselection.cpp \
    libGGA/EvolutionEngine/SelectionMethod/elitismwithrandom.cpp

HEADERS += elitism.h \
    entityselection.h \
    generationalreplacement.h \
    proportionalselection.h \
    rankbasedselection.h \
    roulettewheel.h \
    selectionpressure.h \
    tournamentselection.h \
    truncatedrankbasedselection.h \
    libGGA/EvolutionEngine/SelectionMethod/elitismwithrandom.h
