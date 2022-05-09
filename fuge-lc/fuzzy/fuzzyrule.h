/**
  * @file   fuzzyrule.h
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
  * @class FuzzyRule
  *
  * @brief This class represent a fuzzy rule.
  *
  * @section DESCRIPTION
  *
  * This class represent a fuzzy rule. The rule is composed of [input/output variables - sets] pairs.
  * The rule is created by reading a rule genome and fetching the corresponding variables in an array
  * of variables passed in parameter. The variable array contains all the existing variables of the
  * system. The variables used in the rule are selected according to the rule genome and stored in
  * internal arrays.
  *
  * Once the rule has been evaluated, the result is stored in the corresponding set of the corresponding
  * variable for later defuzzification.
  */

#ifndef FUZZYRULE_H
#define FUZZYRULE_H

#include <QVector>

#include "fuzzyvariable.h"
#include "fuzzyrulegenome.h"

class FuzzyRule
{
public:
    FuzzyRule(FuzzyVariable** inVarArray, FuzzyVariable** outVarArray, FuzzyRuleGenome* ruleGenome/*, FuzzyOperator* fuzzyOp*/);
    FuzzyRule(QVector<FuzzyVariable*> inVarsVector, QVector<int> inSetsVector,
              QVector<FuzzyVariable*> outVarsVector, QVector<int> outSetsVector);
    virtual ~FuzzyRule();

    void evaluate();
    double getFireLevel(int varNum);
    QList<int>* getUsedOutVars();
    QString getDescription();
    int getNbInPairs();
    int getNbOutPairs();
    FuzzyVariable* getInVarAtPos(int pos);
    FuzzySet* getInSetAtPos(int pos);
    FuzzyVariable* getOutVarAtPos(int pos);
    FuzzySet* getOutSetAtPos(int pos);

private:
    int inVars;
    FuzzyVariable** inVarsTab;
    FuzzyVariable** outVarsTab;
    int* inVarsSetsTab;
    int* outVarsSetsTab;
    long long pad1;
    long long pad2;
    int outVars;
    //QList<double> fireLevel;
    double* fireLevel;
    QList<int> usedOutVars;
    QString description;
};

#endif // FUZZYRULE_H
