/**
  * @file   coevolution.cpp
  * @author Jean-Philippe Meylan <jean-philippe.meylan_at_heig-vd.ch>
  * @author ReDS (Reconfigurable and embedded digital systems) <www.reds.ch>
  * @author HEIG-VD (Haute école d'ingénierie et de gestion) <www.heig-vd.ch>
  * @author Yvan Da Silva <yvan.dasilva_at_heig-vd.ch>
  * @date   06.2012
  * @date   03.2010
  * @section LICENSE
  *
  * This application is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 3.0 of the License, or (at your option) any later version.
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
  * @class CoEvolution
  *
  * @brief This class implements the evaluation operator for the coevolution. Two methods are
  * mandatory for Beagle : makeSets and evaluateSets. Basically, this class receives the
  * two populations and is responsible to evaluate the fitness of all individuals.
  */

#include "coevolution.h"

int counter = 0;
int counterCoops = 0;

/**
 * @brief CoEvolution::CoEvolution
 * Constructs a new coevolution system.
 *
 * @param fSystem The Fuzzy System to work on.
 * @param leftLock Lock of synchronization of the first population
 * @param rightLock Lock of synchronization of the second population
 * @param leftPop The first population
 * @param rightPop The second population
 * @param generationCount Number of generation to evolve
 * @param crossoverProbability Probability of crossover, reproduction
 * @param mutationProbability Probability of mutation
 * @param mutationPerBitProbability Per bit mutation probability
 * @param eliteSize Number of elites to keep from a generation to another.
 * @param cooperatorsCount Number of cooperators
 * @param parent Parent thread
 */
CoEvolution::CoEvolution(FuzzySystem *fSystem, QMutex *leftLock, QMutex *rightLock, Population *leftPop, Population *rightPop, quint32 generationCount, qreal crossoverProbability, qreal mutationProbability, qreal mutationPerBitProbability, quint32 eliteSize, quint32 cooperatorsCount, QObject *parent) : EvolutionEngine(leftPop, generationCount, crossoverProbability, mutationProbability, mutationPerBitProbability), QThread(parent), fSystem(fSystem), leftLock(leftLock), rightLock(rightLock), eliteSize(eliteSize), left(leftPop), right(rightPop), generationCount(generationCount), cooperatorsCount(cooperatorsCount)
{
    isFirst = true;
    needToSave = false;
    fileName.clear();
}

/**
 * @brief CoEvolution::~CoEvolution
 * Destroy the coevolution system.
 */
CoEvolution::~CoEvolution()
{
}

/**
 * @brief CoEvolution::run
 * Starts the coevolution.
 */
void CoEvolution::run(){
    // Command the verbose output print
    qDebug() << "RUN : " << left->getName() << " : left_getsize : " << left->getSize();

    //TODO set generation values trough the interface.
    startEvolution(leftLock,rightLock, generationCount,
                   getEntitySelectors().at(0),eliteSize,
                   getEntitySelectors().at(1),left->getSize()-eliteSize,
                   getMutationMethods().at(0),getCrossoverMethods().at(0), cooperatorsCount);
}

/**
 * @brief CoEvolution::evaluatePopulation
 * Evaluate the current population.
 *
 * @param population to evaluate
 * @param generation number of the population
 * @return true if succeed, false if something gone wrong.
 */
bool CoEvolution::evaluatePopulation(Population* population, quint32 generation){
    vector<PopEntity *> leftPopEntities = population->getAllEntities();
    PopEntity *bestCurrGenRepresentative;
    PopEntity *bestCurrGenLeftPopEntity;
    qreal currentIndBestFit = 0.0;
    qreal overallBestFit = 0.0;

    // If coevolutionary (default fuzzyCoco method) is enabled or that we use
    // coevolution to define init memberships for learning.
    if(ComputeThread::sysParams->isCoevolutionary()){
        //Evaluate our population with the other cooperators(elites)

        vector<PopEntity *> RightRepresentative;

        // Due to multithreading representatives from the other population might not be ready.
        RightRepresentative = right->getRepresentativesCopy();

        vector<PopEntity *>::iterator itLeftPop, itRepresentative;



        for(itLeftPop=leftPopEntities.begin(); itLeftPop!=leftPopEntities.end(); itLeftPop++)
        {
            currentIndBestFit = 0.0;
            // Loop through all cooperators
            for(itRepresentative=RightRepresentative.begin(); itRepresentative!=RightRepresentative.end(); itRepresentative++)
            {
                fitness = 0.0;
                if(left->getName() == "MEMBERSHIPS")
                    calcFitness((*itLeftPop),(*itRepresentative));
                else
                    calcFitness((*itRepresentative), (*itLeftPop));
                if (fitness > currentIndBestFit) {
                    currentIndBestFit = fitness;
                    if(fitness > overallBestFit) {
                        overallBestFit = fitness;
                        bestCurrGenRepresentative = *itRepresentative;
                        bestCurrGenLeftPopEntity = *itLeftPop;
                    }
                    (*itLeftPop)->setFitness(fitness); // choose the best fit, between ind & all coops
                }
                if(ComputeThread::stop)
                    break;
            }
            if(currentIndBestFit)
                getStatisticEngine()->addFitness(fitness);
            if(ComputeThread::stop)
                break;
        }

        // FIXME: HOT fix because the best fuzzy system is the last generation best fuzzy system
        // which is wrong, but until we continue to use ELITISM it will work.
        // This should not be needed, instead the whole fuzzy system object should be saved on computeThread !
        if(left->getName() == "MEMBERSHIPS")
            calcFitness(bestCurrGenLeftPopEntity, bestCurrGenRepresentative);
        else
            calcFitness(bestCurrGenRepresentative, bestCurrGenLeftPopEntity);

        // Delete representatives
        for(int i = 0; i < RightRepresentative.size(); i++)
            delete RightRepresentative[i];

        // Print the output in verbose mode if needed
        if (ComputeThread::sysParams->getVerbose()) {
            std::cout << "verbose (evalop) is " << ComputeThread::sysParams->getVerbose() << std::endl;
            fSystem->printVerboseOutput();
        }

        // Build stats
        statsEngine.buildStats();
        // Plot stats and save fuzzy system
        ComputeThread::saveSystemStats(population->getName(), statsEngine.getMinFitness(),statsEngine.getMaxFitness(),statsEngine.getMeanFitness(),statsEngine.getStandardDeviation(),population->getSize(), generation);

        // Reset stats engine
        statsEngine.reset();

        // Stop in case max fitness reached.
        if (left->getName() == "MEMBERSHIPS"){
            if(ComputeThread::bestFSystem->getFitness() >= ComputeThread::sysParams->getMaxFitPop1())
                emit fitnessThreshReached();
        }else{
            if(ComputeThread::bestFSystem->getFitness() >= ComputeThread::sysParams->getMaxFitPop2())
                emit fitnessThreshReached();
        }
    }
    else
        // IN CASE OF LEARNING :
    {
        vector<PopEntity *>::iterator itLeftPop;
        for(itLeftPop=leftPopEntities.begin(); itLeftPop!=leftPopEntities.end(); itLeftPop++)
        {
            fitness = 0.0;

            calcFitness(NULL, *itLeftPop);

            if(fitness > overallBestFit) {
                overallBestFit = fitness;
                bestCurrGenLeftPopEntity = *itLeftPop;
            }
            (*itLeftPop)->setFitness(fitness); // choose the best fit, between ind & all coops
            getStatisticEngine()->addFitness(fitness);
            if(ComputeThread::stop)
                break;
        }
        calcFitness(NULL, bestCurrGenLeftPopEntity);

        // Build stats
        statsEngine.buildStats();
        // Plot stats and save fuzzy system
        ComputeThread::saveSystemStats(population->getName(), statsEngine.getMinFitness(),statsEngine.getMaxFitness(),statsEngine.getMeanFitness(),statsEngine.getStandardDeviation(),population->getSize(), generation);

        // Reset stats engine
        statsEngine.reset();

        //Stop in case max fitness reached.
        if(ComputeThread::bestFSystem->getFitness() >= ComputeThread::sysParams->getMaxFitPop2())
            emit fitnessThreshReached();
    }

    return !ComputeThread::stop;
}



/**
  * @brief CoEvolution::calcFitness
  * Compute the fitness of a couple of two individuals, which form a fuzzy system. The fuzzy
  * system is evaluated against the dataset.
  *
  * @param inX Individual of population 1 (membership functions)
  * @param inY Individual of population 2 (rules)
  */
void CoEvolution::calcFitness(PopEntity *inX, PopEntity *inY)
{
    QBitArray *genotypeDataX;
    if(ComputeThread::sysParams->isCoevolutionary())
        genotypeDataX = inX->getGenotype()->getData();
    QBitArray *genotypeDataY = inY->getGenotype()->getData();
    quint16 ruleBitString[ComputeThread::ruleGenSize];

    FuzzyMembershipsGenome* membGen = new FuzzyMembershipsGenome(fSystem->getNbInVars(),fSystem->getNbOutVars(), fSystem->getNbInSets(),fSystem->getNbOutSets(), fSystem->getInSetsPosCodeSize(), fSystem->getOutSetsPosCodeSize());
    FuzzyRuleGenome* ruleGenTab[ComputeThread::nbRules];

    for (int i = 0; i < ComputeThread::nbRules; i++) {
        ruleGenTab[i] = new FuzzyRuleGenome(fSystem->getNbVarPerRule(), fSystem->getNbInVars(),fSystem->getNbOutVars(),
                                            fSystem->getInVarsCodeSize(),fSystem->getOutVarsCodeSize(),
                                            fSystem->getInSetsCodeSize(), fSystem->getOutSetsCodeSize());
    }


    // Read the memberships genome
    if(ComputeThread::sysParams->isCoevolutionary())
        membGen->readGenomeBitString(genotypeDataX, ComputeThread::membersGenSize);

    // Rules genome transcription with FIXED VARS

    // FIXME: Fixed vars were never used on any test case provided, is it still necessary ? From : Yvan Da Silva.
    if (ComputeThread::sysParams->getFixedVars()) {
        qDebug() << "FIXED";
        // Transcript each rule one by one
        for (int k = 0; k < ComputeThread::nbRules; k++) {
            // Variables d'entrée : le code des variables d'entrée est toujours 0
            for (int l = 0; l < ComputeThread::nbVarPerRule; l++) {
                ruleBitString[l*(ComputeThread::inSetsCodeSize+1)] = 0;
                for (int m = 1; m < ComputeThread::inSetsCodeSize+1; m++) {
                    ruleBitString[l*(ComputeThread::inSetsCodeSize+1) + m] = genotypeDataY->at(k*fSystem->getRuleBitStringSize() + (l*ComputeThread::inSetsCodeSize) + m-1);
                }
            }
            // Variables de sortie
            int outBase = ComputeThread::nbVarPerRule * (ComputeThread::inSetsCodeSize+1);
            for (int l = 0; l < ComputeThread::nbOutVars; l++) {
                // Le code des variables de sortie est toujours 0
                ruleBitString[outBase + l*(ComputeThread::outSetsCodeSize+1)] = 0;
                for (int m = 1; m < ComputeThread::outSetsCodeSize+1; m++) {
                    ruleBitString[outBase + l*(ComputeThread::outSetsCodeSize+1) + m] = genotypeDataY->at(k*fSystem->getRuleBitStringSize()
                                                                                                          + ComputeThread::nbVarPerRule*ComputeThread::inSetsCodeSize + l*ComputeThread::outSetsCodeSize + m-1);
                }
            }
            // Read the rule
            ruleGenTab[k]->readGenomeBitString(ruleBitString, ComputeThread::ruleGenSize);
        }
    }
    // Genome transcription with EVOLVING VARS
    else {
        // FIXME: ReadGenomeBitString should be able to decode QbitArray directly avoid this N2 operation. From : Yvan Da Silva.
        for (int k = 0; k < ComputeThread::nbRules; k++) {
            for (int l = 0; l < ComputeThread::ruleGenSize; l++) {
                ruleBitString[l] = genotypeDataY->at(l + k*ComputeThread::ruleGenSize);
            }
            ruleGenTab[k]->readGenomeBitString(ruleBitString, ComputeThread::ruleGenSize);
        }
    }
    // Default rules transcription
    int defRulesSize = fSystem->getDefaultRulesBitStringSize();
    int defRulesPos = fSystem->getRuleBitStringSize()*ComputeThread::nbRules;
    int defRules[defRulesSize];
    for (int i = 0; i < defRulesSize; i++) {
        defRules[i] = genotypeDataY->at(defRulesPos+i);
    }

    // Reset the previous fuzzy system
    fSystem->reset();

    // Load the genomes
    if(ComputeThread::sysParams->isCoevolutionary()){
        fSystem->loadMembershipsGenome(membGen, 2);
    }
    else if(ComputeThread::sysParams->getInitVarsMethod() == 0){
        fSystem->loadMembershipsGenome(membGen, 0);
    }
    else if(ComputeThread::sysParams->getInitVarsMethod() == 1){
        fSystem->loadMembershipsGenome(membGen, 1);
    }

    fSystem->loadRulesGenome(ruleGenTab, defRules);
    // Get the textual systemDescription
    fitness = fSystem->evaluateFitness();

    ComputeThread::saveFuzzyAndFitness(fSystem,fitness);

    // Delete everything we don't need anymore ( Created in this functio ).
    delete membGen;
    for (int i = 0; i < ComputeThread::nbRules; i++) {
        delete ruleGenTab[i];
    }



}
