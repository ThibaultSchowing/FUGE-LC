#ifndef RANKBASEDSELECTION_H
#define RANKBASEDSELECTION_H

#include "entityselection.h"

class RankBasedSelection : public EntitySelection
{
public:
    RankBasedSelection();

    vector<PopEntity *> selectEntities(quint32 quantity, vector<PopEntity *> entityList);
};

#endif // RANKBASEDSELECTION_H
