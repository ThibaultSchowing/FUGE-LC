#include "elitism.h"

Elitism::Elitism() :
    EntitySelection()
{
}

inline bool entityLesserThan(PopEntity* a, PopEntity* b){
    return a->getFitness() < b->getFitness();
}

vector<PopEntity *> Elitism::selectEntities(quint32 quantity, vector<PopEntity *> entityList)
{
    vector<PopEntity *> selectedEntities;
    sort(entityList.begin(), entityList.end(), entityLesserThan);

    for(quint32 i=entityList.size()-1; i > entityList.size()-1-quantity; i--){
        selectedEntities.push_back(entityList.at(i));
    }

    return selectedEntities;

}
