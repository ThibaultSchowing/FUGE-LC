#include "toggling.h"

Toggling::Toggling()
{
}

void Toggling::mutateEntity(PopEntity *entity, qreal mutationPerBitProbability)
{
    // TODO : DONE: Check if not too CPU consuming : Care of per bit mutation probability
    QBitArray *genotypeData = entity->getGenotype()->getData();

    if(mutationPerBitProbability != 0){
        qreal luck = 0.0;
        for(int i=0; i < genotypeData->size(); i++){
            luck = RandomGenerator::getGeneratorInstance()->randomReal(0,1);
            if(luck < mutationPerBitProbability){
                genotypeData->toggleBit(i);
            }
        }
    }else{
        quint32 togglingPos = RandomGenerator::getGeneratorInstance()->random(0,genotypeData->size()-1);
        genotypeData->toggleBit(togglingPos);
    }

}
