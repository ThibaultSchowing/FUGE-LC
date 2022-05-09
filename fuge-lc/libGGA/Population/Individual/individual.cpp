


/**
 * @file individual.cpp
 * @author Yvan Da Silva <yvan.m.silva AT gmail.com>
 * @author IICT Institute for Information and Communication<www.iict.ch>
 * @authorHEIG-VD (Haute école d'inénierie et de gestion) <www.heig-vd.ch>
 * @date 02.2012
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
 * @class Individual
 * @brief An individual from a population
 *
 * Individual describes a single element from a population.
 */

#include "individual.h"

Individual::Individual(quint32 lenght) : PopEntity(lenght)
{
}

Individual::Individual(Genotype *genotype) : PopEntity(genotype)
{
}

Individual::Individual(Individual *individual) : PopEntity(individual)
{
}
