/**
  * @file   fuzzyrulegenome.h
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

#ifndef FUZZYRULEGENOME_H
#define FUZZYRULEGENOME_H

#include <QtGlobal>
#include <QBitArray>

class FuzzyRuleGenome
{
public:
    FuzzyRuleGenome(int inLimit, int inCount, int outCount, int inCodeSize,
                    int outCodeSize, int inSetCodeSize, int outSetCodeSize);
    virtual ~FuzzyRuleGenome();

    int readGenomeBitString(quint16* bitString, int stringSize);
    int readGenomeIntString(quint16* intString, int stringSize);
    int getInputVarCount();
    int getOutputVarCount();
    int getVarNumber(int varPos);
    int getSetNumber(int varPos);

private:
    int* genomeArray;
    int inLimit;
    int inputCount;
    int outputCount;
    int inVarCodeSize;
    int outVarCodeSize;
    int inSetCodeSize;
    int outSetCodeSize;

    int elimDuplicateVars(int nbVars, int position);
};

#endif // FUZZYRULEGENOME_H
