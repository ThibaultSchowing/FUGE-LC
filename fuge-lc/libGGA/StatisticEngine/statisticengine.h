#ifndef STATISTICENGINE_H
#define STATISTICENGINE_H

#include <QList>
#include <vector>
#include "evolutionarymesure.h"
#include "math.h"
class StatisticEngine
{
public:
    StatisticEngine();
    void addFitness(qreal fitness);
    void buildStats();
    void reset();

    qreal getMinFitness();
    qreal getMaxFitness();
    qreal getMeanFitness();
    qreal getStandardDeviation();

private:
    QList<qreal> fitnesslist;
    qreal minfitness;
    qreal maxfitness;
    qreal meanfitness;
    qreal standardDeviation;

    QList<EvolutionaryMesure> evolutionaryMesures;
};

#endif // STATISTICENGINE_H
