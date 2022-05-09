#include "elitismwithrandom.h"

ElitismWithRandom::ElitismWithRandom():
    EntitySelection()
{
}
inline bool entityLesserThan(PopEntity* a, PopEntity* b){
    return a->getFitness() < b->getFitness();
}
vector<PopEntity *> ElitismWithRandom::selectEntities(quint32 quantity, vector<PopEntity *> entityList)
{
    vector<PopEntity *> selectedEntities;
    sort(entityList.begin(), entityList.end(), entityLesserThan);

    for(quint32 i=entityList.size()-1; i > entityList.size()-quantity; i--){
        selectedEntities.push_back(entityList.at(i));
    }

    int pos = RandomGenerator::getGeneratorInstance()->random(0,entityList.size()-1);
    selectedEntities.push_back(entityList.at(pos));

    return selectedEntities;

}
