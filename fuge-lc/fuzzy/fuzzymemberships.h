/**
  * @file   fuzzymemberships.h
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

// TODO : voir si on peut hériter de QList pour éviter d'implémenter le méthodes d'accès.
#ifndef FUZZYMEMBERSHIPS_H
#define FUZZYMEMBERSHIPS_H

#include <QList>

#include "fuzzyset.h"

class FuzzyMemberships
{
public:
    FuzzyMemberships();
    virtual ~FuzzyMemberships();
    virtual double evaluateSet(const double value, const int setNum) = 0;
    virtual void addSet(FuzzySet *fSet) = 0;
    virtual int removeSet(int setNum) = 0;
    virtual int removeLastSet() = 0;
    int getSetsCount();
    FuzzySet* getSet(int setNum);

protected:
    QList<FuzzySet*> *setList;
};

#endif // FUZZYMEMBERSHIPS_H
