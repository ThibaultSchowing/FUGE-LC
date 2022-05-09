#include "rankbasedselection.h"

RankBasedSelection::RankBasedSelection() : EntitySelection()
{
}

vector<PopEntity *> RankBasedSelection::selectEntities(quint32 quantity, vector<PopEntity *> entityList)
{
    // THERE IS A SMALL CHANCE, that we get multiple copy of the same individual.

    vector<PopEntity *> selectedEntities;

    PopEntity *tempEntity;
    quint32 pos;
    qreal bestfitness;
    qreal fitness;
    for(quint32 q = 0; q < quantity; q++){
        bestfitness = 0.0;
        for(quint32 i = 0; i < entityList.size()/10; i++){
            pos = RandomGenerator::getGeneratorInstance()->random(0,entityList.size()-1);
            tempEntity = entityList.at(pos);
            fitness = tempEntity->getFitness();

            if(fitness > bestfitness){
                bestfitness = fitness;
                tempEntity = entityList.at(pos);
            }
        }
        selectedEntities.push_back(tempEntity);
    }

    return selectedEntities;

}
