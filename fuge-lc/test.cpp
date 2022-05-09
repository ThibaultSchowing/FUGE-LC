#include "test.h"

#include <QDebug>
#include <math.h>
#include <iostream>

Test::Test(Population* left, Population* right, quint32 generationCount, QObject *parent) :
    QThread(parent), EvolutionEngine(left, generationCount)
{
 // PRODUCER_CONSUMER  LIMITE DIFFERENCE ON GENERATION BETWEEN thread 1 and thread 2.
    this->left = left;
    this->right = right;
}



void Test::run(){
   startEvolution(generationCount,
                  getEntitySelectors().at(0),(quint32)left->getSize()*0.1f,
                  getEntitySelectors().at(0),left->getSize()-((quint32)left->getSize()*0.1f),
                  getMutationMethods().at(0),0.0001,
                  getCrossoverMethods().at(0),0.001);
}

void Test::evaluatePopulation(Population* population){

    //Test with simply fuzzy system : x20bit y20bit.

    //Evaluate our population with the other cooperators(elites)
    vector<PopEntity *> leftPopEntities = this->left->getAllEntities();
    vector<PopEntity *> RightRepresentative = right->getRepresentatives();

    vector<PopEntity *>::iterator itLeftPop, itRepresentative;
    for(itLeftPop=leftPopEntities.begin();itLeftPop!=leftPopEntities.end(); itLeftPop++)
    {
        //Fitness = left * right / 70 (coded on 3 bits 2³ = 8)
        QBitArray ldata = (*itLeftPop)->getGenotype()->getData();
        quint32 lvalue = 0;
        for(quint32 i = 0; i < (*itLeftPop)->getGenotype()->getLength(); i++)
        {
            lvalue <<= 1;
            lvalue += (quint32)ldata.at(i);

        }

        // Compare with the other population.
        for(itRepresentative=RightRepresentative.begin(); itRepresentative!=RightRepresentative.end(); itRepresentative++)
        {
            //Fitness = left * right / 70 (coded on 3 bits 2³ = 8)
            QBitArray rdata = (*itRepresentative)->getGenotype()->getData();
            quint32 rvalue = 0;
            for(quint32 i = 0; i < (*itRepresentative)->getGenotype()->getLength(); i++)
            {
                rvalue <<= 1;
                rvalue += (quint32)rdata.at(i);
            }
            qreal fitness = (lvalue*rvalue)/ pow(pow(2,(*itRepresentative)->getGenotype()->getLength()),2);

            if((*itLeftPop)->getFitness() < fitness)
                (*itLeftPop)->setFitness(fitness);
        }

        // DEBUG DISPLAY
        QString genotypeString;
        for(int m = 0; m < ldata.size(); m++){
            if(ldata.at(m))
                genotypeString.append("1");
            else
                genotypeString.append("0");
        }
        qDebug() << "Entity (" << *itLeftPop <<") " << " Fit: " << (*itLeftPop)->getFitness() << " genotype : " << genotypeString;
    }

}
