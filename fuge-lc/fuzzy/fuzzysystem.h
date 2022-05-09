/**
  * @file   fuzzysystem.h
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
  * @class FuzzySystem
  *
  * @brief This class represent a complete fuzzy system and holds also the data to be processed
  * by the system. Once the system is created, the rules and the memberships functions must
  * loaded before making an evaluation with the data.
  */

#ifndef FUZZYSYSTEM_H
#define FUZZYSYSTEM_H
#include <cmath>
#include <algorithm>
#include <unistd.h>

#include <QList>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include <QTime>
#include <QDir>
#include <QStringList>
#include <QVector>
#include <QHash>
#include <iostream>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>

#include "fuzzyset.h"
#include "systemparameters.h"
#include "assert.h"
#include "coevstats.h"

#include "fuzzyvariable.h"
#include "fuzzyrule.h"
#include "fuzzyrulegenome.h"
#include "fuzzymembershipsgenome.h"

typedef enum {truePos, trueNeg, falsePos, falseNeg} evalResult_t;

class FuzzySystem : public QObject
{
    Q_OBJECT

public:
    FuzzySystem();
    virtual ~FuzzySystem();
    void setParameters(int nbRules, int nbVarPerRule, int nbOutVars, int nbInSets, int nbOutSets, int inVarsCodeSize,
                         int outVarsCodeSize, int inSetsCodeSize, int outSetsCodeSize, int inSetsPosCodeSize, int outSetsPosCodeSize);

    void loadData(QList<QStringList>* systemData);
    void loadRulesGenome(FuzzyRuleGenome** ruleGenArray, int* defaultRuleSet);
    void loadMembershipsGenome(FuzzyMembershipsGenome* membGen);
    float evaluateFitness();
    QVector<float> doEvaluateFitness();
    void reset();
    int getNbRules();
    int getNbVarPerRule();
    int getNbInVars();
    int getNbOutVars();
    int getNbInSets();
    int getNbOutSets();
    int getInVarsCodeSize();
    int getOutVarsCodeSize();
    int getInSetsCodeSize();
    int getOutSetsCodeSize();
    int getInSetsPosCodeSize();
    int getOutSetsPosCodeSize();
    int getMembershipsBitStringSize();
    int getRuleBitStringSize();
    int getRuleMaxBitStringSize();
    int getDefaultRulesBitStringSize();
    void setNbInSets(int num);
    void setNbOutSets(int num);
    QString getSystemDescritpion();
    void updateSystemDescription();
    float threshold(int outVar, float value);
    float getFitness();
    void setFitness(float fit);
    float getSensitivity();
    float getSpecificity();
    float getAccuracy();
    float getPpv();
    float getRmse();
    // MODIF - BUJARD - 18.03.2010
    float getRrse    (){return rrse;    }
    float getRae     (){return rae;     }
    // FIN - MODIF - BUJARD - 18.03.2010
    /* MODIF - Bujard - 2.12.2010 */
    //bool  getSizeBonus;
    float getMse () {return mse;}
    float getDistanceThreshold () {return distanceThreshold;}
    float getDistanceMinThreshold () {return distanceMinThreshold;}
    float getDontCare () {return dontCare;}
    float getOverLearn (){return overLearn;}
    /* FIN - MODIF - Bujard - 2.12.2010 */
    FuzzyVariable* getInVar(int varnum);
    FuzzyVariable* getOutVar(int varnum);
    FuzzyVariable* getVar(int varnum);
    FuzzyVariable* getInVarByName(QString name);
    FuzzyVariable* getOutVarByName(QString name);
    float getVarValMin(int varnum);
    float getVarValMax(int varnum);
    FuzzyRule* getRule(int ruleNum);
    void replaceRule(int ruleNum, FuzzyRule* newRule);
    QVector<int> getDefaultRules();
    void updateDefaultRule(int outVarNum,  int defaultSet);
    void printVerboseOutput();

    QMutex mutex;

private:
    QList<QStringList>* systemData;
    QString systemDescription;
    FuzzyVariable** inVarArray;
    FuzzyVariable** outVarArray;
    FuzzyRule** rulesArray;
    QVector<float> defuzzValues;
    QVector<float> threshValues;
    QVector<float> computedResults;
    float** results;
    int nbVars;
    int nbInVars;
    int nbOutVars;
    int nbInSets;
    int nbOutSets;
    int inVarsCodeSize;
    int outVarsCodeSize;
    int inSetsCodeSize;
    int outSetsCodeSize;
    int inSetsPosCodeSize;
    int outSetsPosCodeSize;
    int nbRules;
    int nbVarPerRule;
    int nbSamples;
    QVector<int> defaultRulesSets;
    bool membershipsLoaded;
    bool rulesLoaded;
    bool dataLoaded;
    struct universeBounds {
        float valMin;
        float valMax;
    };
    universeBounds* varUniverseArray;
    float fitness;
    float sensitivity;
    float specificity;
    float accuracy;
    float ppv;
    float rmse;
    float rrse;
    float rae;

    float mse;
    float distanceThreshold;
    float distanceMinThreshold;
    float squareError;
    float errorSum;
    float rmseError;
    float sumDistW;
    float sumW;
    float distMinAbove;
    float distMinBelow;
    float maxActualValue;
    float dontCare;
    float overLearn;
    int* arrRuleFired; // chaque case correspond aux nombre de fois ou la règle est enclenché pour un certain dataSet
    int* arrRuleWinner; // chaque case correspond aux nombre de fois ou la règle est la gagnante
    //les min et max et chaque sortie. outMinMax[sortie][0] pour le min et outMinMax[sortie][1] pour le max.
    double** outMinMax;
    float maxFireLevel;

    QHash<QString, int> hashVar; //var name map position of var in dataSet

    void detectVarUniverses(universeBounds* varUniArray);
    void evaluateSample(int sampleNum);
    int getVarIndex(QString name);
    int getVarInDataset(const QString name);

    typedef struct  {
        int tPosCount, tNegCount, fPosCount, fNegCount;
        //float fitness;
        float sensitivity;
        float specificity;
        float accuracy;
        float ppv;
        float rmse;
        float rrse;
        float rae;
        float mse;
        float distanceThreshold;
        float distanceMinThreshold;
        float squareError; /* relative square error */
        float rmseError; /* Error for compute RMSE is Sum( Predict - Actual ) */
        float distMinBelow; /* distance minimal to threshold from below  */
        float distMinAbove; /* distance minimal to threshold from above  */
        float maxActualValue;
        float errorSum; /* absolute error used to compute RAE */
        float sumDistW;
        float sumDistBelow; /* used to compute MDM */
        float sumDistAbove; /* used to compute MDM */
    } fitnessStruct;

public slots:
    void saveToFile(QString fileName, float fitness);
    void loadFromFile(QString fileName);

signals:
    void fitnessThreshReached();
};

#endif // FUZZYSYSTEM_H
