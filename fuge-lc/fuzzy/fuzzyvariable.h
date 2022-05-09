/**
  * @file   fuzzyvariable.h
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
  * @class FuzzyVariable
  *
  * @brief This class represent a fuzzy variable. It can be either an input or an output variable
  * following the "output" flag. The memberships associated with this variable are hold
  * by the "memberships" member (an instance of "FuzzyMemberships") which is a set of membership
  * functions. If the variable is an output, the "defuzz" method is used to retrieve the defuzzified
  * value after the rules of the fuzzy system have been evaluated.
  */

#ifndef FUZZYVARIABLE_H
#define FUZZYVARIABLE_H

#include <QString>
#include <QList>

#include "fuzzyset.h"
#include "fuzzymemberships.h"

typedef enum {coco, singleton} membership_t;

class FuzzyVariable
{
public:
    FuzzyVariable(QString varName, membership_t membType);
    virtual ~FuzzyVariable();

    void setOutput(bool output);
    void setUsedBySystem(bool usedBySystem);
    bool isOutput(void);
    bool isUsedBySystem(void);
    void addSet(FuzzySet *fSet);
    int removeSet(int setNum);
    int removeLastSet();
    int getSetsCount();
    double evaluateSet(const int setNum);
    FuzzySet* getSet(int setNum);
    FuzzySet* getSetByName(QString setName);
    int getSetIndexByName(QString setName);
    void setInputValue(double value);
    void setMissingVal(bool isMissing);
    void setOutputSetValue(int setNum, double outputValue);
    QString getName();
    double defuzz(int precision);
    FuzzyMemberships* getMemberships();
    void setDefaultRule(int setNum, double value);

private:
    QString name;
    bool output;
    bool usedBySystem;
    bool missingVal;
    FuzzyMemberships *memberships;
    double inputValue;
};

#endif // FUZZYVARIABLE_H
