#ifndef POPENTITY_H
#define POPENTITY_H

#include <memory>
#include <Qt>

#include "genotype.h"
#include "randomgenerator.h"

class PopEntity
{
public:
    PopEntity();
    virtual ~PopEntity();
public:
    PopEntity(quint32 lenght);
    PopEntity(Genotype *genotype);
    PopEntity(PopEntity *popEntity);
    virtual bool operator< (PopEntity *rEntity);

    void setFitness(qreal fitness){
        this->fitness = fitness;
    }

    qreal getFitness();
    virtual Genotype *getGenotype();
    virtual PopEntity *getCopy();

protected:
    Genotype *genotype;
    RandomGenerator *randomGenerator;
    qreal fitness;
};

#endif // POPENTITY_H
