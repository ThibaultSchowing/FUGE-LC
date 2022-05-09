/**
  * @file   fuzzyrulegenome.cpp
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
  * @class FuzzyRuleGenome
  *
  * @brief This class represent a genome encoding a fuzzy rule.
  *
  * @section DESCRIPTION
  *
  * This class represent a genome encoding a fuzzy rule. The input and output variables
  * are encoded as pair of values : one designing the variable number and the other the
  * associated set number. The number of integers encoding the input/output variable and
  * set numbers are defined in the constructor. This flexibility is provided in order to
  * be able to process bitstring coded sets and variables.
  *
  * The genome is encoded the following way :
  *
  * Genome = {[inVar_n,inVar1Set_a], ...,[inVar_m,inVar1Set_b], [outVar_n,inVar1Set_a], ...,[outVar_m,inVar1Set_b]}
  */

#include <iostream>
#include <assert.h>

#include "fuzzyrulegenome.h"
#include "systemparameters.h"

#define EMPTY -2

/**
  * Constructor. Creates an EMPTY genome with the specified format. One
  * of the readGenome methods MUST be called in order to populate the
  * genome.
  *
  *@param inLimit maximu number of input variables in the genome
  *@param inCount Number of input variables encoded in the genome.
  *@param outCount Number of output variables encoded in the genome.
  *@param inCodeSize Number of integers used to encode the input variables number.
  *@param outCodesize Number of integers used to encode the output variables number.
  *@param setCodeSize Number of integers used to encode the sets number.
  */
FuzzyRuleGenome::FuzzyRuleGenome(int inLimit, int inCount, int outCount, int inCodeSize,
                                 int outCodeSize, int inSetCodeSize, int outSetCodeSize)
{
    // Verify the validity of the parameters
    assert(inCount > 0 && outCount > 0 && inCodeSize > 0 && outCodeSize > 0 && inSetCodeSize > 0 && outSetCodeSize > 0);

    this->inLimit = inLimit;
    this->inputCount = inCount;
    this->outputCount = outCount;
    this->inVarCodeSize = inCodeSize;
    this->outVarCodeSize = outCodeSize;
    this->inSetCodeSize = inSetCodeSize;
    this->outSetCodeSize = outSetCodeSize;

    genomeArray = new int[inputCount*2 + outputCount*2];
    // Mark the genome as empty.
    genomeArray[0] = EMPTY;
}

/**
  * Destructor
  */
FuzzyRuleGenome::~FuzzyRuleGenome(){
    delete[] genomeArray;
}

/**
  * Retrieve the numbers of encoded input variables.
  */
int FuzzyRuleGenome::getInputVarCount()
{
    return inLimit;
}

/**
  * Retrieve the numbers of encoded output variables.
  */
int FuzzyRuleGenome::getOutputVarCount()
{
    return outputCount;
}

/**
  * Retrieve the number of a variable encoded in the genome.
  *
  * @param varPos position of the variable;
  */
int FuzzyRuleGenome::getVarNumber(int varPos)
{
    // Ensure that the variable exists
    assert(varPos <= inputCount+outputCount);
    // Ensure that the genome is not empty
    assert(genomeArray[0] != EMPTY);

    return genomeArray[varPos*2];
}

/**
  * Retrieve the number of a set encoded in the genome.
  *
  * @param varPos position of2] = 0; the variable;
  */
int FuzzyRuleGenome::getSetNumber(int varPos)
{
    // Ensure that the variable exists
    assert(varPos <= inputCount+outputCount);
    // Ensure that the genome is not empty
    assert(genomeArray[0] != EMPTY);

    return genomeArray[(varPos*2)+1];
}

/**
  * Populate the genome by reading a bitstring.
  *
  * @param intString intstring to be read;
  * @param stringSize size of the bitstring to be read.
  */
int FuzzyRuleGenome::readGenomeBitString(quint16* bitString, int stringSize)
{

    int varNum;
    int setNum;
    bool usedInVarsTab[inputCount];
    bool usedOutVarsTab[outputCount];

    SystemParameters& sysParams = SystemParameters::getInstance();

    for (int i = 0; i < inputCount; i++)
        usedInVarsTab[i] = false;
    for (int i = 0; i < outputCount; i++)
        usedOutVarsTab[i] = false;

    // Ensure that the int string has the correct length according to in/out variable
    assert(stringSize == inLimit * (inVarCodeSize+inSetCodeSize) + outputCount * (outVarCodeSize+outSetCodeSize));
    // Retrieve the input variables and their associated sets
    for (int i = 0; i < inLimit; i++) {
        varNum = 0;
        setNum = 0;

        if (!sysParams.getFixedVars()) {
            // Retrieve variable number
            for (int k = 0; k < inVarCodeSize; k++) {
                varNum += bitString[i*(inVarCodeSize+inSetCodeSize) + k] << k;
            }
            // Find a valid variable number if necessary (variable already used or index out of range)
            // Out of range
            if (varNum >= inputCount) {
                // Find next available variable
                // std::cout << "invalid var num" << std::endl;
                genomeArray[i*2] = -1;
            }
            // Index valid but already used
            else if (usedInVarsTab[varNum]) {
             //   std::cout << "var already used, Var num = " << varNum << " i (input num) = " << i << std::endl;
                genomeArray[i*2] = -1;
            }
            // Valid and available
            else {
                // Store variable number
                genomeArray[i*2] = varNum;
                usedInVarsTab[varNum] = true;
            }
        }
        else {
            genomeArray[i*2] = i;
        }

        // Retrieve set number
        for (int k = 0; k < inSetCodeSize; k++) {
            setNum += bitString[i*(inVarCodeSize+inSetCodeSize) + inVarCodeSize + k] << k;
        }
        // Store set number
        genomeArray[i*2 + 1] = setNum;
    }


    // Index of the first bit of the first output variable
    int outputStart = inLimit * (inVarCodeSize+inSetCodeSize);

    // Retrieve the output variables and their associated sets
    for (int i = 0; i < outputCount; i++) {

        varNum = 0;
        setNum = 0;
        
        int base = inLimit*2 + i*2;

        // Retrieve variable number
        for (int k = 0; k < outVarCodeSize; k++) {
            varNum += bitString[outputStart + i*(outVarCodeSize+outSetCodeSize) + k] << k;
        }

        // Find a valid variable number if necessary (variable already used or index out of range)
        // Out of range
        if (varNum >= outputCount) {
            // Find next available variable
            genomeArray[base] = -1;
        }
        // Index valid but already used
        else if (usedOutVarsTab[varNum]) {
            genomeArray[base] = -1;
        }
        // Valid and available
        else {
            // Store variable number
            genomeArray[base] = varNum;
            usedOutVarsTab[varNum] = true;
        }

        // Retrieve set number
        for (int k = 0; k < outSetCodeSize; k++) {
            setNum += bitString[outputStart + i*(outVarCodeSize+outSetCodeSize) + outVarCodeSize + k] << k;
        }

        // Store set number
        genomeArray[base + 1] = setNum;
    }

    // DEBUG
    /*
    std::cout << " ------------------------------------->  Rules genome : ";
    for (int i = 0; i < inLimit*2 + outputCount*2; i++) {
        std::cout << "/" << genomeArray[i];
    }
    std::cout << std::endl;*/

    return 0;
}

/**
  * Populate the genome by reading an intstring.
  *
  * @param intString intstring to be read;
  * @param stringSize size of the bitstring to be read.
  */
int FuzzyRuleGenome::readGenomeIntString(quint16* intString, int stringSize)
{
    // Ensure that the int string has the correct length according to in/out variables
    assert(stringSize == inputCount*2 + outputCount*2);

    for (int i = 0; i < stringSize; i++) {
        genomeArray[i] = intString[i];
    }

    return 0;
}
