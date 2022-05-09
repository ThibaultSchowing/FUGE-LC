/**
  * @file   coevcooperator.cpp
  * @author Jean-Philippe Meylan <jean-philippe.meylan_at_heig-vd.ch>
  * @author ReDS (Reconfigurable and embedded digital systems) <www.reds.ch>
  * @author HEIG-VD (Haute école d'ingénierie et de gestion) <www.heig-vd.ch>
  * @date   03.2010
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
  * @class CoevCooperator
  * @brief This class implements a coevolution cooperator. A cooperator is an individual who
  * has been selected for its good fitness.
  */

#include "coevcooperator.h"

CoevCooperator::CoevCooperator()
{
}

/**
  * Alternate constructor
  *
  * @param coop Handle to an existing individual
  * @param fitness Fitness of this individual
  */
CoevCooperator::CoevCooperator(PopEntity *coop, double fitness)
{
    this->coop = coop;
    this->fitness = fitness;
}

CoevCooperator::~CoevCooperator()
{
}

/**
  * Return a handle to the cooperator
  *
  */
PopEntity *CoevCooperator::getCoop()
{
    return coop;
}

/**
  * Return the fitness of the cooperator
  *
  */
double CoevCooperator::getFitness()
{
    return fitness;
}

/**
  * Affect a new cooperator
  *
  * @param indiv Handle to the new cooperator
  */

void CoevCooperator::setCoop(PopEntity *indiv)
{
    coop = indiv;
}

/**
  * Set the fitness of the cooperator
  *
  * @param fit Fitness to be set
  */
void CoevCooperator::setFitness(double fit)
{
    fitness = fit;
}
