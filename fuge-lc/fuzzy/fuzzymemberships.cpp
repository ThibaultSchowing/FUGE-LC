/**
  * @file   fuzzymemberships.cpp
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
  * @class FuzzyMemberships
  * @brief Abstract class representing a set of membership functions (i.e. a set of fuzzy sets)
  * attached to a variable. So only one FuzzyMemberships is attached to a variable.
  */


#include <iostream>

#include "fuzzymemberships.h"

/**
  * Constructor. It initialises the set list.
  */
FuzzyMemberships::FuzzyMemberships()
{
    setList = new QList<FuzzySet*>;
}

/**
  * Destructor.
  */
FuzzyMemberships::~FuzzyMemberships()
{
   for (int i = 0; i < setList->size(); i++) {
       delete setList->at(i);
   }
   delete setList;
}

/**
  * Retrieve the number of fuzzy sets contained in the FuzzyMemberships.
  */
int FuzzyMemberships::getSetsCount()
{
    return setList->size();
}

/**
  * Return a pointer on a specific fuzzy set.
  *
  * @param setNum The number of the set to be retrieved.
  */
FuzzySet* FuzzyMemberships::getSet(int setNum)
{

    //qDebug("set list size = %d", setList->size());

    //TODO: essaier de supprimer les protections pour voir la diff de vitesse
    /*if (setNum > setList->size()-1) {
        std::cout << "Error : cannot access set : set number out of range !" << std::endl;
        // TODO : utiliser une exception plutôt
        throw;
        return NULL;
    }
    else*/
        return setList->at(setNum);
}
