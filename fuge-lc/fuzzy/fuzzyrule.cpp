/**
  * @file   fuzzyrule.cpp
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
  * @class FuzzyRule
  *
  * @brief This class represent a fuzzy rule.
  *
  * @section DESCRIPTION
  *
  * This class represent a fuzzy rule. The rule is composed of [input/output variables - sets] pairs.
  * The rule is created by reading a rule genome and fetching the corresponding variables in an array
  * of variables passed in parameter. The variable array contains all the existing variables of the
  * system. The variables used in the rule are selected according to the rule genome and stored in
  * internal arrays.
  *
  * Once the rule has been evaluated, the result is stored in the corresponding set of the corresponding
  * variable for later defuzzification.
  */

#include <iostream>

#include "fuzzyrule.h"
#include "fuzzyoperator.h"
#include "fuzzyoperatorand.h"
#include "systemparameters.h"

// FIXME: Output values don't support -1 due to this implementation.
#define DONT_CARE_EVAL_RULE -1.0


/**
  * Constructor. The rule is constructed using the variables passed in the array according to the information
  * stored in the genome.
  *
  * @param inVarArray Array containing the input variables defined in the system.
  * @param outVarArray Array containing the output variables defined in the system.
  * @param ruleGenome Genome encoding the information describing the rule.
  */
FuzzyRule::FuzzyRule(FuzzyVariable** inVarArray, FuzzyVariable** outVarArray, FuzzyRuleGenome* ruleGenome)
{
    SystemParameters& sysParams = SystemParameters::getInstance();

    inVars = ruleGenome->getInputVarCount();

    int inVarsOrig = inVars;
    outVars = ruleGenome->getOutputVarCount();
    int outVarsOrig = outVars;

    inVarsTab = new FuzzyVariable* [inVars];
    outVarsTab = new FuzzyVariable* [outVars];
    inVarsSetsTab = new int[inVars];
    outVarsSetsTab = new int[outVars];

    fireLevel = new double[outVars];

    int varNum;

    // Copy the input variables and the set numbers present in the genome in a local array
    for (int i = 0, pos = 0; i < inVarsOrig; i++) {
        varNum = ruleGenome->getVarNumber(i);

        // EVOLVING VARS
        if (!sysParams.getFixedVars()) {
            // Check that variable number is valid, if not, it is removed from the rule
            if (varNum == -1) {
                inVars--;
            }
            else if (varNum >= sysParams.getNbInVars()) {
                inVars--;
            }
            // Check that set number is valid, if not, the variable is removed from the rule
            else if (ruleGenome->getSetNumber(i) >= inVarArray[varNum]->getSetsCount()) {
                inVars--;
            }
            // Valid variable number
            else {
                // Mark the variable as used by the system
                inVarArray[varNum]->setUsedBySystem(true);
                // Retrieve the variable
                inVarsTab[pos] = inVarArray[varNum];
                // Retrieve the corresponding set
                inVarsSetsTab[pos] = ruleGenome->getSetNumber(i);
                pos++;
            }
        }
        // FIXED VARS
        else {
            // Check that set number is valid, if not, the variable is removed from the rule
            if (ruleGenome->getSetNumber(i) >= inVarArray[varNum]->getSetsCount()) {
                inVars--;
            }
            else {
                // Mark the variable as used by the system
                inVarArray[varNum]->setUsedBySystem(true);
                // Retrieve the variable
                inVarsTab[pos] = inVarArray[varNum];
                // Retrieve the corresponding set
                inVarsSetsTab[pos] = ruleGenome->getSetNumber(i);
                pos++;
            }
        }
    }

    // Copy the output variables and the sets numbers present in the genome in a local array
    for (int i = 0, pos = 0; i < outVarsOrig; i++) {
        varNum = ruleGenome->getVarNumber(i+inVarsOrig);
        // Special case. When only one output variable we always use it
        if (sysParams.getNbOutVars() == 1) {
            outVarsTab[0] = outVarArray[0];
            // Check that the set number is valid
            int setNum = ruleGenome->getSetNumber(inVarsOrig);
            if (setNum < outVarsTab[0]->getSetsCount()) {
                outVarsSetsTab[0] = setNum;
                usedOutVars.append(0);
            }
            // Set number invalid, we replace it by a valid set
            else {
                //std::cout << "SET INVALID " << std::endl;
                outVarsSetsTab[0] = setNum % outVarsTab[0]->getSetsCount();
                usedOutVars.append(0);
            }
        }

        // Check that variable number is valid, if not, it is removed from the rule
        else if (varNum == -1) {
            outVars--;
        }
        else if (ruleGenome->getSetNumber(i+inVarsOrig) >= outVarArray[varNum]->getSetsCount()) {
            outVars--;
        }

        // Valid variable number
        else {
            outVarsTab[pos] = outVarArray[varNum];
            // Check that the set number is valid
            int setNum = ruleGenome->getSetNumber(i+inVarsOrig);
            if (setNum < outVarsTab[pos]->getSetsCount()) {
                outVarsSetsTab[pos] = setNum;
                usedOutVars.append(varNum);
                pos++;
            }
            // Set number invalid, we drop the consequent
            else {
                outVars--;
            }
        }
    }

    // Create the textual description of the rule
    description.append(" IF ");
    for (int i = 0; i < inVars; i++)  {
        description.append(inVarsTab[i]->getName());
        description.append(" is ");
        description.append(inVarsTab[i]->getSet(inVarsSetsTab[i])->getName());
        if (i != inVars-1)
            description.append(" AND ");
    }
    description.append(" THEN ");
    for (int i = 0; i < outVars; i++) {
        description.append(outVarsTab[i]->getName());
        description.append(" is ");
        description.append(outVarsTab[i]->getSet(outVarsSetsTab[i])->getName());
        if (i != outVars-1)
            description.append(" AND ");
    }

#if 0

    //DEBUG
    std::cout << "##########In Vars/Sets Tab" << std::endl;
    for (int i = 0; i < inVars; i++) {
        std::cout << " var =  " << inVarsTab[i]->getName().toStdString() << " set = " << inVarsSetsTab[i];
    }
    std::cout << std::endl;


       //DEBUG
    std::cout << "###########out Vars/Sets Tab" << std::endl;
    for (int i = 0; i < outVars; i++)
        std::cout << " var =  " << outVarsTab[i]->getName().toStdString() << " set = " << outVarsSetsTab[i];
    std::cout << std::endl;
#endif
}

/**
  * Alternate constructor. The rule is constructed using the variables passed in the vectors. In this case,
  * variables and sets are existing ones (because they are created via the GUI).
  *
  * @param inVarsVector Vector containing the input variables of the rule.
  * @param inSetsVector Vector containing the input sets of the rule.
  * @param outVarsVector Vector containing the output variables of the rule.
  * @param outSetsVector Vector containing the output sets of the rule.
  */
FuzzyRule::FuzzyRule(QVector<FuzzyVariable*> inVarsVector, QVector<int> inSetsVector,
                    QVector<FuzzyVariable*> outVarsVector, QVector<int> outSetsVector)
{
    inVars = inVarsVector.size();
    outVars = outVarsVector.size();
    inVarsTab = new FuzzyVariable* [inVars];
    outVarsTab = new FuzzyVariable* [outVars];
    inVarsSetsTab = new int[inVars];
    outVarsSetsTab = new int[outVars];

    fireLevel = new double[outVars];

    // Copy the vectors to the local arrays
    for (int i = 0; i < inVars; i++) {
        inVarsTab[i] = inVarsVector.at(i);
        inVarsSetsTab[i] = inSetsVector.at(i);
    }
    for (int i = 0; i < outVars; i++) {
        outVarsTab[i] = outVarsVector.at(i);
        usedOutVars.append(i);
        outVarsSetsTab[i] = outSetsVector.at(i);
    }

    // Create the textual description of the rule
    description.append(" IF ");
    for (int i = 0; i < inVars; i++)  {
        description.append(inVarsTab[i]->getName());
        description.append(" is ");
        description.append(inVarsTab[i]->getSet(inVarsSetsTab[i])->getName());
        if (i != inVars-1)
            description.append(" AND ");
    }
    description.append(" THEN ");
    for (int i = 0; i < outVars; i++) {
        description.append(outVarsTab[i]->getName());
        description.append(" is ");
        description.append(outVarsTab[i]->getSet(outVarsSetsTab[i])->getName());
        if (i != outVars-1)
            description.append(" AND ");
    }
}

/**
  * Destructor.
  */
FuzzyRule::~FuzzyRule()
{

    // MODIF - BUJARD Alexandre - 22.04.2010

    /*
    for(int i=0;i<inVars;i++)
        delete inVarsTab[i];

    for(int i=0;i<outVars;i++)
        delete outVarsTab[i];
    */
    //std::cout << "TEST 1 "<< std::endl;

    delete[] inVarsSetsTab;
    delete[] outVarsSetsTab;
    delete[] inVarsTab;
    delete[] outVarsTab;
    delete[] fireLevel;
    //std::cout << "TEST 2 "<< std::endl;
        //delete[] inVarsSetsTab;
    // FIN - MODIF - BUJARD Alexandre - 22.04.2010

    //inVarsTab = new FuzzyVariable* [inVars];
    //outVarsTab = new FuzzyVariable* [outVars];

}

QString FuzzyRule::getDescription()
{
    return description;
}

/**
  * Evaluate one rule and store the result in each output variable set concerned by the rule.
  */
void FuzzyRule::evaluate()
{
    double eval = DONT_CARE_EVAL_RULE;
#if 0
    // DEBUG
        //DEBUG
    std::cout << "$$ In Vars/Sets Tab" << std::endl;
    for (int i = 0; i < inVars; i++)
        std::cout << " var =  " << inVarsTab[i]->getName().toStdString() << " set = " << inVarsSetsTab[i];
    std::cout << std::endl;

        //DEBUG
    std::cout << "$$ out Vars/Sets Tab" << std::endl;
    for (int i = 0; i < outVars; i++)
        std::cout << " var =  " << outVarsTab[i]->getName().toStdString() << " set = " << outVarsSetsTab[i];
    std::cout << std::endl;
#endif
    //TODO: The operator should be provided as a param
    FuzzyOperator *fOp = new FuzzyOperatorAND;

    // Compute the evaluation for all input variables
    if (inVars == 1) {
        eval = inVarsTab[0]->evaluateSet(inVarsSetsTab[0]);
    }
    else {
        for (int i = 0; i < inVars; i++) {
            // First two antecedents evaluated so they are evaluated together
            // and not with the previous result
            if (i == 0) {
                eval = fOp->operate(inVarsTab[i]->evaluateSet(inVarsSetsTab[i]), inVarsTab[i+1]->evaluateSet(inVarsSetsTab[i+1]));
                // The next antecedent is the third one
                i++;
            }
            else {
                eval = fOp->operate(eval, inVarsTab[i]->evaluateSet(inVarsSetsTab[i]));
            }
        }
    }

    //fireLevel.clear();

    for (int i = 0; i < outVars; i++) {
        fireLevel[i] = 0.0;
        // Aggregation
        // Verify that the set exist. If not, we drop the rule
        if (outVarsSetsTab[i] < outVarsTab[i]->getSetsCount()) {
            if (eval <= 1.0 && eval >= 0.0) {
                // Set evaluation to the corresponding output set
                outVarsTab[i]->setOutputSetValue(outVarsSetsTab[i], eval);
                //fireLevel.insert(i, eval);
                fireLevel[i] = eval;
            }
            else {
                // Dont'care value --> rule dropped
                outVarsTab[i]->setOutputSetValue(outVarsSetsTab[i], 0.0);
            }
        }
    }

    delete fOp;

#if 0
    std::cout << "Rule : In vars : ";
    for (int i = 0; i < inVars; i++) {
        std::cout << inVarsTab[i]->getName().toStdString() << "/" << inVarsSetsTab[i] << "  ";
    }
    std::cout << std::endl;
    std::cout << "Rule : Out vars : ";
    for (int i = 0; i < outVars; i++) {
        std::cout << outVarsTab[i]->getName().toStdString() << "/" << outVarsSetsTab[i] << "  ";
    }
    std::cout << std::endl;

#endif
}
/**
  * Returns the fire level for the corresponding output variable.
  *
  * @param varNum Index of the output variable.
  */
double FuzzyRule::getFireLevel(int varNum)
{
    return fireLevel[varNum];
}

/**
  * Returns the list of used output variables.
  */
QList<int>* FuzzyRule::getUsedOutVars()
{
    return &usedOutVars;
}

/**
  * Returns the number of input variables/sets pairs.
  */
int FuzzyRule::getNbInPairs()
{
    return inVars;
}

/**
  * Returns the number of output variables/sets pairs.
  */
int FuzzyRule::getNbOutPairs()
{
    return outVars;
}

/**
  * Returns the input variable at the corresponding index.
  *
  * @param pos Index of the input variable.
  */
FuzzyVariable* FuzzyRule::getInVarAtPos(int pos)
{
    return inVarsTab[pos];
}

/**
  * Returns the input set at the corresponding index.
  *
  * @param pos Index of the input set.
  */
FuzzySet* FuzzyRule::getInSetAtPos(int pos)
{
    return inVarsTab[pos]->getSet(inVarsSetsTab[pos]);

}

/**
  * Returns the output variable at the corresponding index.
  *
  * @param pos Index of the output variable.
  */
FuzzyVariable* FuzzyRule::getOutVarAtPos(int pos)
{
    return outVarsTab[pos];
}

/**
  * Returns the output set at the corresponding index.
  *
  * @param pos Index of the output set.
  */
FuzzySet* FuzzyRule::getOutSetAtPos(int pos)
{
    return outVarsTab[pos]->getSet(outVarsSetsTab[pos]);
}
