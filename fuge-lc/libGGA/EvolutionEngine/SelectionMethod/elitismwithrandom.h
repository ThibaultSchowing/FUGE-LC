#ifndef ELITISMWITHRANDOM_H
#define ELITISMWITHRANDOM_H

#include "entityselection.h"

class ElitismWithRandom : public EntitySelection
{
public:
    ElitismWithRandom();
    vector<PopEntity *> selectEntities(quint32 quantity, vector<PopEntity *> entityList);
};

#endif // ELITISMWITHRANDOM_H
