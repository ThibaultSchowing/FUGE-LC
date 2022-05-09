#ifndef TOGGLING_H
#define TOGGLING_H

#include "mutate.h"
#include "randomgenerator.h"
#include <QDebug>

class Toggling : public Mutate
{
public:
    Toggling();
    void mutateEntity(PopEntity *entity, qreal mutationPerBitProbability);
};

#endif // TOGGLING_H
