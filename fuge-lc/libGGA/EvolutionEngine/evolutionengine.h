#ifndef EVOLUTIONENGINE_H
#define EVOLUTIONENGINE_H

#include <vector>
#include <QThread>

#include "population.h"
#include "crossover.h"
#include "onepoint.h"
#include "mutate.h"
#include "toggling.h"
#include "entityselection.h"
#include "elitism.h"
#include "elitismwithrandom.h"
#include "rankbasedselection.h"
#include "statisticengine.h"

class EvolutionEngine
{
public:
    EvolutionEngine(Population *population, quint32 generationCount, qreal crossoverProbability, qreal mutationProbability, qreal mutationPerBitProbability);
    virtual bool evaluatePopulation(Population* population, quint32 generation) = 0;
    void startEvolution(QMutex *leftLock,
                                            QMutex *rightLock,
                                            quint32 generationCount,
                                            EntitySelection *eliteSelection,
                                            quint32 eliteSelectionCount,
                                            EntitySelection *individualsSelection,
                                            quint32 individualsSelectionCount,
                                            Mutate *mutateMethod, Crossover *crossoverMethod, quint32 cooperatorsCount);
    void setEntitySelector(EntitySelection *eliteSelection, quint32 eliteSelectionCount, EntitySelection * individualsSelection, quint32 individualsSelectionCount);
    void setMutationMethod(Mutate * mutateMethod, quint32 mutationProbability);
    void setCrossoverMethod(Crossover * crossoverMethod);

    void replacePopulation();
    void replacePopulation(Population *population);
    bool isElite(Genotype *genotype);

    StatisticEngine *getStatisticEngine();
    Population *getPopulation();

    vector<EntitySelection *> getEntitySelectors();
    vector<Mutate *> getMutationMethods();
    vector<Crossover *> getCrossoverMethods();

    static QMutex * critMutex;

protected:
    quint32 generationCount;
    StatisticEngine statsEngine;

private:
    void initializePopulation();
    void selectElites();
    void selectIndividuals();
    void crossover();
    void mutate();

    Population *population;
    qreal crossoverProbability;
    qreal mutationProbability;
    qreal mutationPerBitProbability;

    EntitySelection *eliteSelection;
    quint32 eliteSelectionCount;
    EntitySelection *individualsSelection;
    quint32 individualsSelectionCount;
    Mutate *mutateMethod;

    Crossover *crossoverMethod;



    vector<EntitySelection *> entitySelectionMethodList;
    vector<Mutate *> mutateMethodList;
    vector<Crossover *> crossoverMethodList;

    vector<PopEntity *> selectedEntitiesCopy;
    vector<PopEntity *> evolvingEntitiesCopy;
};

#endif // EVOLUTIONENGINE_H
