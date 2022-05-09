#ifndef ELITISM_H
#define ELITISM_H

#include <QDebug>

#include "entityselection.h"

class Elitism : public EntitySelection
{
public:
    Elitism();
    vector<PopEntity *> selectEntities(quint32 quantity, vector<PopEntity *> entityList);
};

#endif // ELITISM_H
