/**
  * @file   fuzzysystem.cpp
  * @author Jean-Philippe Meylan <jean-philippe.meylan_at_heig-vd.ch>
  * @author ReDS (Reconfigurable and embedded digital systems) <www.reds.ch>
  * @author HEIG-VD (Haute école d'ingénierie et de gestion) <www.heig-vd.ch>
  * @date   07.2009
  * @section LICENSE
  *
  * This application is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
  *
  * @class FuzzySystem
  *
  * @brief This class represent a complete fuzzy system and holds also the data to be processed
  * by the system. Once the system is created, the rules and the memberships functions must
  * loaded before making an evaluation with the data.
  */

#include "fuzzysystem.h"
// TODO: Refractoring or even better start a new fuzzysystem project.
// This class is 1800~ lines long.
// FIXME : URGENT this class does not support negative outputs ! due to -1 being declared as "don't care" for rules / out vars.

#define VAL_MAX 1000000.0
#define VAL_MIN 0.0
#define DEFAULT_SET 0
#define MAX_ADM 0.71428

/**
  * Constructor.
  *
  */
FuzzySystem::FuzzySystem() : learningEngine(0)
{
    membershipsLoaded = false;
    rulesLoaded = false;
    dataLoaded = false;
    varUniverseArray = NULL;
    outMinMax = NULL;
    fitness = 0.0;
    sensitivity = 0.0;
    specificity = 0.0;
    accuracy = 0.0;
    ppv = 0.0;
    rmse = 0.0;
    rrse = 0.0;
    rae = 0.0;
    mse = 0.0;
    distanceThreshold = 0.0;
    distanceMinThreshold = 0.0;
    dontCare = 0.0;
    overLearn = 0.0;
}

/**
  * Destructor.
  */
FuzzySystem::~FuzzySystem()
{
    if(learningEngine)
        delete learningEngine;

    // TODO : trouver un bon moyen de supprimer les règles
    // Add variables
    for (int i = 0; i < nbInVars; i++) {
        for (int k = inVarArray[i]->getSetsCount() - 1 ; k > -1  ; k--) {
            inVarArray[i]->removeSet(k);
        }
        delete inVarArray[i];
    }
    delete[] inVarArray;

    for (int i = 0; i < nbOutVars; i++) {
        for (int k = outVarArray[i]->getSetsCount() - 1 ; k > -1  ; k--) {
            outVarArray[i]->removeSet(k);
        }
        delete outVarArray[i];
    }
    delete[] outVarArray;

    if (dataLoaded) {
        // Delete the results array
        for (int i = 0; i < nbOutVars; i++) {
            delete results[i];
        }
        delete[] results;
    }

    // Delete the rules
    for (int i = 0; i < nbRules ; i++) {
        delete rulesArray[i];
    }
    delete[] rulesArray;

    if (!outMinMax == NULL)
        //Delete min and max of each out
        for (int i = 0; i < nbOutVars ; i++) {
            delete outMinMax[i];
        }
    delete[] outMinMax;

    if (dataLoaded && varUniverseArray != NULL) {
        // Delete the universe bounds array
        delete[] varUniverseArray;
    }

}


// FIXME: Could and should be done trough the constructor, this vars must and are only set once.
// The object is not complete.
/**
  * Set the parameters defining the fuzzy system.
  *
  * @param nbRules Number of rules in the system.
  * @param nbVarPerRule Maximum number of input variables per rule.
  * @param nbOutVars Number of output variables.
  * @param nbInSets Number of input sets.
  * @param nbOutSets Number of output sets.
  * @param inVarsCodeSize Number of bits used to code an input variable.
  * @param outVarsCodeSize Number of bits used to code an output variable.
  * @param inSetsCodeSize Number of bits used to code an input set.
  * @param outSetsCodeSize Number of bits used to code an output set.
  * @param inSetsPosCodeSize Number of bits used to code a position for the input sets.
  * @param outSetsPosCodeSize Number of bits used to code a position for the output sets.
  */
void FuzzySystem::setParameters(int nbRules, int nbVarPerRule, int nbOutVars, int nbInSets, int nbOutSets, int inVarsCodeSize,
                                int outVarsCodeSize, int inSetsCodeSize, int outSetsCodeSize, int inSetsPosCodeSize, int outSetsPosCodeSize)
{

    // Retrieve the system parameters
    this->nbRules = nbRules;
    this->nbVarPerRule = nbVarPerRule;
    this->nbVars = nbVars;
    this->nbOutVars = nbOutVars;
    this->nbInSets = nbInSets;
    this->nbOutSets = nbOutSets;
    this->inVarsCodeSize = inVarsCodeSize;
    this->outVarsCodeSize = outVarsCodeSize;
    this->inSetsCodeSize = inSetsCodeSize;
    this->outSetsCodeSize = outSetsCodeSize;
    this->inSetsPosCodeSize = inSetsPosCodeSize;
    this->outSetsPosCodeSize = outSetsPosCodeSize;

    // Create an empty rules array
    rulesArray = new FuzzyRule*[nbRules];
    for (int i = 0; i < nbRules; i++)
        rulesArray[i] = NULL;
    // Create an empty default rule array
    defaultRulesSets.resize(nbOutVars);

    // Create arrRuleFired and arrRuleAlone
    /*
    arrRuleFired = new int[nbRules];
    arrRuleAlone = new int[nbRules];
    for(int i = 0; i < nbRules; i++)
    {
        arrRuleFired[i] = 0;
        arrRuleAlone[i] = 0;
    }
    */
}

/**
  * Loads a dataset for the fuzzy system evaluation.
  *
  * @param systemData Dataset
  */
void FuzzySystem::loadData(QList<QStringList>* systemData)
{
    SystemParameters& sysParams = SystemParameters::getInstance();

    // Retrieve the system data
    this->systemData = systemData;

    //Put list of var in a hash map
    QStringList lstVar = systemData->at(0);
    const int size = lstVar.size();
    for (int i = 1; i < size; i++) {
        hashVar[lstVar.at(i)] = i;
    }

    //this array will contain min and max of each out
    outMinMax = new double*[nbOutVars];
    for( int i = 0; i < nbOutVars; i++ )
    {
        outMinMax[i] = new double[2];
        outMinMax[i][0] = VAL_MAX;
        outMinMax[i][1] = 0;
    }

    // No fuzzy system has been loaded from a file
    if (!(membershipsLoaded && rulesLoaded)) {

        // Retrieve the number of variables (in+out)
        nbVars = systemData->at(0).count() - 1;
        nbInVars = nbVars - nbOutVars;
        sysParams.setNbInVars(nbInVars);

        // Create the variables arrays from the systemData information
        inVarArray  = new FuzzyVariable*[nbInVars];
        outVarArray = new FuzzyVariable*[nbOutVars];

        for (int i = 0; i < nbInVars; i++) {
            inVarArray[i] = new FuzzyVariable(systemData->at(0).at(i+1), coco);
            for (int l = 0; l < nbInSets; l++) {
                FuzzySet* fSet = new FuzzySet("MF "+QString::number(l), 0, l);
                inVarArray[i]->addSet(fSet);
            }
        }
        for (int i = nbInVars, k = 0; i < nbInVars+nbOutVars; i++, k++) {
            outVarArray[k] = new FuzzyVariable(systemData->at(0).at(i+1), singleton/*coco*/);
            // Set the output flag
            outVarArray[k]->setOutput(true);
            for (int l = 0; l < nbOutSets; l++) {
                FuzzySet* fSet = new FuzzySet("MF "+QString::number(l), 0, l);
                outVarArray[k]->addSet(fSet);
            }
        }

        // Create the results array from the systemData information
        nbSamples = systemData->size()-1;
        results = new float*[nbOutVars];
        for (int i = 0; i < nbOutVars; i++) {
            results[i] = new float [nbSamples];
        }
        for (int i = 0; i < nbOutVars; i++) {
            for (int k = 0; k < nbSamples; k++) {
                results[i][k] = systemData->at(k+1).at(nbInVars+1 + i).toDouble();
                if (results[i][k] < outMinMax[i][0])
                {
                    outMinMax[i][0] = results[i][k];
                }
                if (results[i][k] > outMinMax[i][1])
                {
                    outMinMax[i][1] = results[i][k];
                }
            }
        }
        // Create the array containing the size of the universe of discourse
        varUniverseArray = new universeBounds[nbVars];
        // Detect the universe of discourse for all variables
        detectVarUniverses(varUniverseArray);
    }

    // The fuzzy system has been loaded from a file
    else {
        // Create the results array from the systemData information
        nbSamples = systemData->size()-1;
        results = new float*[nbOutVars];
        for (int i = 0; i < nbOutVars; i++) {
            results[i] = new float [nbSamples];
        }

        for (int i = 0; i < nbOutVars; i++) {
            for (int k = 0; k < nbSamples; k++) {
                results[i][k] = systemData->at(k+1).at(systemData->at(0).count() - 1 - nbOutVars +1 + i).toDouble();
                if (results[i][k] < outMinMax[i][0])
                {
                    outMinMax[i][0] = results[i][k];
                }

                if (results[i][k] > outMinMax[i][1])
                {
                    outMinMax[i][1] = results[i][k];
                }
            }
        }
    }

    dataLoaded = true;
}

/**
  * Resets completely the fuzzy system.
  */
void FuzzySystem::reset()
{

    fitness = 0.0;
    sensitivity = 0.0;
    specificity = 0.0;
    accuracy = 0.0;
    ppv = 0.0;
    rmse = 0.0;
    rrse = 0.0;
    rae = 0.0;
    mse = 0.0;
    distanceThreshold = 0.0;
    distanceMinThreshold = 0.0;
    dontCare = 0.0;
    overLearn = 0.0;

    // Clear the description
    systemDescription.clear();

    defuzzValues.clear();
    threshValues.clear();
    computedResults.clear();

    membershipsLoaded = false;
    rulesLoaded = false;

    // Delete membership functions
    for (int i = 0; i < nbInVars; i++) {
        // Set the variables as no more used by the system
        inVarArray[i]->setUsedBySystem(false);
    }
    for (int i = 0; i < nbOutVars; i++) {
    }

    // Delete the rules
    for (int i = 0; i < nbRules ; i++) {
        if (rulesArray[i] != NULL) {
            delete rulesArray[i];
            rulesArray[i] = NULL;
        }
    }
}

void FuzzySystem::detectVarUniverses(universeBounds* varUniArray)
{

    float valMin = VAL_MAX;
    float valMax = VAL_MIN;
    float value = 0.0;

    for (int i = 0; i < nbVars; i++) {
        for (int k = 0; k < nbSamples; k++) {
            value = systemData->at(k+1).at(i+1).toFloat();
            if (value <= valMin)
                valMin = value;
            if (value >= valMax)
                valMax = value;
        }
        varUniArray[i].valMax = valMax;
        varUniArray[i].valMin = valMin;
        qDebug() << "varUniArray["<<i<<"].valMax = " << valMax;
        qDebug() << "varUniArray["<<i<<"].valMin = " << valMin;
        valMin = VAL_MAX;
        valMax = VAL_MIN;


    }
}

/**
  * Create the rules contained in a rule genome array.
  *
  * @param ruleGenArray Array containing the different rule genomes to be created.
  */
void FuzzySystem::loadRulesGenome(FuzzyRuleGenome** ruleGenArray, int* defRulesSets)
{

    for (int i = 0; i <  nbRules; i++) {
        // Create the rule
        rulesArray[i] = new FuzzyRule(inVarArray, outVarArray, ruleGenArray[i]);
        // Update the system description
        systemDescription.append(rulesArray[i]->getDescription());
        systemDescription.append("\n");
    }
    // Decode the default rules
    int val = 0;
    for (int i = 0; i < nbOutVars; i++) {
        val = 0;
        for (int k = 0; k < outSetsCodeSize; k++) {
            val += defRulesSets[i*outSetsCodeSize+k] << k;
        }
        // Verify that the set value encoded is valid, otherwise we set it to 0
        if (val >= nbOutSets)
            val = 0;
        defaultRulesSets.replace(i, val);
    }


    // Add the default rule to the system description
    systemDescription.append(" ELSE : ");
    for (int i = 0; i < nbOutVars; i++) {
        systemDescription.append(outVarArray[i]->getName());
        systemDescription.append(" is ");
        systemDescription.append(QString::number(defaultRulesSets.at(i)));
        systemDescription.append("  ");
    }

    // Update the system description with the membership functions
    // of the variables used in the rules
    systemDescription.append("\n\nMembership functions : \n");
    for (int i = 0; i  < nbInVars; i++) {
        if (inVarArray[i]->isUsedBySystem()) {
            systemDescription.append(inVarArray[i]->getName());
            systemDescription.append(" (");
            for (int k = 0; k < inVarArray[i]->getSetsCount(); k++) {
                systemDescription.append(QString::number(inVarArray[i]->getSet(k)->getPosition()));
                if (k != inVarArray[i]->getSetsCount() - 1)
                    systemDescription.append(" , ");
            }
            systemDescription.append(") ; ");
        }
    }
    for (int i = 0; i  < nbOutVars; i++) {
        systemDescription.append(outVarArray[i]->getName());
        systemDescription.append(" (");
        for (int k = 0; k < outVarArray[i]->getSetsCount(); k++) {
            systemDescription.append(QString::number(outVarArray[i]->getSet(k)->getPosition()));
            if (k != outVarArray[i]->getSetsCount() - 1)
                systemDescription.append(" , ");
        }
        systemDescription.append(")");
        if (i != nbOutVars-1)
            systemDescription.append(" ; ");
        else
            systemDescription.append("\n");
    }

    rulesLoaded = true;
}

void FuzzySystem::updateSystemDescription()
{

    // Clear the previous description
    systemDescription.clear();

    // Add the rules to the system description
    for (int i = 0; i <  nbRules; i++) {
        systemDescription.append(rulesArray[i]->getDescription());
        systemDescription.append("\n");
    }
    // Add the default rule to the system description
    systemDescription.append(" ELSE : ");
    for (int i = 0; i < nbOutVars; i++) {
        systemDescription.append(outVarArray[i]->getName());
        systemDescription.append(" is ");
        systemDescription.append(QString::number(defaultRulesSets.at(i)));
        systemDescription.append("  ");
    }
    // Update the system description with the membership functions
    // of the variables used in the rules
    systemDescription.append("\n\nMembership functions : \n");
    for (int i = 0; i  < nbInVars; i++) {
        if (inVarArray[i]->isUsedBySystem()) {
            systemDescription.append(inVarArray[i]->getName());
            systemDescription.append(" (");
            for (int k = 0; k < inVarArray[i]->getSetsCount(); k++) {
                systemDescription.append(QString::number(inVarArray[i]->getSet(k)->getPosition()));
                if (k != inVarArray[i]->getSetsCount() - 1)
                    systemDescription.append(" , ");
            }
            systemDescription.append(") / ");
        }
    }
    for (int i = 0; i  < nbOutVars; i++) {
        systemDescription.append(outVarArray[i]->getName());
        systemDescription.append(" (");
        for (int k = 0; k < outVarArray[i]->getSetsCount(); k++) {
            systemDescription.append(QString::number(outVarArray[i]->getSet(k)->getPosition()));
            if (k != outVarArray[i]->getSetsCount() - 1)
                systemDescription.append(" , ");
        }
        systemDescription.append(")");
        if (i != nbOutVars-1)
            systemDescription.append(" / ");
        else
            systemDescription.append("\n");
    }
}


// FIXME : Do not use index for learning methods, but the method itself instead.
// (Provided by classes derivated from FuzzyLearningMethod.)
/**
  * Create the memberships functions contained in a membership functions genome.
  *
  * @param membGen Genome containing the different memberships funcions to be created.
  * @param indexInitMethod Index of the learning method.(ui based)
  */
void FuzzySystem::loadMembershipsGenome(FuzzyMembershipsGenome* membGen, int indexInitMethod)
{

    float position = 0.0;
    float step = 0.0;
    float valMin = 0.0;
    float diff = 0.0;

    QVector<float> posVector;

    // Loop through all input variables
    for (int i = 0; i  < nbInVars; i++) {
        valMin = varUniverseArray[i].valMin;
        diff = varUniverseArray[i].valMax - valMin;
        step = diff / (pow(2, membGen->getInSetsPosCodeSize())-1);
        // Loop through all sets of the current variable
        for (int k = 0; k < membGen->getNbInSets(); k++) {
            // Retrieve the position encoded in the genome

            // Initialize the fuzzy system depending on the method selected.
            if(indexInitMethod == 2)
                position = ((float) membGen->getInParam(i,k) * step);
            else if(indexInitMethod == 0)
                position = ((float) (k+1)*(diff/(membGen->getNbInSets()+1.0)));
            else if(indexInitMethod == 1)
                position = ((float) RandomGenerator::getGeneratorInstance()->randomReal(valMin,varUniverseArray[i].valMax));
            posVector.append(position);
        }
        // Sort the vector
        qSort(posVector);
        // Add the sets to the variable
        for (int l = 0; l < posVector.size(); l++) {
            inVarArray[i]->getSet(l)->setPosition(posVector.at(l)+valMin);
            //qDebug() << "inVarArray["<<i<<"]"<< "posvector.at("<<l<<") + valMin & diff = " << posVector.at(l) << "+" << valMin << " & " << diff <<  " VALMAX : " << varUniverseArray[i].valMax;
        }
        posVector.clear();
    }

    // Loop through all output variables
    for (int i = 0; i  < nbOutVars; i++) {
        valMin = varUniverseArray[i+nbInVars].valMin;
        diff = varUniverseArray[i+nbInVars].valMax - valMin;
        step = diff / (pow(2, membGen->getOutSetsPosCodeSize())-1);
        // Loop through all sets of the current variable
        for (int k = 0; k < membGen->getNbOutSets(); k++) {
            // Retrieve the position encoded in the genome
            if(indexInitMethod == 2)
                position = ((float) membGen->getOutParam(i,k) * step);
            else if(indexInitMethod == 0)
                position = ((float) (k+1)*(diff/(membGen->getNbOutSets()+1.0)));
            else if(indexInitMethod == 1)
                position = ((float) RandomGenerator::getGeneratorInstance()->randomReal(valMin,varUniverseArray[i+nbInVars].valMax));
            posVector.append(position);
        }
        // Sort the vector
        qSort(posVector);
        // Add the sets to the variable
        for (int l = 0; l < posVector.size(); l++) {
            outVarArray[i]->getSet(l)->setPosition(posVector.at(l)+valMin);
        }
        posVector.clear();
    }

    membershipsLoaded = true;
}

float FuzzySystem::threshold(int outVar, float value)
{

    /// TODO : ne pas nécessairement retourner 0 ou 1
    SystemParameters& sysParams = SystemParameters::getInstance();

    if (sysParams.getThreshActivated()) {
        if (value >= sysParams.getThresholdVal(outVar))
            value = 1.0;
        else if (value >= 0.0)
            value = 0.0;
        else
            value = -1.0;
    }

    //qDebug("value = %f", value);

    return value;
}
//Idk if this function is still usefull
int FuzzySystem::getVarInDataset(const QString name)
{

    return hashVar.value(name);
}

void FuzzySystem::evaluateSample(int sampleNum)
{

    assert(sampleNum >= 0 && sampleNum < nbSamples);
    float maxFiredRule[nbOutVars];

    // Clean the previous evaluation values in the output variables sets
    for (int i = 0; i < nbOutVars; i++) {
        const int setCount = outVarArray[i]->getSetsCount();
        for (int k = 0; k < setCount; k++) {
            outVarArray[i]->getSet(k)->clearEval();
        }
        maxFiredRule[i] = 0.0;
        // Initialise the defuzz array values to -1
        defuzzValues.replace(i, -1.0);
    }

    // Set the input values
    for (int i = 0; i < nbInVars; i++) {
        if (inVarArray[i]->isUsedBySystem()) {

            bool isOk;
            float value = systemData->at(sampleNum+1).at(hashVar.value(inVarArray[i]->getName())).toFloat(&isOk);

            // Value is not numeric
            if (isOk == false) {
                //qDebug("missing value at sample num : %d, var : %d", sampleNum, i);
                inVarArray[i]->setMissingVal(true);
            }
            // Value is OK
            else {
                inVarArray[i]->setInputValue(value);
                //qDebug() << "inVarArray [" << i << "] = " << value;
            }
        }
    }

    //Number of rule fired for this sample
    //int nbRuleFired = 0;
    //if nbRuleFired is 1, so ruleFired will be the number of the rule fired
    //int ruleFired = -1;

    //Who's the winner rule
    int winner = -1;
    //int second = -1;
    float winnerFireLvl = 0.0;
    float secondFireLvl = 0.0;
    //Who's the second rule, like this we can compute th distance between the winner and the second.
    //This is usefull for the the overlearn criterra.
    //int secondWinner = -1;

    // Evaluate all rules
    for (int i = 0; i < nbRules; i++) {
        // Evaluate the rule only if it exists
        if (rulesArray[i] != NULL) {
            rulesArray[i]->evaluate();
        }

        //usefull to know if the rule was fired
        float fire = 0.0;

        const QList<int> *usedOutVars = rulesArray[i]->getUsedOutVars();
        const int usedOutVarsSize = usedOutVars->size();

        for (int k = 0; k < usedOutVarsSize; k++) {

            float fireLvl = rulesArray[i]->getFireLevel(k);

            if (fireLvl > maxFiredRule[k]) {
                maxFiredRule[usedOutVars->at(k)] = fireLvl;
            }

            if ( fireLvl > 0.0 ) {
                fire += fireLvl;
            }

            if ( fireLvl > winnerFireLvl )
            {
                //The ex winner become the second
                secondFireLvl = winnerFireLvl;

                //Remplace winner
                winner = i;
                winnerFireLvl = fireLvl;
            }
            else if ( fireLvl > secondFireLvl ) //not winner but beat the second
            {
                secondFireLvl = fireLvl;
            }
        }

        if ( fire >= 0.2 )
        {
            arrRuleFired[i]++;
        }
    }

    //Check the winner rule
    if ( ( winnerFireLvl - secondFireLvl >= 0.2 )  || ( secondFireLvl == 0.0 && winner != -1 ) )
    {
        //arrRuleAlone[ruleFired]++;
        arrRuleWinner[winner]++;
    }

    // Default rule
    for (int i = 0; i < nbOutVars; i++) {
        outVarArray[i]->getSet(defaultRulesSets.at(i))->setEval(1.0-maxFiredRule[i]);
    }

    // Defuzz output variables and apply threshold
    for (int i = 0; i < nbOutVars; i++) {
        defuzzValues.replace(i, outVarArray[i]->defuzz(100));
        if (defuzzValues.at(i) == -1) {
            std::cout << "Error : variable " << i << " defuzzification = -1 !!!" << std::endl;
            throw;
        }
        //int test = threshold(i, defuzzValues.at(i));
        threshValues.replace(i, threshold(i, defuzzValues.at(i)));
        //qDebug("value = %f", defuzzValues.at(i));
    }
}

QVector<float> FuzzySystem::doEvaluateFitness()
{

    fitness = evaluateFitness();

    return computedResults;
}

// FIXME : This function is a mess. Inits, resets, copies of copies of copies of elements and values.
// This needs urgent refractoring.

float FuzzySystem::evaluateFitness()
{
    SystemParameters& sysParams = SystemParameters::getInstance();

    if(learningEngine)
        delete learningEngine;

    if(sysParams.getLearningMethod() == 1){
        learningEngine = new LearningEngine(new WinnerTakesAll(), sysParams.getLearningFactor());
    }else if(sysParams.getLearningMethod() == 2){
        learningEngine = new LearningEngine(new Credits(), sysParams.getLearningFactor());
    }

    fitnessStruct fitVector[nbOutVars];

    for (int i = 0; i < nbOutVars; i++) {
        fitVector[i].tPosCount = 0;
        fitVector[i].tNegCount = 0;
        fitVector[i].fPosCount = 0;
        fitVector[i].fNegCount = 0;
        fitVector[i].sensitivity = 0.0;
        fitVector[i].specificity = 0.0;
        fitVector[i].accuracy = 0.0;
        fitVector[i].ppv = 0.0;
        fitVector[i].rmse = 0.0;
        fitVector[i].rrse = 0.0;
        fitVector[i].rae = 0.0;
        fitVector[i].mse = 0.0;
        fitVector[i].distanceThreshold = 0.0;
        fitVector[i].distanceMinThreshold = 0.0;
        fitVector[i].squareError = 0.0; /* relative square error */
        fitVector[i].rmseError   = 0.0; /* Error for compute RMSE is Sum( Predict - Actual ) */
        fitVector[i].distMinBelow  = VAL_MAX; /* distance minimal to threshold from below  */
        fitVector[i].distMinAbove  = VAL_MAX; /* distance minimal to threshold from above  */
        fitVector[i].sumDistBelow = 0.0; /* used to compute MDM */
        fitVector[i].sumDistAbove = 0.0; /* used to compute MDM */
        fitVector[i].maxActualValue = 0.0;
        fitVector[i].errorSum = 0.0;/* absolute error used to compute RAE */
    }

    //Reset
    this->sensitivity = 0;
    this->specificity = 0;
    this->accuracy = 0;
    this->ppv = 0;
    this->rmse = 0;
    this->rrse = 0;
    this->rae = 0;
    this->mse = 0;
    this->distanceThreshold = 0;
    this->distanceMinThreshold = 0;
    this->dontCare = 0;
    this->overLearn = 0;

    // Ensure that data, rules and memberships are loaded
    assert(dataLoaded && rulesLoaded && membershipsLoaded);

    defuzzValues.resize(nbOutVars);
    threshValues.resize(nbOutVars);
    computedResults.resize(nbSamples*nbOutVars);

    //to compute overLearn
    arrRuleFired = new int[nbRules];
    //arrRuleAlone = new int[nbRules];
    arrRuleWinner = new int[nbRules];
    //arrRuleGrade = new float[nbRules];


    for(int i = 0; i < nbRules; i++)
    {
        arrRuleFired[i] = 0;
        //arrRuleAlone[i] = 0;
        arrRuleWinner[i] = 0;
        //arrRuleGrade[i] = 0.0;
    }

    // Evaluate all samples
    for (int i = 0; i < nbSamples; i++) {

        // In case of learning, we evaluate multiple time the sample and apply the learning engine to our vars.
        for(int j = 0; j < (sysParams.isLearning() ? (sysParams.getLearningEpochs()):(1)); j++){
            evaluateSample(i);

            // Prepare the learning engine if there is one.
            if(learningEngine){
                learningEngine->resetLearningEngineInformation();
                learningEngine->setInOutVars(inVarArray,nbInVars,outVarArray,nbOutVars);
                learningEngine->setRules(rulesArray,nbRules);
            }

            for (int k = 0; k < nbOutVars; k++) {
                const float defuzzedValue = defuzzValues.at(k);
                computedResults.replace(i*nbOutVars + k, defuzzedValue);

                /* Compute regression criterra : RMSE, MSE, RRSE and RAE */
                const float error = defuzzedValue - results[k][i]; /* Predict - Actual */

                //
                outVarArray[k]->setActivation(defuzzedValue);
                outVarArray[k]->setError(error);
                outVarArray[k]->setDataSetRealOutput(results[k][i]);


                if (error != 0.0){
                    const float errorMoy = ( defuzzedValue + results[k][i] ) / 2.0;
                    fitVector[k].squareError += ( error / errorMoy ) * ( error / errorMoy ); /* relative square error */
                    fitVector[k].errorSum    += fabs( error ) / errorMoy;
                    fitVector[k].rmseError   += error * error;
                }

                /* Compute classification criterra : sensi, specy, ppv, accuracy, ADM, MDM */
                const float resTmp = threshold(k, results[k][i]);
                const float thresholdAtK = sysParams.getThresholdVal(k);
                const float threshValueAtK = threshValues.at(k);


                if (threshValueAtK == resTmp && resTmp == 0) { //well classified, below threshold
                    fitVector[k].tNegCount++;

                    const float distThreshBelow = (thresholdAtK - defuzzedValue) / (thresholdAtK - results[k][i]);
                    //distThreshBelow = (distThreshBelow) > 1.0 ? 1.0 : distThreshBelow;// to keep the adm between 0 and 1
                    //fitVector[k].sumDistBelow += distThreshBelow * (1.0-(distThreshBelow-1.0)*(distThreshBelow-1.0)*(distThreshBelow-1.0)*(distThreshBelow-1.0));
                    if (distThreshBelow >= MAX_ADM) {
                        fitVector[k].sumDistBelow +=  1.0;
                    } else {
                        fitVector[k].sumDistBelow += distThreshBelow * ( 2.8 - ( 1.96 * distThreshBelow ) );
                    }

                    // Distance min to threshold from below
                    if( fitVector[k].distMinBelow > distThreshBelow ) {
                        fitVector[k].distMinBelow = distThreshBelow;
                    }
                } else if (threshValueAtK == resTmp && resTmp == 1) { //well classified, above threshold
                    fitVector[k].tPosCount++;

                    const float distThreshAbove = (defuzzedValue - thresholdAtK) / (results[k][i] - thresholdAtK);
                    //distThreshAbove = (distThreshAbove) > 1.0 ? 1.0 : distThreshAbove;
                    //fitVector[k].sumDistAbove += distThreshAbove  * (1.0-(distThreshAbove-1.0)*(distThreshAbove-1.0)*(distThreshAbove-1.0)*(distThreshAbove-1.0));
                    if (distThreshAbove >= MAX_ADM) {
                        fitVector[k].sumDistAbove +=  1.0;
                    } else {
                        fitVector[k].sumDistAbove += distThreshAbove * ( 2.8 - ( 1.96 * distThreshAbove ) );
                    }

                    // Distance min to threshold from above
                    if( fitVector[k].distMinAbove >  distThreshAbove ) {
                        fitVector[k].distMinAbove = distThreshAbove;
                    }
                } else if (threshValueAtK != resTmp && resTmp == 0) { //wrong classified, above threshold
                    fitVector[k].fPosCount++;
                } else if (threshValueAtK != resTmp && resTmp == 1) { // wrong classified, below threshold
                    fitVector[k].fNegCount++;
                }
            }
            if(learningEngine)
                learningEngine->learn();
        }
    }

    // Sum values for the different outputs of each fitness parameter
    for (int l = 0; l < nbOutVars; l++) {
        if ((fitVector[l].tPosCount + fitVector[l].fNegCount) > 0) {
            fitVector[l].sensitivity = (float) fitVector[l].tPosCount / ((float) (fitVector[l].tPosCount + fitVector[l].fNegCount));
        }
        if ((fitVector[l].tNegCount + fitVector[l].fPosCount) > 0) {
            fitVector[l].specificity = (float) fitVector[l].tNegCount / ((float) (fitVector[l].tNegCount + fitVector[l].fPosCount));
        }
        fitVector[l].accuracy = (float) (fitVector[l].tPosCount+fitVector[l].tNegCount) /
                ((float) (fitVector[l].tPosCount+fitVector[l].tNegCount+fitVector[l].fPosCount+fitVector[l].fNegCount));
        if ((fitVector[l].tPosCount+fitVector[l].fPosCount) > 0) {
            fitVector[l].ppv = (float) fitVector[l].tPosCount / ((float) (fitVector[l].tPosCount+fitVector[l].fPosCount));
        }

        //rmse
        fitVector[l].rmse = sqrt( fitVector[l].rmseError / ( nbSamples ) );

        //rrse
        fitVector[l].rrse = sqrt( fitVector[l].squareError / (double) nbSamples );

        //rae
        fitVector[l].rae = fitVector[l].errorSum / ( double ) nbSamples;

        //mse
        fitVector[l].mse = fitVector[l].rmseError / ( nbSamples );

        //Compute mean distance to threshold ADM
        fitVector[l].distanceThreshold = ( (fitVector[l].sumDistBelow / (fitVector[l].tNegCount + fitVector[l].fPosCount))
                                           + (fitVector[l].sumDistAbove / (fitVector[l].tPosCount + fitVector[l].fNegCount)) )
                / 2.0;


        //Compute min distance to threshold MDM
        if ( fitVector[l].distMinBelow == VAL_MAX )
            fitVector[l].distMinBelow = 0;
        if ( fitVector[l].distMinAbove == VAL_MAX )
            fitVector[l].distMinAbove = 0;

        this->sensitivity += fitVector[l].sensitivity;
        this->specificity += fitVector[l].specificity;
        this->accuracy += fitVector[l].accuracy;
        this->ppv += fitVector[l].ppv;
        this->rmse += fitVector[l].rmse;
        this->rrse += fitVector[l].rrse;
        this->rae += fitVector[l].rae;
        this->mse += fitVector[l].mse;
        this->distanceThreshold += fitVector[l].distanceThreshold;
        this->distanceMinThreshold += (fitVector[l].distMinAbove + fitVector[l].distMinBelow) / 2.0;
    }

    // Compute the mean values for the different outputs of each fitness parameter
    this->sensitivity /= nbOutVars;
    this->specificity /= nbOutVars;
    this->accuracy /= nbOutVars;
    this->ppv /= nbOutVars;
    this->rmse /= nbOutVars;
    this->rrse /= nbOutVars;
    this->rae /= nbOutVars;
    this->mse /= nbOutVars;
    this->distanceThreshold /= nbOutVars;
    this->distanceMinThreshold /= nbOutVars;


    //Size (dont care)
    float sumVar = 0.0;
    //Evaluate all rules
    for (int i = 0; i < nbRules; i++) {
        //Evaluate the rule only if it exists
        if (rulesArray[i] != NULL) {
            sumVar += (float)rulesArray[i]->getNbInPairs();
        }
    }

    if( sumVar > 0.0 )
    {
        this->dontCare = 1.0 / sumVar;
    }
    else
    {
        this->dontCare = 0.0;
    }



    //Over learn, the grade is given by fuzzy system

    //Membership function for Firing
    float const mfLow = 0.1; //trapez
    float const mfHigh = 0.5; //trapez

    //Membership function for Winner
    float const mfNever = 0.1; //trapez
    float const mfSometime = 0.4; // triangle
    float const mfAlways = 0.7; //trapez

    int const nbRuleInGeneralityFuzzy = 4;
    float arrTruthLvl[nbRules][nbRuleInGeneralityFuzzy];
    float arrRuleGrade[nbRules];

    for ( int i = 0; i < nbRules; i++ ) {
        arrRuleGrade[i] = 1.0;
        for ( int j = 0; j < nbRuleInGeneralityFuzzy; j++) {
            arrTruthLvl[i][j] = 0.0;
        }
    }

    for( int i = 0; i < nbRules; i++ ) {
        const float firing = (float)arrRuleFired[i] / (float)nbSamples;
        float winner = 0.0;

        if( arrRuleFired[i] != 0 ) {
            winner = (float)arrRuleWinner[i] / (float)arrRuleFired[i];
        }else {
            winner = 0.0;
        }

        //Firing high truth level
        float firingHigh = 0.0; // ( mfHigh - mfLow );
        if ( firing <= mfLow ) {
            firingHigh = 0.0;
        } else if ( firing >= mfHigh ) {
            firingHigh = 1.0;
        } else {
            firingHigh = ( (firing - mfLow) / ( mfHigh - mfLow ) );
        }

        //Firing low truth level
        float firingLow = 0.0;
        if ( firing <= mfLow ) {
            firingLow = 1.0;
        } else if ( firing >= mfHigh ) {
            firingLow = 0.0;
        } else {
            firingLow = (firing - mfLow) / ( mfHigh - mfLow );
        }

        float winnerAlways = 0.0;
        if ( winner <= mfSometime ) {
            winnerAlways = 0.0;
        } else if ( winner >= mfAlways ) {
            winnerAlways = 1.0;
        } else {
            winnerAlways = ( winner - mfSometime ) / ( mfAlways - mfSometime );
        }

        //membership function triangular
        float winnerSometime = 0.0;
        if ( winner <= mfNever ) {
            winnerSometime = 0.0;
        } else if ( winner >= mfAlways) {
            winnerSometime = 0.0;
        } else if ( winner ==  mfSometime ) {
            winnerSometime = 1.0;
        } else if ( winner > mfSometime ) {
            winnerSometime = 1.0 - (winner - mfSometime) / ( mfSometime - mfAlways );
        } else { // in this case winner < mfSometime
            winnerSometime = (winner - mfNever) / ( mfSometime - mfNever );
        }

        float winnerNever = 0.0;
        if ( winner <= mfNever ) {
            winnerNever = 1.0;
        } else if ( winner >= mfSometime ) {
            winnerNever = 0.0;
        } else {
            winnerNever = 1.0 - ( ( winner - mfNever ) / ( mfSometime - mfNever ) );
        }

        //Generality Rule
        arrTruthLvl[i][0] = firingHigh;
        arrTruthLvl[i][1] = std::min( firingLow, winnerNever );
        arrTruthLvl[i][2] = std::min( firingLow, winnerSometime );
        arrTruthLvl[i][3] = std::min( firingLow, winnerAlways );

        const float evalProduct = arrTruthLvl[i][0] * 1.0 +  //high
                arrTruthLvl[i][1] * 0.7 +  //med high
                arrTruthLvl[i][2] * 0.3 +  //med low
                arrTruthLvl[i][3] * 0.0;   //low

        const float evalSum = arrTruthLvl[i][0] +
                arrTruthLvl[i][1] +
                arrTruthLvl[i][2] +
                arrTruthLvl[i][3];

        arrRuleGrade[i] = evalProduct / evalSum;
    }

    float minGrade = 1.0;
    for(int i = 0; i < nbRules; i++) {
        //std::cout << arrRuleGrade[i] << std::endl;
        if ( arrRuleGrade[i] < minGrade ) {
            minGrade = arrRuleGrade[i];
        }
    }

    //this->overLearn = sumAloneOnFired / (float) nbRules ;
    this->overLearn = minGrade;



    float num = sysParams.getSensiW() * sensitivity
            + sysParams.getSpeciW() * specificity
            + sysParams.getAccuracyW() * accuracy
            + sysParams.getPpvW() * ppv
            + sysParams.getRmseW() * pow( 2.0, -rmse )
            + sysParams.getRrseW() * pow( 2.0,-rrse )
            + sysParams.getRaeW() * pow( 2.0,-rae )
            + sysParams.getMseW() * pow( 2.0, -mse )
            //+ sysParams.getDistanceThresholdW() * distanceThreshold
            //+ sysParams.getDistanceMinThresholdW() * distanceMinThreshold
            + sysParams.getDontCareW() * dontCare;
    //+ sysParams.getOverLearnW()* overLearn;

    float denum = sysParams.getSensiW()
            + sysParams.getSpeciW()
            + sysParams.getAccuracyW()
            + sysParams.getPpvW()
            + sysParams.getRmseW()
            + sysParams.getRrseW()
            + sysParams.getRaeW()
            + sysParams.getMseW()
            //+ sysParams.getDistanceThresholdW()
            //+ sysParams.getDistanceMinThresholdW()
            + sysParams.getDontCareW();
    //+ sysParams.getOverLearnW();

    this->fitness = num / denum;

    //TEST
    /*
    for( int i = 0; i < nbRules; i++ )
    {
        std::cout << "Rule " << i << " - Nb Fired : "   << arrRuleFired[i]  << std::endl;
        std::cout << "Rule " << i << " - Nb Winner : "  << arrRuleWinner[i] << std::endl;
        std::cout << "Rule " << i << " - Nb Alone : "   << arrRuleAlone[i]  << std::endl;
    }
    */


    //Delete arrRuleFired and arrRuleAlone
    delete[] arrRuleFired;
    //delete[] arrRuleAlone;
    delete[] arrRuleWinner;
    //delete[] arrRuleGrade;

    // Avoid crash when fitness is 0 or lower
    if (fitness <= 0.0)
        fitness = 0.001;

    return fitness;
}

int FuzzySystem::getNbRules()
{
    return this->nbRules;
}

int FuzzySystem::getNbVarPerRule()
{
    return this->nbVarPerRule;
}

int FuzzySystem::getNbInVars()
{
    return this->nbInVars;
}

int FuzzySystem::getNbOutVars()
{
    return this->nbOutVars;
}

int FuzzySystem::getNbInSets()
{
    return this->nbInSets;
}

int FuzzySystem::getNbOutSets()
{
    return this->nbOutSets;
}

int FuzzySystem::getInVarsCodeSize()
{
    return this->inVarsCodeSize;
}

int FuzzySystem::getOutVarsCodeSize()
{
    return this->outVarsCodeSize;
}

int FuzzySystem::getInSetsCodeSize()
{
    return this->inSetsCodeSize;
}

int FuzzySystem::getOutSetsCodeSize()
{
    return this->outSetsCodeSize;
}

int FuzzySystem::getInSetsPosCodeSize()
{
    return this->inSetsPosCodeSize;
}

int FuzzySystem::getOutSetsPosCodeSize()
{
    return this->outSetsPosCodeSize;
}

int FuzzySystem::getMembershipsBitStringSize()
{
    return (nbInVars*nbInSets*inSetsPosCodeSize + nbOutVars*nbOutSets*outSetsPosCodeSize);
}

int FuzzySystem::getRuleBitStringSize()
{
    SystemParameters& sysParams = SystemParameters::getInstance();

    if (sysParams.getFixedVars())
        return (nbVarPerRule * (inSetsCodeSize) + nbOutVars * (outSetsCodeSize));
    else
        return (nbVarPerRule * (inVarsCodeSize+inSetsCodeSize) + nbOutVars * (outVarsCodeSize+outSetsCodeSize));
}

int FuzzySystem::getRuleMaxBitStringSize()
{
    return (nbVarPerRule * (inVarsCodeSize+inSetsCodeSize) + nbOutVars * (outVarsCodeSize+outSetsCodeSize));
}

int FuzzySystem::getDefaultRulesBitStringSize()
{
    return nbOutVars*outSetsCodeSize;
}

QString FuzzySystem::getSystemDescritpion()
{
    return systemDescription;
}

float FuzzySystem::getFitness()
{
    return fitness;
}

float FuzzySystem::getSensitivity()
{
    return sensitivity;
}

float FuzzySystem::getSpecificity()
{
    return specificity;
}

float FuzzySystem::getAccuracy()
{
    return accuracy;
}

float FuzzySystem::getPpv()
{
    return ppv;
}

float FuzzySystem::getRmse()
{
    return rmse;
}

void FuzzySystem::saveToFile(QString fileName, float fitVal)
{

    SystemParameters& sysParams = SystemParameters::getInstance();

    QDomDocument doc("FUGE-LC_fuzzy_system_file");
    QDomElement fuzzySystem = doc.createElement("Fuzzy_System");
    doc.appendChild(fuzzySystem);
    QDomElement datasetName = doc.createElement("Dataset_name");
    fuzzySystem.appendChild(datasetName);
    QDomText datasetNameText = doc.createTextNode(sysParams.getDatasetName());
    datasetName.appendChild(datasetNameText);
    QDomElement fit = doc.createElement("Fitness");
    fuzzySystem.appendChild(fit);
    QDomElement fitValue = doc.createElement("Value");
    fit.appendChild(fitValue);
    QDomText fitValueText = doc.createTextNode(QString::number(fitVal));
    fitValue.appendChild(fitValueText);
    QDomElement fitSensi = doc.createElement("SensiW");
    fit.appendChild(fitSensi);
    QDomText sensiText = doc.createTextNode(QString::number(sysParams.getSensiW()));
    fitSensi.appendChild(sensiText);
    QDomElement fitSpeci = doc.createElement("SpeciW");
    fit.appendChild(fitSpeci);
    QDomText speciText = doc.createTextNode(QString::number(sysParams.getSpeciW()));
    fitSpeci.appendChild(speciText);
    QDomElement fitAccu = doc.createElement("AccuW");
    fit.appendChild(fitAccu);
    QDomText accuText = doc.createTextNode(QString::number(sysParams.getAccuracyW()));
    fitAccu.appendChild(accuText);

    QDomElement fitPpv = doc.createElement("PPVW");
    fit.appendChild(fitPpv);
    QDomText ppvText = doc.createTextNode(QString::number(sysParams.getPpvW()));
    fitPpv.appendChild(ppvText);

    QDomElement fitRmse = doc.createElement("RMSEW");
    fit.appendChild(fitRmse);
    QDomText rmseText = doc.createTextNode(QString::number(sysParams.getRmseW()));
    fitRmse.appendChild(rmseText);

    // MODIF - BUJARD Alexandre - 16.04.2010
    QDomElement fitRrse = doc.createElement("RRSEW");
    fit.appendChild(fitRrse);
    QDomText rrseText = doc.createTextNode(QString::number(sysParams.getRrseW()));
    fitRrse.appendChild(rrseText);

    QDomElement fitRae = doc.createElement("RAEW");
    fit.appendChild(fitRae);
    QDomText raeText = doc.createTextNode(QString::number(sysParams.getRaeW()));
    fitRae.appendChild(raeText);

    QDomElement fitMse = doc.createElement("MSEW");
    fit.appendChild(fitMse);
    QDomText mseText = doc.createTextNode(QString::number(sysParams.getMseW()));
    fitMse.appendChild(mseText);

    QDomElement fitDistanceThreshold = doc.createElement("ADMW");
    fit.appendChild(fitDistanceThreshold);
    QDomText distanceThresholdText = doc.createTextNode(QString::number(sysParams.getDistanceThresholdW()));
    fitDistanceThreshold.appendChild(distanceThresholdText);

    QDomElement fitDistanceMinThreshold = doc.createElement("MDMW");
    fit.appendChild(fitDistanceMinThreshold);
    QDomText distanceMinThresholdText = doc.createTextNode(QString::number(sysParams.getDistanceMinThresholdW()));
    fitDistanceMinThreshold.appendChild(distanceMinThresholdText);

    QDomElement fitDontCare = doc.createElement("SizeW");
    fit.appendChild(fitDontCare);
    QDomText dontCareText = doc.createTextNode(QString::number(sysParams.getDontCareW()));
    fitDontCare.appendChild(dontCareText);

    QDomElement fitOverLearn = doc.createElement("OverLearnW");
    fit.appendChild(fitOverLearn);
    QDomText overLearnText = doc.createTextNode(QString::number(sysParams.getOverLearnW()));
    fitOverLearn.appendChild(overLearnText);
    // FIN - MODIF - BUJARD Alexandre - 16.04.2010

    QDomElement fitThresh = doc.createElement("Threshold");
    fit.appendChild(fitThresh);
    for (int i = 0; i < this->nbOutVars; i++) {
        QDomElement var = doc.createElement("Thresh");
        fitThresh.appendChild(var);
        QDomText threshText = doc.createTextNode(QString::number(sysParams.getThresholdVal(i)));
        var.appendChild(threshText);
    }

    QDomElement varList = doc.createElement("Variables");
    fuzzySystem.appendChild(varList);

    for (int i = 0; i < nbInVars; i++) {

        // Save only variables that are present in the rules
        if (inVarArray[i]->isUsedBySystem()) {
            QDomElement var = doc.createElement("Variable_in");
            varList.appendChild(var);

            QDomElement name = doc.createElement("Name");
            var.appendChild(name);
            QDomText t = doc.createTextNode(/*systemData->at(0).at(i+1)*/inVarArray[i]->getName());
            name.appendChild(t);

            for (int j = 0; j < inVarArray[i]->getSetsCount(); j++) {
                QDomElement set = doc.createElement("Set");
                var.appendChild(set);
                QDomElement setName = doc.createElement("Set_name");
                set.appendChild(setName);
                QDomElement setPos = doc.createElement("Set_position");
                set.appendChild(setPos);
                QDomText setNameText = doc.createTextNode(inVarArray[i]->getSet(j)->getName());
                setName.appendChild(setNameText);
                QDomText setPosText = doc.createTextNode(QString::number(inVarArray[i]->getSet(j)->getPosition()));
                setPos.appendChild(setPosText);
            }
        }
    }

    for (int i = 0; i < nbOutVars; i++) {

        QDomElement var = doc.createElement("Variable_out");
        varList.appendChild(var);

        QDomElement name = doc.createElement("Name");
        var.appendChild(name);
        QDomText t = doc.createTextNode(/*systemData->at(0).at(i+nbInVars+1)*/outVarArray[i]->getName());
        name.appendChild(t);
        for (int j = 0; j < nbOutSets; j++) {
            QDomElement set = doc.createElement("Set");
            var.appendChild(set);
            QDomElement setName = doc.createElement("Set_name");
            set.appendChild(setName);
            QDomElement setPos = doc.createElement("Set_position");
            set.appendChild(setPos);
            QDomText setNameText = doc.createTextNode(outVarArray[i]->getSet(j)->getName());
            setName.appendChild(setNameText);
            QDomText setPosText = doc.createTextNode(QString::number(outVarArray[i]->getSet(j)->getPosition()));
            setPos.appendChild(setPosText);
        }
    }

    QDomElement rulesList = doc.createElement("Rules");
    fuzzySystem.appendChild(rulesList);

    for (int i = 0; i < nbRules; i++) {

        // Save only rules that are not empty
        if (rulesArray[i]->getNbInPairs() > 0) {
            QDomElement rule = doc.createElement("Rule");
            rulesList.appendChild(rule);
            for (int k = 0; k < rulesArray[i]->getNbInPairs(); k++) {
                QDomElement inVar = doc.createElement("In_Var");
                rule.appendChild(inVar);
                QDomText varNameText = doc.createTextNode(rulesArray[i]->getInVarAtPos(k)->getName());
                inVar.appendChild(varNameText);
                QDomElement inSet = doc.createElement("In_Set");
                rule.appendChild(inSet);
                QDomText setNameText = doc.createTextNode(rulesArray[i]->getInSetAtPos(k)->getName());
                inSet.appendChild(setNameText);
            }
            for (int k = 0; k < rulesArray[i]->getNbOutPairs(); k++) {
                if (rulesArray[i]->getNbInPairs() != 0) {
                    QDomElement outVar = doc.createElement("Out_Var");
                    rule.appendChild(outVar);
                    QDomText varNameText = doc.createTextNode(rulesArray[i]->getOutVarAtPos(k)->getName());
                    outVar.appendChild(varNameText);
                    QDomElement outSet = doc.createElement("Out_Set");
                    rule.appendChild(outSet);
                    QDomText setNameText = doc.createTextNode(rulesArray[i]->getOutSetAtPos(k)->getName());
                    outSet.appendChild(setNameText);
                }
            }
        }
    }
    QDomElement defRules = doc.createElement("Default_Rules");
    rulesList.appendChild(defRules);
    for (int i = 0; i < nbOutVars; i++) {
        QDomElement defRule = doc.createElement("Default_Rule");
        defRules.appendChild(defRule);
        QDomText defRuleText = doc.createTextNode(QString::number(defaultRulesSets.at(i)));
        defRule.appendChild(defRuleText);
    }

    QString xml = doc.toString();
    QFile file(fileName);
    file.remove(fileName);
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream out(&file);
    out << xml;
    file.close();
}

int FuzzySystem::getVarIndex(QString name)
{
    for (int i = 0; i < nbInVars; i++) {
        if (inVarArray[i]->getName() == name) {
            return i;
        }
    }
    for (int i = 0; i < nbOutVars; i++) {
        if (outVarArray[i]->getName() == name)
            return i;
    }
    std::cout << " ERROR : FuzzySystem::getVarIndex failed : not possible ;)" << std::endl;
    throw;
}

void FuzzySystem::loadFromFile(QString fileName)
{

    SystemParameters& sysParams = SystemParameters::getInstance();

    QString errorMsg;
    int errorLine, errorColumn;

    QDomDocument doc("Fuzzy_System");
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
        return;

    if (!doc.setContent(&file, false, &errorMsg, &errorLine, &errorColumn)) {
        std::cout << errorMsg.toStdString() << " " <<  errorLine << " " << errorColumn << std::endl;
        file.close();
        return;
    }
    file.close();

    QDomNodeList nodesInVariables;
    QDomNodeList nodesOutVariables;
    QDomNodeList nodesSets;
    QDomNodeList nodesRules;
    QDomNodeList nodesRulesInVars;
    QDomNodeList nodesRulesOutVars;
    QDomNodeList nodesRulesInSets;
    QDomNodeList nodesRulesOutSets;
    QDomNodeList nodesDefaultRules;
    QDomNodeList nodesFitness;

    // Retrieve the fitness value


    // Retrieve the input variables list
    nodesInVariables = doc.documentElement().namedItem("Variables").toElement().elementsByTagName("Variable_in");
    // Retrieve the input variables number and create an empty array
    nbInVars = nodesInVariables.size();
    sysParams.setNbInVars(nbInVars);
    inVarArray = new FuzzyVariable*[nbInVars];

    // Read all the input variables
    for (int i = 0; i < nbInVars; i++) {
        // Create the corresponding variables
        inVarArray[i] = new FuzzyVariable(nodesInVariables.at(i).toElement().namedItem("Name").toElement().text(), coco);
        // Retrieve the sets list
        nodesSets = nodesInVariables.at(i).toElement().elementsByTagName("Set");
        this->nbInSets = nodesSets.size();
        for (int k = 0; k < nodesSets.size(); k++) {
            // Create the corresponding sets and add them to the variable
            FuzzySet* fSet = new FuzzySet(nodesSets.at(k).toElement().namedItem("Set_name").toElement().text(),
                                          nodesSets.at(k).toElement().namedItem("Set_position").toElement().text().toFloat(), k);
            inVarArray[i]->addSet(fSet);
        }
        inVarArray[i]->setUsedBySystem(true);
    }
    // Retrieve the output variables list
    nodesOutVariables = doc.documentElement().namedItem("Variables").toElement().elementsByTagName("Variable_out");
    // Retrieve the output variables number and create an empty array
    nbOutVars = nodesOutVariables.size();
    sysParams.setNbOutVars(nbOutVars);
    outVarArray = new FuzzyVariable*[nbOutVars];

    // Read all the output variables
    for (int i = 0; i < nbOutVars; i++) {
        // Create the corresponding variables
        outVarArray[i] = new FuzzyVariable(nodesOutVariables.at(i).toElement().namedItem("Name").toElement().text(), singleton);
        // Set the output flag
        outVarArray[i]->setOutput(true);
        // Retrieve the sets list
        nodesSets = nodesOutVariables.at(i).toElement().elementsByTagName("Set");
        this->nbOutSets = nodesSets.size();
        for (int k = 0; k < nodesSets.size(); k++) {
            // Create the corresponding sets and add them to the variable
            FuzzySet* fSet = new FuzzySet(nodesSets.at(k).toElement().namedItem("Set_name").toElement().text(),
                                          nodesSets.at(k).toElement().namedItem("Set_position").toElement().text().toFloat(), k);
            outVarArray[i]->addSet(fSet);
        }
    }

    // Retrieve the rules list
    nodesRules = doc.documentElement().namedItem("Rules").toElement().elementsByTagName("Rule");
    // Retrieve the number of rules and create an empty rules array
    nbRules = nodesRules.size();
    rulesArray = new FuzzyRule*[nbRules];
    // Create an empty default rule array
    defaultRulesSets.resize(nbOutVars);
    // Retrieve the rules
    for (int i = 0; i < nbRules; i++) {
        // Retrieve the input/output variables from the rule and their corresponding sets
        nodesRulesInVars = nodesRules.at(i).toElement().elementsByTagName("In_Var");
        nodesRulesOutVars = nodesRules.at(i).toElement().elementsByTagName("Out_Var");
        nodesRulesInSets = nodesRules.at(i).toElement().elementsByTagName("In_Set");
        nodesRulesOutSets = nodesRules.at(i).toElement().elementsByTagName("Out_Set");

        // Create an empty intstring rule genome
        FuzzyRuleGenome *ruleGen = new FuzzyRuleGenome(nodesRulesInVars.size(), nodesRulesInVars.size(), nodesRulesOutVars.size(),
                                                       1,1,1,1);
        // Create an empty instring
        int intStringSize = nodesRulesInVars.size()*2 + nodesRulesOutVars.size()*2;
        quint16 intString[intStringSize];

        for (int k = 0, l = 0; k < nodesRulesInVars.size(); k++, l+=2) {
            intString[l] = getVarIndex(nodesRulesInVars.at(k).toElement().text());
            intString[l+1] =  inVarArray[intString[l]]->getSetIndexByName(nodesRulesInSets.at(k).toElement().text());
        }
        for (int k = 0, l = nodesRulesInVars.size()*2; k < nodesRulesOutVars.size(); k++, l+=2) {
            intString[l] = getVarIndex(nodesRulesOutVars.at(k).toElement().text());
            intString[l+1] = outVarArray[intString[l]]->getSetIndexByName(nodesRulesOutSets.at(k).toElement().text());
        }
        // Load the instring into the genome
        ruleGen->readGenomeIntString(intString, intStringSize);

        // Create the rule
        rulesArray[i] = new FuzzyRule(inVarArray, outVarArray, ruleGen);
        // Update the system description
        systemDescription.append(rulesArray[i]->getDescription());
        systemDescription.append("\n");
    }

    // Retrieve the default rules
    nodesDefaultRules = doc.documentElement().namedItem("Rules").toElement().namedItem("Default_Rules").toElement().elementsByTagName("Default_Rule");
    for (int i = 0; i < nodesDefaultRules.size(); i++) {
        defaultRulesSets.replace(i, nodesDefaultRules.at(i).toElement().text().toInt());
    }

    // Add the default rule to the system description
    systemDescription.append(" ELSE : ");
    for (int i = 0; i < nbOutVars; i++) {
        systemDescription.append(outVarArray[i]->getName());
        systemDescription.append(" is ");
        systemDescription.append(QString::number(defaultRulesSets.at(i)));
        systemDescription.append("  ");
    }
    // Update the system description with the membership functions
    // of the variables used in the rules
    systemDescription.append("\nMembership functions : \n");
    for (int i = 0; i  < nbInVars; i++) {
        systemDescription.append(inVarArray[i]->getName());
        systemDescription.append(" (");
        for (int k = 0; k < inVarArray[i]->getSetsCount(); k++) {
            systemDescription.append(QString::number(inVarArray[i]->getSet(k)->getPosition()));
            if (k != inVarArray[i]->getSetsCount() - 1)
                systemDescription.append(" , ");
        }
        systemDescription.append(") / ");
    }
    for (int i = 0; i  < nbOutVars; i++) {
        systemDescription.append(outVarArray[i]->getName());
        systemDescription.append(" (");
        for (int k = 0; k < outVarArray[i]->getSetsCount(); k++) {
            systemDescription.append(QString::number(outVarArray[i]->getSet(k)->getPosition()));
            if (k != outVarArray[i]->getSetsCount() - 1)
                systemDescription.append(" , ");
        }
        systemDescription.append(")");
        if (i != nbOutVars-1)
            systemDescription.append(" / ");
        else
            systemDescription.append("\n");
    }

    // Set some parameters related to the system being loaded
    CoevStats& coevStats = CoevStats::getInstance();

    coevStats.setFitMaxPop1(doc.documentElement().namedItem("Fitness").toElement().namedItem("Value").toElement().text().toFloat());
    sysParams.setNbOutVars(nbOutVars);
    sysParams.setSensiW(doc.documentElement().namedItem("Fitness").toElement().namedItem("SensiW").toElement().text().toFloat());
    sysParams.setSpeciW(doc.documentElement().namedItem("Fitness").toElement().namedItem("SpeciW").toElement().text().toFloat());
    sysParams.setAccuracyW(doc.documentElement().namedItem("Fitness").toElement().namedItem("AccuW").toElement().text().toFloat());
    sysParams.setPpvW(doc.documentElement().namedItem("Fitness").toElement().namedItem("PPVW").toElement().text().toFloat());
    sysParams.setRmseW(doc.documentElement().namedItem("Fitness").toElement().namedItem("RMSEW").toElement().text().toFloat());
    sysParams.setRrseW(doc.documentElement().namedItem("Fitness").toElement().namedItem("RRSEW").toElement().text().toFloat());
    sysParams.setRaeW(doc.documentElement().namedItem("Fitness").toElement().namedItem("RAEW").toElement().text().toFloat());
    sysParams.setMseW(doc.documentElement().namedItem("Fitness").toElement().namedItem("MSEW").toElement().text().toFloat());
    sysParams.setDistanceThresholdW(doc.documentElement().namedItem("Fitness").toElement().namedItem("ADMW").toElement().text().toFloat());
    sysParams.setDistanceMinThresholdW(doc.documentElement().namedItem("Fitness").toElement().namedItem("MDMW").toElement().text().toFloat());
    sysParams.setDontCareW(doc.documentElement().namedItem("Fitness").toElement().namedItem("SizeW").toElement().text().toFloat());
    sysParams.setOverLearnW(doc.documentElement().namedItem("Fitness").toElement().namedItem("OverLearnW").toElement().text().toFloat());


    QVector<float> threshold;
    threshold.resize(nbOutVars);
    /*for (int i = 0; i < this->nbOutVars; i++) {
        sysParams.setThresholdVal(i, doc.documentElement().namedItem("Fitness").toElement().namedItem("Threshold").toElement().text().toFloat());
        //sysParams.setThreshold(threshold);
    }*/

    QDomNodeList nodesThreshold = doc.documentElement().namedItem("Fitness").toElement().namedItem("Threshold").toElement().elementsByTagName("Thresh");
    for (int i = 0; i < nodesThreshold.size(); i++) {
        sysParams.setThresholdVal(i, nodesThreshold.at(i).toElement().text().toFloat());
    }



    QString datasetName = doc.documentElement().namedItem("Dataset_name").toElement().text();
    if (datasetName  != "")
        sysParams.setDatasetName(doc.documentElement().namedItem("Dataset_name").toElement().text());

    rulesLoaded = true;
    membershipsLoaded = true;
}


FuzzyVariable* FuzzySystem::getInVar(int varnum)
{
    assert (varnum < nbInVars);
    return inVarArray[varnum];
}

FuzzyVariable* FuzzySystem::getVar(int varnum)
{
    assert (varnum < nbInVars + nbOutVars);
    if (varnum >= this->nbInVars) {
        return outVarArray[varnum-this->nbInVars];
    }
    else {
        return inVarArray[varnum];
    }
}

FuzzyVariable* FuzzySystem::getInVarByName(QString name)
{
    for (int i = 0; i < nbInVars; i++) {
        if (inVarArray[i]->getName() == name)
            return inVarArray[i];
    }
    return NULL;
}

FuzzyVariable* FuzzySystem::getOutVarByName(QString name)
{
    for (int i = 0; i < nbOutVars; i++) {
        if (outVarArray[i]->getName() == name)
            return outVarArray[i];
    }
    return NULL;
}

FuzzyVariable* FuzzySystem::getOutVar(int varnum)
{
    assert (varnum < nbOutVars);
    return outVarArray[varnum];
}

float FuzzySystem::getVarValMin(int varnum)
{
    assert(dataLoaded);
    return varUniverseArray[varnum].valMin;
}

float FuzzySystem::getVarValMax(int varnum)
{
    assert(dataLoaded);
    return varUniverseArray[varnum].valMin;
}

FuzzyRule* FuzzySystem::getRule(int ruleNum)
{
    return this->rulesArray[ruleNum];
}

void FuzzySystem::replaceRule(int ruleNum, FuzzyRule* newRule)
{
    // Delete the old rule
    delete rulesArray[ruleNum];
    rulesArray[ruleNum] = newRule;
}

QVector<int> FuzzySystem::getDefaultRules()
{
    return defaultRulesSets;
}

void FuzzySystem::updateDefaultRule(int outVarNum, int defaultSet)
{
    defaultRulesSets.replace(outVarNum, defaultSet);
}

void FuzzySystem::setNbInSets(int num)
{
    nbInSets = num;
}

void FuzzySystem::setNbOutSets(int num)
{
    nbOutSets = num;
}

void FuzzySystem::printVerboseOutput()
{

    CoevStats& stats = CoevStats::getInstance();

    std::cout << "[GENERATION_POP1] " << stats.getGenNumberPop1() << std::endl;
    std::cout << "[GENERATION_POP2] " << stats.getGenNumberPop2() << std::endl;
    std::cout << "[FITNESS] " << this->getFitness() << std::endl;
    std::cout << "[SENSITIVITY] " << stats.getSensi() << std::endl;
    std::cout << "[SPECIFICITY] " << stats.getSpeci() << std::endl;
    std::cout << "[ACCURACY] " << stats.getAccu() << std::endl;
    std::cout << "[PPV] " << stats.getPpv() << std::endl;
    std::cout << "[RMSE] " << stats.getRmse() << std::endl;
    std::cout << "[DESCRIPTION] " << std::endl;
    for (int i = 0; i < this->nbRules; i++) {
        std::cout << "[RULE " << i << "] " << rulesArray[i]->getDescription().toStdString() << std::endl;
    }
    std::cout << "[DEFRULE] ELSE ";
    for (int i = 0; i < nbOutVars; i++) {
        std::cout << outVarArray[i]->getName().toStdString();
        std::cout << " is ";
        std::cout << defaultRulesSets.at(i);
        std::cout << "  ";
    }
    std::cout << std::endl;

    QStringList completeDesc = this->getSystemDescritpion().split("Membership functions :");
    std::cout << "[MEMBERSHIPS] " << completeDesc.at(1).toStdString();
}

void FuzzySystem::setFitness(float fit)
{

    this->fitness = fit;
}


FuzzyVariable** FuzzySystem::getInVars(){
    return inVarArray;
}

FuzzyVariable** FuzzySystem::getOutVars(){
    return outVarArray;
}

FuzzyRule** FuzzySystem::getRules(){
    return rulesArray;
}
