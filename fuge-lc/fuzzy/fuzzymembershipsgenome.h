/**
  * @file   fuzzymembershipsgenome.h
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

#ifndef FUZZYMEMBERSHIPSGENOME_H
#define FUZZYMEMBERSHIPSGENOME_H

#include <QtGlobal>
#include <QBitArray>
#include <QDebug>

class FuzzyMembershipsGenome
{
public:
    FuzzyMembershipsGenome(int nbInVars, int nbOutVars, int nbInSets,
                           int nbOutSets, int inSetsPosCodeSize, int outSetsPosCodeSize);
    virtual ~FuzzyMembershipsGenome();

    int readGenomeBitString(QBitArray *bitString, int stringSize);
    int readGenomeIntString(quint16* intString, int stringSize);
    int getNbInSets();
    int getNbOutSets();
    int getInParam(int varNumber, int setNumber);
    int getOutParam(int varNumber, int setNumber);
    int getInSetsPosCodeSize();
    int getOutSetsPosCodeSize();

private:
    int* genomeArray;
    int nbInSets;
    int nbOutSets;
    int nbInVars;
    int nbOutVars;
    int inSetsPosCodeSize;
    int outSetsPosCodeSize;
};

#endif // FUZZYMEMBERSHIPSGENOME_H
