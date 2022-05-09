/**
  * @file   defuzzmethod.cpp
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
  * @class DefuzzMethod
  *
  * @brief Abscract class representing a defuzzification method.
  */

#include <assert.h>

#include "defuzzmethod.h"

/**
  * Constructor.
  */
DefuzzMethod::DefuzzMethod()
{
    // Default precision
    precision = 100;
}

/**
  * Destructor.
  */
DefuzzMethod::~DefuzzMethod()
{
}

/**
  * Set the precision of the defuuzification algorithm.
  *
  * @param prec Precision.
  */
void DefuzzMethod::setPrecision(int prec)
{
    // Ensure the precision is 1 at least (although a value of 1 doesn't produce a credible result!)
    /**
      * Precision, is not the right value to be tested, should be prec which is the new value for precision.
      * Yvan Da Silva
      */
    assert(precision > 0);

    precision = prec;
}

/**
  * Get the precision of the defuuzification algorithm.
  */
int DefuzzMethod::getPrecision()
{






    return precision;
}
