/**
  * @file   fuzzymembershipssingle.cpp
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
  * @class FuzzyMembershipsSingle
  *
  * @brief This class implements a singleton type of memberships functions.
  */

#include <iostream>

#include "fuzzymembershipssingle.h"

FuzzyMembershipsSingle::FuzzyMembershipsSingle()
{
}
/**
  * Evaluate a specific set given a certain input value.
  *
  * @param value Input value for the evaluation.
  * @param setNum Number of the set to be evaluated.
  */
double FuzzyMembershipsSingle::evaluateSet(double value, int)
{
    return value;
}

/**
  * Add a fuzzy set to the memberships.
  *
  * @param fSet Set to add.
  */
void FuzzyMembershipsSingle::addSet(FuzzySet *fSet)
{
    setList->append(fSet);
}

/**
  * Remove a fuzzy set to the memberships.
  *
  * @param setNum Number of the set to be removed.
  */
int FuzzyMembershipsSingle::removeSet(int setNum)
{
    if (setNum > setList->size()) {
        std::cout << "Error : cannot remove set : set number out of range !" << std::endl;
        return -1;
    }
    else {

        delete setList->at(setNum);
        setList->removeAt(setNum);
        return 0;
    }
}

/**
  * Remove the last fuzzy set to the memberships.
  *
  */
int FuzzyMembershipsSingle::removeLastSet()
{
    setList->removeLast();
    return 0;
}
