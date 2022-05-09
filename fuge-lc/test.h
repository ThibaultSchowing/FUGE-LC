#ifndef TEST_H
#define TEST_H

#include <QThread>
#include <Qt>

#include "EvolutionEngine/evolutionengine.h"
#include "Population/population.h"
#include "Population/Individual/popentity.h"

class Test : public QThread, public EvolutionEngine
{
    Q_OBJECT
public:
    explicit Test(Population* left, Population* right, quint32 generationCount, QObject *parent = 0);
    void run();
    void evaluatePopulation(Population* population);
signals:
    
public slots:
    
private :
    Population* left;
    Population* right;
};

#endif // TEST_H
