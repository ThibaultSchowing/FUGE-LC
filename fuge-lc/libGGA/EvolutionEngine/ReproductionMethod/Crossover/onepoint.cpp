#include "onepoint.h"

OnePoint::OnePoint() : Crossover()
{
}

void OnePoint::reproducePairOf(vector<PopEntity *> pairOfEntityList, qreal probability){

    for(quint32 i = 0; i < pairOfEntityList.size()-1; i+=2)
    {
        int j=i+1; // Second entity
        qreal entitiyLuck = RandomGenerator::getGeneratorInstance()->randomReal(0,1);

        //  if goal < chance OK.
        if(entitiyLuck < probability){
            // Never exchange the whole genotype, must be at least 1bit of the other part.
            QBitArray *temp = pairOfEntityList.at(i)->getGenotype()->getDataCopy();
            quint32 cutPoint = RandomGenerator::getGeneratorInstance()->random(1,temp->size()-2);

            for(quint32 k = cutPoint; k < pairOfEntityList.at(i)->getGenotype()->getLength()-1; k++)
            {
                //G2 cut go to temp
                temp->setBit(k,pairOfEntityList.at(j)->getGenotype()->getData()->at(k));
                //G1 cut go to G2
                pairOfEntityList.at(j)->getGenotype()->getData()->setBit(k, pairOfEntityList.at(i)->getGenotype()->getData()->at(k));
                //temp cut go to G1
                pairOfEntityList.at(i)->getGenotype()->getData()->setBit(k, temp->at(k));
            }
            delete temp;
        }

    }
}
