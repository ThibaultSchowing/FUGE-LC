/**
  * @file   defuzzmethodcoa.cpp
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
  * @class DefuzzMethodCOA
  *
  * @brief This class implements the COA (Center of Aeras) defuzzification algorithm.
  * WARNING : NEEDS TO BE TESTED
  */

#include <assert.h>
#include <iostream>

#include "defuzzmethodcoa.h"

/**
  * Constructor.
  */
DefuzzMethodCOA::DefuzzMethodCOA()
{
}

/**
  * Destructor.
  */
DefuzzMethodCOA::~DefuzzMethodCOA()
{
}

/**
  * Compute the defuzzified value for a given output variable.
  * The rules containing this variable must have been evaluated first.
  *
  * @param fVar Fuzzy variable to be defuzzified.
  */
double DefuzzMethodCOA::defuzzVariable(FuzzyVariable* fVar)
{
        int setsCount = fVar->getSetsCount();
        int nbSteps = this->getPrecision();
        double step;
        double valuesTab[nbSteps ];
        double xValue;
        double yValue;
        double thisSetEval;
        double defuzzValue = 0;
        FuzzySet* set;

        // Ensure that we have at least on set on this output variable
        assert(setsCount > 0);

        // Initialize the fuzzy array
        for (int i = 0; i < nbSteps; i++)
            valuesTab[i] = 0;

        // We divide the universe of discourse in nbSteps steps.
        step = /*fVar->getSet(setsCount-1)->getPosition()*/(double) 1.0 / nbSteps; // TODO: @@@@@@ prendre la valeur max et non la dernière pos

        // TODO : on peut suprrimer le calcul si on est en dehors du set (Joël)
        for (int i = 0; i < setsCount; i++) {
            set = fVar->getSet(i);
            thisSetEval = set->getEval();
            //DEBUG
            //std::cout << " $$ Set eval : setnum = " << i << " , eval = "<< thisSetEval << std::endl;
            for (int j = 0; j < nbSteps ; j++) {
                xValue = j * step;
                // We take the lower value between the set evaluation value and the membership curve value
                yValue = fVar->getMemberships()->evaluateSet(xValue, i);
                yValue = yValue > thisSetEval ? thisSetEval : yValue;
                // We take the higher value between the previous yValue and this one
                if (yValue > valuesTab[j])
                    valuesTab[j] = yValue;
            }
        }

        // COA defuzzification
        double div = 0;

        for (int i = 0; i < nbSteps ; i++) {
            defuzzValue += valuesTab[i] * i;
            div += valuesTab[i];
        }

        if (div == 0.0)
            div = 1.0;

        return defuzzValue / (div*nbSteps);
}
