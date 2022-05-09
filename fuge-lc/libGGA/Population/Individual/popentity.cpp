#include "popentity.h"

PopEntity::PopEntity()
{
}

PopEntity::PopEntity(quint32 lenght) :
    genotype(new Genotype(lenght)),
    fitness(0)
{
}

PopEntity::~PopEntity(){
    if(genotype != NULL)
        delete genotype;
}

PopEntity::PopEntity(Genotype *popEntity) :
             genotype(popEntity->getCopy()),
             fitness(0)
{
}

PopEntity::PopEntity(PopEntity *popEntity) :
             genotype(new Genotype(popEntity->getGenotype()->getData())),
             fitness(popEntity->getFitness())
{
}

bool PopEntity::operator< (PopEntity *rEntity)
{
        return getFitness() < rEntity->getFitness();
}

qreal PopEntity::getFitness()
{
    return fitness;
}

Genotype *PopEntity::getGenotype()
{
    return genotype;
}

PopEntity *PopEntity::getCopy()
{
    return new PopEntity(this);
}
