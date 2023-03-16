#include "winnertakesall.h"

WinnerTakesAll::WinnerTakesAll()
{
}

WinnerTakesAll::~WinnerTakesAll(){

}

void WinnerTakesAll::executeLearn(FuzzyVariable **inVars, int nbInVars, FuzzyVariable **outVars, int nbOutVars, FuzzyRule **rules, int nbRules, float learningFactor){

    // Find the output variable that is the most responsible
    // for the output error
    float error = 0.0;
    int worstOutVar = -1;
    for(int i = 0; i < nbOutVars; i++){
        if(outVars[i]->getError() > error){
            error = outVars[i]->getError();
            worstOutVar = i;
        }
    }

    // Find the rule most contributing to the error which is using
    // to the worst ouput variable
    float ruleActivation = 0.0;
    int mostContributingRule = -1;
    for(int i = 0; i < nbRules; i++){
        if(rules[i]->getUsedOutVars()->contains(worstOutVar)){
            if(rules[i]->getActivation() > ruleActivation){
                ruleActivation = rules[i]->getActivation();
                mostContributingRule = i;
            }
        }
    }

    // Find the lowest or highest input variable in the rule.
    int varToMove = -1;
    float varActivation;
    if(error > 0.0 && mostContributingRule >= 0){
        // our prediction is too much on the right (Predict > RealValue)
        varActivation = 0.0;
        for(int i = 0; i < rules[mostContributingRule]->getNbInPairs(); i++){
            float currentVarAct = rules[mostContributingRule]->getInVarAtPos(i)->getActivation();
            if(currentVarAct > varActivation){
                varActivation = currentVarAct;
                varToMove = i;
            }
        }
    }else if (mostContributingRule >= 0){
        // our prediction is too much on the left. (Predict < RealValue)
        varActivation = 2;
        for(int i = 0; i < rules[mostContributingRule]->getNbInPairs(); i++){
            float currentVarAct = rules[mostContributingRule]->getInVarAtPos(i)->getActivation();
            if( currentVarAct < varActivation && currentVarAct > 0.0){
                varActivation = currentVarAct;
                varToMove = i;
            }
        }
    }

    // Apply learning to inputs :
    // Move the input winner variable to a closer position
    if(varToMove >= 0){

        double oldInputValue = rules[mostContributingRule]->getInVarAtPos(varToMove)->getInputValue();
        rules[mostContributingRule]->getInVarAtPos(varToMove)->setInputValue( oldInputValue +
                    (error*learningFactor));

        qDebug() << "inputValue " << oldInputValue;
        qDebug() << "error " << error;
    }

    // Apply learning to outputs :
    // Move the output variable to a closer position
    //outVarArray[i]->getSet(l)->setPosition(posVector.at(l)+valMin);

    if(worstOutVar >= 0){

        // search what output point to move related to predicted value.
        for(int i = 0; i < outVars[worstOutVar]->getSetsCount(); i++){

            // TODO : move the correct output points close to the real dataset value.
            // To make the predicted become closer to the real value.
            if(outVars[worstOutVar]->getSet(i)->getPosition() + error == outVars[worstOutVar]->getDataSetRealOutput()){
                qDebug() << "IT HAPPENED";
                //outVars[worstOutVar]->getSet(i)->setPosition();
            }
        }

    }



}
