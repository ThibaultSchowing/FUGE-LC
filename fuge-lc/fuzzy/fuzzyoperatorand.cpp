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
  * @class  FuzzyOperatorAND
  *
  * @brief This class implements the AND rule operator. It simply returns the minimum
  * between the two values passed in parameter.
  */

#include <iostream>

#include "fuzzyoperatorand.h"

/**
  * Constructor.
  */
FuzzyOperatorAND::FuzzyOperatorAND()
{
}

/**
  * Destructor.
  */
FuzzyOperatorAND::~FuzzyOperatorAND()
{
}

/**
  * AND operator.
  *
  * @param x left value
  * @param y rigth value
  */
double FuzzyOperatorAND::operate(double x, double y)
{
    //Check that evaluations are valid. The dont'care ones are dropped.
    if (x == DONT_CARE_EVAL && y == DONT_CARE_EVAL)
        return DONT_CARE_EVAL;
    else if (x == DONT_CARE_EVAL)
        return y;
    else if (y == DONT_CARE_EVAL)
        return x;
    else if (x <= y)
        return x;
    else
        return y;
}
