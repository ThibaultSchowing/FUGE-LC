/**
  * @file   fuzzymembershipsgenome.cpp
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
  * @class FuzzyMembershipsGenome
  *
  * @brief This class represent a genome encoding the memberhips functions of all the variables
  * of a given fuzzy system.
  *
  * @section DESCRIPTION
  *
  * This class represent a genome encoding the memberhips functions of all the variables
  * of a given fuzzy system.
  *
  * The genome is encoded the following way :
  *
  * Genome = {[Param_Set1_V1, ..., Param_Setn_V1], ... , [Param_Set1_Vn, ..., Param_Setn_Vn]}
  */

#include <assert.h>
#include <iostream>

#include "fuzzymembershipsgenome.h"

/**
  * Constructor. Creates an EMPTY genome with the specified format. One
  * of the readGenome methods MUST be called in order to populate the
  * genome.
  *
  *@param nbSets Number of sets per variable.
  *@param nbVars Number of output variables.
  *@param setsCodeSize Number of bits used to code one set's parameters
  */
FuzzyMembershipsGenome::FuzzyMembershipsGenome(int nbInVars, int nbOutVars, int nbInSets,
                                               int nbOutSets, int inSetsPosCodeSize, int outSetsPosCodeSize)
{
    // Verify the validity of the parameters
    assert(nbInVars > 0 && nbOutVars > 0 && nbInSets > 0 && nbOutSets > 0 && inSetsPosCodeSize > 0 && outSetsPosCodeSize > 0);

    this->nbInSets = nbInSets;
    this->nbOutSets = nbOutSets;
    this->nbInVars = nbInVars;
    this->nbOutVars = nbOutVars;
    this->inSetsPosCodeSize = inSetsPosCodeSize;
    this->outSetsPosCodeSize = outSetsPosCodeSize;

    genomeArray = new int[(nbInVars*nbInSets) + (nbOutVars*nbOutSets)];
}

/**
  * Destructor.
  */
FuzzyMembershipsGenome::~FuzzyMembershipsGenome()
{
    delete[] genomeArray;
}

/**
  * Populate the genome by reading a bitString encoding the membership functions.
  *
  *@param bitString Bitstring to be decoded.
  *@param stringSize Size of the Bitstring.
  */
// TODO: Cette méthode est coûteuse, on pourrait ganer en attaquant directement
//       le bitstring lorsqu'on l'utilise. (mais ça reste plus propre)
int FuzzyMembershipsGenome::readGenomeBitString(QBitArray *bitString, int stringSize)
{
    // Ensure that the bit string has the correct length
    assert(stringSize == nbInVars*nbInSets*inSetsPosCodeSize + nbOutVars*nbOutSets*outSetsPosCodeSize);

    int temp = 0;

    // Decode input variables
    for (int i = 0; i < nbInVars; i++) {
        for (int k = 0; k < nbInSets; k++) {
            for (int l = 0; l < inSetsPosCodeSize; l++) {
                temp += bitString->at(i*nbInSets*inSetsPosCodeSize + k*inSetsPosCodeSize + l) << l;
            }
            genomeArray[i*nbInSets + k] = temp;
            temp = 0;
        }
    }

    // Decode output variables
    int outBitIndex = nbInVars*nbInSets*inSetsPosCodeSize;
    int outGenomeIndex = nbInVars*nbInSets;
    for (int i = 0; i < nbOutVars; i++) {
        for (int k = 0; k < nbOutSets; k++) {
            for (int l = 0; l < outSetsPosCodeSize; l++) {
                temp += bitString->at(outBitIndex + i*nbOutSets*outSetsPosCodeSize + k*outSetsPosCodeSize + l) << l;
          }
            genomeArray[outGenomeIndex + i*nbOutSets + k] = temp;
            temp = 0;
        }
    }
    return 0;
}

/**
  * Return the number of input sets.
  */
int FuzzyMembershipsGenome::getNbInSets()
{
    return nbInSets;
}

/**
  * Return the number of output sets.
  */
int FuzzyMembershipsGenome::getNbOutSets()
{
    return nbOutSets;
}

/**
  * Return the position of the given set of a given input variable in the genome.
  *
  * @param varNumber Position of the input variable in the genome
  * @param setNumber Index of the set
  */
int FuzzyMembershipsGenome::getInParam(int varNumber, int setNumber)
{
    return genomeArray[varNumber*nbInSets + setNumber];
}

/**
  * Return the position of the given set of a given output variable in the genome.
  *
  * @param varNumber Position of the output variable in the genome
  * @param setNumber Index of the set
  */
int FuzzyMembershipsGenome::getOutParam(int varNumber, int setNumber)
{
    return genomeArray[nbInVars*nbInSets + (varNumber*nbOutSets + setNumber)];
}

/**
  * Return the number of bits used to code the position of the input sets.
  */
int FuzzyMembershipsGenome::getInSetsPosCodeSize()
{
    return inSetsPosCodeSize;
}

/**
  * Return the number of bits used to code the position of the output sets.
  */
int FuzzyMembershipsGenome::getOutSetsPosCodeSize()
{
    return outSetsPosCodeSize;
}
