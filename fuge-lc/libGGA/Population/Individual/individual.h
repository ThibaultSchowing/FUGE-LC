#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <Qt>
#include "popentity.h"

using namespace std;
class Individual : PopEntity
{
public:
    Individual(quint32 lenght);
    Individual(Genotype *genotype);
    Individual(Individual *individual);
};

#endif // INDIVIDUAL_H
