#include "statisticengine.h"

StatisticEngine::StatisticEngine() : minfitness(0.0), maxfitness(0.0), meanfitness(0.0), standardDeviation(0.0)
{
}
void StatisticEngine::addFitness(qreal fitness){
    if(fitness > maxfitness)
        maxfitness = fitness;
    if(fitness < minfitness || minfitness == 0.0)
        minfitness = fitness;

    fitnesslist.append(fitness);
    meanfitness += fitness;
}
void StatisticEngine::buildStats(){

    meanfitness /= (qreal)fitnesslist.size();

    qreal temp;
    for(int i = 0; i < fitnesslist.size(); i++){
        temp = fitnesslist.at(i) -  meanfitness;
        standardDeviation += (temp*temp);
    }
    standardDeviation /= (qreal)fitnesslist.size();
    standardDeviation = sqrt(standardDeviation);

}
void StatisticEngine::reset(){
    meanfitness = 0.0;
    standardDeviation = 0.0;
    minfitness = 0.0;
    maxfitness = 0.0;
    fitnesslist.clear();
}

qreal StatisticEngine::getMinFitness(){
    return minfitness;
}

qreal StatisticEngine::getMaxFitness(){
    return maxfitness;
}

qreal StatisticEngine::getMeanFitness(){
    return meanfitness;
}

qreal StatisticEngine::getStandardDeviation(){
    return standardDeviation;
}
