/**
 * @file individual.cpp
 * @author Yvan Da Silva <yvan.m.silva AT gmail.com>
 * @author IICT Institute for Information and Communication<www.iict.ch>
 * @author HEIG-VD (Haute école d'inénierie et de gestion) <www.heig-vd.ch>
 * @date 02.2012
 * @section LICENSE
 *
 * This application is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
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

#ifndef COEVCOOPERATORS_H
#define COEVCOOPERATORS_H

#include "popentity.h"

class CoevCooperator
{
public:
    CoevCooperator();
    CoevCooperator(PopEntity *coop, double fitness);
    virtual ~CoevCooperator();

    PopEntity *getCoop();
    double getFitness();
    void setCoop(PopEntity *indiv);
    void setFitness(double fit);
    inline void setId(int value) {id = value;}
    inline int getId() {return id;}

private:
    PopEntity *coop;
    double fitness;
    int id;
};

#endif // COEVCOOPERATOR_H
