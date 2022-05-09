/**
  * @file   defuzzmethodsingleton.h
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
  * @class DefuzzMethodSingleton
  *
  * @brief This class implements the singleton defuzzification algorithm.
  */

#ifndef DEFUZZMETHODSINGLETON_H
#define DEFUZZMETHODSINGLETON_H

#include "defuzzmethod.h"
class DefuzzMethodSingleton : public DefuzzMethod
{
public:
    DefuzzMethodSingleton();
    double defuzzVariable(FuzzyVariable* fVar);
};

#endif // DEFUZZMETHODSINGLETON_H
