#include "learningengine.h"

LearningEngine::LearningEngine() : learningMethod(0), learningFactor(0.0)
{
    resetLearningEngineInformation();
}

LearningEngine::LearningEngine(LearningMethod *learningMethod, float learningFactor):
    learningMethod(learningMethod), learningFactor(learningFactor)
{

}
LearningEngine::~LearningEngine(){
    if(learningMethod)
        delete learningMethod;
}

void LearningEngine::setLearningMethod(LearningMethod *learningMethod){
    if(this->learningMethod)
        delete learningMethod;
    this->learningMethod = learningMethod;
}

void LearningEngine::setInOutVars(FuzzyVariable** inVars, int nbInVars,
                  FuzzyVariable** outVars, int nbOutVars){
    this->inVars = inVars;
    this->nbInVars = nbInVars;
    this->outVars = outVars;
    this->nbOutVars = nbOutVars;
}

void LearningEngine::setRules(FuzzyRule** rules, int nbRules){
    this->rules = rules;
    this->nbRules = nbRules;
}

void LearningEngine::resetLearningEngineInformation(){
    inVars = 0;
    nbInVars = 0;
    outVars = 0;
    nbOutVars = 0;
    rules = 0;
}

void LearningEngine::learn(){
    learningMethod->executeLearn(inVars,nbInVars,outVars,nbOutVars,
                                 rules, nbRules, learningFactor);
}
