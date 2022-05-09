#include "evolutionengine.h"
#include "../computethread.h"

QMutex * EvolutionEngine::critMutex = new QMutex();


EvolutionEngine::EvolutionEngine(Population *population, quint32 generationCount, qreal crossoverProbability, qreal mutationProbability, qreal mutationPerBitProbability) :
    population(population), generationCount(generationCount), crossoverProbability(crossoverProbability), mutationProbability(mutationProbability), mutationPerBitProbability(mutationPerBitProbability)
{
    //entitySelectionMethodList.push_back(new Elitism());
    entitySelectionMethodList.push_back(new ElitismWithRandom());
    entitySelectionMethodList.push_back(new RankBasedSelection());

    crossoverMethodList.push_back(new OnePoint());
    mutateMethodList.push_back(new Toggling());
}

void EvolutionEngine::startEvolution(QMutex *leftLock, QMutex *rightLock, quint32 generationCount, EntitySelection *eliteSelection, quint32 eliteSelectionCount, EntitySelection *individualsSelection, quint32 individualsSelectionCount, Mutate *mutateMethod, Crossover *crossoverMethod, quint32 cooperatorsCount)
{
    // TODO: crashes here when run under Qt 5 (but not under Qt 4) because leftLock and rightLock are invalid

    setEntitySelector(eliteSelection,eliteSelectionCount,individualsSelection,individualsSelectionCount);

    this->mutateMethod = mutateMethod;
    this->crossoverMethod = crossoverMethod;

    rightLock->lock();
    selectElites();
    population->setRepresentativesCopy(selectedEntitiesCopy, cooperatorsCount);

    qDebug() << population->getName() << " Before join";
    // JOIN both evolution
    leftLock->unlock();
    rightLock->lock();
    leftLock->unlock();

    qDebug() << population->getName() << " AFTER join";
    if(!evaluatePopulation(population, 0))
        return;

    for(quint32 i = 1; i <= generationCount; i++)
    {
        // Select elites from the population
        selectElites();

        // Set cooperators
        population->setRepresentativesCopy(selectedEntitiesCopy, cooperatorsCount);

        // Select non elite individuals
        selectIndividuals();

        // Crossover
        crossover();

        // Mutate
        mutate();
        population->replace(selectedEntitiesCopy, evolvingEntitiesCopy);

        // Evaluate population
        if(!evaluatePopulation(population, i))
            break;


    }
    rightLock->lock();
    qDebug() << population->getName() << " Before LAST join";
    // Join and leave
    leftLock->unlock();
    rightLock->lock();
    qDebug() << population->getName() << " AFTER LAST join";
    leftLock->unlock();
}

void EvolutionEngine::setEntitySelector(EntitySelection *eliteSelection, quint32 eliteSelectionCount, EntitySelection *entitySelection, quint32 selectionCount)
{
    this->eliteSelection = eliteSelection;
    this->eliteSelectionCount = eliteSelectionCount;
    this->individualsSelection = entitySelection;
    this->individualsSelectionCount = selectionCount;
}
void EvolutionEngine::setMutationMethod(Mutate * mutateMethod, quint32 mutationProbability)
{
    this->mutateMethod = mutateMethod;
    this->mutationProbability = mutationProbability;
}
void EvolutionEngine::setCrossoverMethod(Crossover * crossoverMethod)
{
    this->crossoverMethod = crossoverMethod;
}

Population *EvolutionEngine::getPopulation(){
    return population;
}

vector<EntitySelection *> EvolutionEngine::getEntitySelectors()
{
    vector<EntitySelection *> availableSelection;
    for(quint32 i=0; i < entitySelectionMethodList.size(); i++)
        availableSelection.push_back(entitySelectionMethodList.at(i));
    return availableSelection;
}
vector<Mutate *> EvolutionEngine::getMutationMethods()
{
    vector<Mutate *> availableMutation;
    for(quint32 i=0; i < mutateMethodList.size(); i++)
        availableMutation.push_back(mutateMethodList.at(i));
    return availableMutation;
}
vector<Crossover *> EvolutionEngine::getCrossoverMethods()
{
    vector<Crossover *> availableCrossover;
    for(quint32 i=0; i < crossoverMethodList.size(); i++)
        availableCrossover.push_back(crossoverMethodList.at(i));
    return availableCrossover;
}

// Private :
void EvolutionEngine::initializePopulation()
{
    population->randomizePopulation();
}
void EvolutionEngine::selectElites()
{
//    for(int i = 0; i < selectedEntitiesCopy.size(); i++)
//        delete selectedEntitiesCopy[i];
    selectedEntitiesCopy.clear();
    selectedEntitiesCopy = population->getSomeEntityCopy(eliteSelection,eliteSelectionCount);

}

bool EvolutionEngine::isElite(Genotype *genotype)
{
    for(quint32 i=0; i < selectedEntitiesCopy.size(); i++){
        if(genotype->getData()->operator ==(*selectedEntitiesCopy.at(i)->getGenotype()->getData()))
            return true;
    }
    return false;
}

void EvolutionEngine::selectIndividuals()
{
//    for(uint i = 0; i < evolvingEntitiesCopy.size(); i++)
//        delete evolvingEntitiesCopy[i];
    evolvingEntitiesCopy.clear();
    evolvingEntitiesCopy = population->getSomeEntityCopy(individualsSelection,individualsSelectionCount);
}
void EvolutionEngine::crossover()
{
    // Missing probability
    // Random list before crossover (To do if better results and not too processing time consuming)
    crossoverMethod->reproducePairOf(evolvingEntitiesCopy, crossoverProbability);
}
void EvolutionEngine::mutate()
{

    vector<PopEntity *>::iterator it;
    for(it=evolvingEntitiesCopy.begin(); it!=evolvingEntitiesCopy.end(); it++)
    {
        qreal entitiyLuck = RandomGenerator::getGeneratorInstance()->randomReal(0,1);
        //  if goal < chance OK.
        if(entitiyLuck < mutationProbability){
            mutateMethod->mutateEntity(*it, mutationPerBitProbability);
        }
    }
}

StatisticEngine *EvolutionEngine::getStatisticEngine(){
    return &statsEngine;
}
