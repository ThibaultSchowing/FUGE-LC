/**
  * @file   fuzzyset.cpp
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
  * @class FuzzySet
  *
  * @brief This class represent a simple fuzzy coco set. It is defined by a name and a parameter
  * which indicates the position of the set. See FuzzyCocoMemberships for more detail.
  */

#include <iostream>

#include "fuzzyset.h"

/**
  * Constructor.
  */
FuzzySet::FuzzySet(QString setName, double setPosition, int setNum)
{
    name = setName;
    position = setPosition;
    number = setNum;
    evaluation = 0.0;
}

/**
  * Destructor.
  */
FuzzySet::~FuzzySet()
{
}

/**
  * Retrieve the name of the set.
  */
QString FuzzySet::getName()
{
    return name;
}

/**
  * Set the name of the set.
  */
void FuzzySet::setName(QString newName)
{
    name = newName;
}

/**
  * Retrieve the position of the set.
  */
double FuzzySet::getPosition()
{
    return position;
}

/**
  * Set the position of the set.
  */
void FuzzySet::setPosition(double pos)
{
    position = pos;
}

/**
  * Attribute an evaluation to this set. For now only maximum type aggregation
  * is supported.
  *
  * @param eval The evaluation to be attributed to the set.
  */
//TODO: Supporter plusieurs méthodes d'aggérgation
void FuzzySet::setEval(double eval)
{
    // Aggregation singleton
    evaluation += eval;
}

/**
  * Reinitialise the evaluation of this set.
  */
void FuzzySet::clearEval()
{
    evaluation = 0.0;
}

/**
  * Retrieve the evaluation attributed to this set.
  */
double FuzzySet::getEval()
{
    return evaluation;
}

/**
  * Retrieve the number attributed to this set.
  */
int FuzzySet::getNumber()
{
    return number;
}
