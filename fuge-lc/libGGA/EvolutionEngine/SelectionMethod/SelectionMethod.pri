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

HEADERS += $$PWD/elitism.h \
    $$PWD/entityselection.h \
    $$PWD/generationalreplacement.h \
    $$PWD/proportionalselection.h \
    $$PWD/rankbasedselection.h \
    $$PWD/roulettewheel.h \
    $$PWD/selectionpressure.h \
    $$PWD/tournamentselection.h \
    $$PWD/truncatedrankbasedselection.h \
    libGGA/EvolutionEngine/SelectionMethod/elitismwithrandom.h
