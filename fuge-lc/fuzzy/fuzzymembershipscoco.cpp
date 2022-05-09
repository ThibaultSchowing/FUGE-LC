/**
  * @file   fuzzymembershipscoco.cpp
  * @author Jean-Philippe Meylan <jean-philippe.meylan_at_heig-vd.ch>
  * @author ReDS (Reconfigurable and embedded digital systems) <www.reds.ch>
  * @author HEIG-VD (Haute école d'ingénierie et de gestion) <www.heig-vd.ch>
  * @date   07.2009
  * @section LICENSE
  *
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
  *
  * @class FuzzyMembershipsCoco
  * @brief This class implements a "Fuzzy Coco" type of memberships functions.
  *
  * @section DESCRIPTION
  *
  * This class implements a "Fuzzy Coco" type of memberships functions. The "Fuzzy
  * Coco" memberships have always the same format :
  *
  * ______               ________
  *       \   /\   /\   /          The first and the last sets are trapezoidal
  *        \ /  \ /  \ /           and all internal sets have a triangular shape.
  *         X    X    X
  *        / \  / \  / \
  * ______/___\/___\/___\________
  */

#include <iostream>
#include <assert.h>

#include "fuzzymembershipscoco.h"

using std::cout; using std::endl;

/**
  * Constructor
  */
FuzzyMembershipsCoco::FuzzyMembershipsCoco()
{
}

/**
  * Destructor
  */
FuzzyMembershipsCoco::~FuzzyMembershipsCoco()
{
}

/**
  * Evaluate a specific set given a certain input value.
  *
  * @param value Input value for the evaluation.
  * @param setNum Number of the set to be evaluated.
  */
double FuzzyMembershipsCoco::evaluateSet(const double value, const int setNum)
{
    //TODO: essaier de supprimer les protections pour voir la diff de vitesse
    //TODO: zone critique en temps à essayer d'optimiser

    const int lastSetNum = setList->size() - 1;

    if (setNum > lastSetNum) {
        cout << "Error : cannot evaluate set : set number out of range !" << endl;
        // TODO : utiliser une exception plutôt
        throw;
        return -1.0;
    }

    const double position = setList->at(setNum)->getPosition();

    //In any case
    if(value == position)
        return 1.0;


    // Last set : part after pn = 1 AND Internal Set with value < than position
    if (setNum == lastSetNum || (setNum != 0 && value < position)) {

        if (value > position)
            return 1.0;

        //The set num is not the first!!!
        const double beforePosition = setList->at(setNum-1)->getPosition();

        if (value <= beforePosition)
            return 0.0;
        else
            return (value - beforePosition) / (position - beforePosition);

    } else {

        //It's the first set num
        if (value < position)
            return 1.0;

        // the set num is not the last (tested just before), so I can do that without checking setList size
        const double afterPosition = setList->at(setNum+1)->getPosition();

        if (value >= afterPosition)
            return 0.0;
        else
            return 1.0 - ((value-position) / (afterPosition - position));
    }
}

/**
  * Add a fuzzy set to the memberships.
  *
  * @param fSet Set to add.
  */
void FuzzyMembershipsCoco::addSet(FuzzySet *fSet)
{
    // Verify that the set parameter is greater than previous' set one (if exists)
    // which is necessary for Coco memberships
    /*int setsCount = this->getSetsCount();
    if (setsCount > 0) {
        //DEBUG
        if (fSet->getPosition() < this->getSet(this->getSetsCount()-1)->getPosition())
            std::cout << "Failure : prev pos = " << this->getSet(this->getSetsCount()-1)->getPosition() << " this pos = " << fSet->getPosition() << " setNum = " << fSet->getName().toStdString() << std::endl;
        assert(fSet->getPosition() >= this->getSet(this->getSetsCount()-1)->getPosition());
    }*/

    setList->append(fSet);
}

/**
  * Remove a fuzzy set to the memberships.
  *
  * @param setNum Number of the set to be removed.
  */
int FuzzyMembershipsCoco::removeSet(int setNum)
{
    if (setNum > setList->size()) {
        cout << "Error : cannot remove set : set number out of range !" << endl;
        // TODO : utiliser une exception plutôt
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
int FuzzyMembershipsCoco::removeLastSet()
{
    setList->removeLast();
    return 0;
}
