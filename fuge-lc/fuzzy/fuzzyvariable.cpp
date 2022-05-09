/**
  * @file   fuzzyvariable.cpp
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


#include "fuzzyvariable.h"
#include "fuzzymembershipscoco.h"

#include "fuzzymembershipssingle.h"
#include "defuzzmethod.h"
#include "defuzzmethodcoa.h"
#include "defuzzmethodsingleton.h"

#include <iostream>
#include <assert.h>

#define MISSINGVAL 999.0

using std::cout; using std::endl;

/**
  * Constructor.
  *
  * @param varName Name of the variable
  */
FuzzyVariable::FuzzyVariable(QString varName, membership_t membType)
{
    name = varName;
    output = false;
    usedBySystem = false;
    missingVal = true;

    switch (membType) {
        case coco:
            memberships = new FuzzyMembershipsCoco;
        break;
        case singleton:
            memberships = new FuzzyMembershipsSingle;
        break;
        default:
            memberships = new FuzzyMembershipsCoco;
        break;
   }
}

/**
  * Destructor.
  */
FuzzyVariable::~FuzzyVariable()
{
    delete memberships;
}

/**
  * Define this variable as an output variable.
  * @param output Output flag.
  */
void FuzzyVariable::setOutput(bool output)
{
    this->output = output;
}

/**
  * Define this variable as used by the fuzzy system.
  *
  * @param usedBySystem Used by the syste flag.
  */
void FuzzyVariable::setUsedBySystem(bool usedBySystem)
{
    this->usedBySystem = usedBySystem;
}

/**
  * Check if this variable is used by the fuzzy system.
  */
bool FuzzyVariable::isUsedBySystem(void)
{
    return usedBySystem;
}

/**
  * Return true if this variable is an output variable.
  */
bool FuzzyVariable::isOutput(void)
{
    return output;
}

/**
  * Add a set to this variable.
  *
  * @param fSet Fuzzy set to attach.
  */
void FuzzyVariable::addSet(FuzzySet *fSet)
{
    memberships->addSet(fSet);
}

/**
  * Remove a set from this variable.
  *
  * @param setNum Number of the set to be removed.
  */
int FuzzyVariable::removeSet(int setNum)
{
    return memberships->removeSet(setNum);
}

/**
  * Remove the last set set from this variable.
  *
  */
int FuzzyVariable::removeLastSet()
{
    return memberships->removeLastSet();
}

/**
  * Get the number of sets attached to this variable.
  */
int FuzzyVariable::getSetsCount()
{
    return memberships->getSetsCount();
}

/**
  * Perform an evaluation of the set value according to the variable
  * input value. The input value must have been set before calling
  * this method.
  *
  * @param setNum Number of the set to be evaluated.
  */
double FuzzyVariable::evaluateSet(const int setNum)
{
    if (this->missingVal) {
        return MISSINGVAL;
    }

    // Set number is dont'care
    // OR set number higher than existing sets --> interpreted as dont'care
    if (setNum < 0 || setNum >= memberships->getSetsCount()) {
        return MISSINGVAL;
    }

    // Compute evaluation
    return memberships->evaluateSet(inputValue, setNum);
}

/**
  * Return a pointer to an attached set.
  *
  * @param setNum Number of the set to be retrieved.
  */
FuzzySet* FuzzyVariable::getSet(int setNum)
{
    return memberships->getSet(setNum);
}

/**
  * Return a pointer to an attached set.
  *
  * @param setName Name of the set to be retrieved.
  */
FuzzySet* FuzzyVariable::getSetByName(QString setName)
{
    for (int i = 0; i < this->getSetsCount(); i++) {
        if (this->getSet(i)->getName() == setName)
            return this->getSet(i);
    }
    return NULL;
}


/**
  * Return the index of an attached set (for example : by default MF0 is 0, MF1 is 1, ...).
  *
  * @param setName Name of the set to be retrieved.
  */
int FuzzyVariable::getSetIndexByName(QString setName)
{
    for (int i = 0; i < this->getSetsCount(); i++) {
        if (this->getSet(i)->getName() == setName)
            return i;
    }
    return -1;
}

/**
  * Set an input value for this variable. This must be done before
  * performing an evaluation.
  *
  * @param inputVal Input value
  */
void FuzzyVariable::setInputValue(double inputVal)
{
    inputValue = inputVal;
    this->missingVal = false;
}

/**
  * Set the missing status of a variable
  *
  * @param isMissing is variable missing
  */
void FuzzyVariable::setMissingVal(bool isMissing)
{
    this->missingVal = isMissing;
}

/**
  * Attach an evaluated output value to the corresponding set.
  *
  * @param setNum Number of the set which has been evaluated.
  * @param outputValue Corresponding computed output value.
  */
void FuzzyVariable::setOutputSetValue(int setNum, double outputValue)
{
    if (output)
        memberships->getSet(setNum)->setEval(outputValue);
    else {
        cout << "Error: cannot set evaluation : not an output variable" << endl;
    }
}

/**
  * Return the name of the variable.
  */
QString FuzzyVariable::getName()
{
    return name;
}

/**
  * Return the meberships collection associated with this variable.
  */
FuzzyMemberships* FuzzyVariable::getMemberships()
{
    return memberships;
}

/**
  * Compute the defuzzificated value for this variable. The variable
  * must be an output variable and its sets must have been evaluated
  * first.
  *
  * @param precision Precision of the defuzzification process. If the
  * defuzzification method choosed is COA, it represents the step (on x-axis)
  * of the discrete sum.
  */
double FuzzyVariable::defuzz(int)
{
    if (output) {

        double defuzzValue = -1.0;
#if 0
        DefuzzMethod* defuzzMethod = new DefuzzMethodCOA();

        defuzzMethod->setPrecision(precision);
        defuzzValue =  defuzzMethod->defuzzVariable(this);
        delete defuzzMethod;

        return defuzzValue;
#endif

        DefuzzMethod* defuzzMethod = new DefuzzMethodSingleton();
        defuzzValue =  defuzzMethod->defuzzVariable(this);
        delete defuzzMethod;


        return defuzzValue;

    }
    else {
        cout << "Error : not an output variable : cannont compute defuzzification !" << endl;
        return 0.0;
    }
}

/**
  * Set the default rule activation level to the corresponding set.
  *
  * @param setNum Index of the set referred by the default rule.
  * @param value Level of default rule activation.
  */
void FuzzyVariable::setDefaultRule(int setNum, double value)
{
    this->getSet(setNum)->setEval(value);
}
