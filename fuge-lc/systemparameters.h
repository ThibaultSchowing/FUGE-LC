/**
  * @file   systemparameters.h
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
  * @class SystemParameters
  * @brief Singleton class which holds all the application parameters
  */


#ifndef SYSTEMPARAMETERS_H
#define SYSTEMPARAMETERS_H

#include <iostream>

#include <QObject>
#include <QVector>

class SystemParameters : public QObject
{
    Q_OBJECT

private:
    SystemParameters();
    ~SystemParameters();
    SystemParameters(const SystemParameters&);
    SystemParameters& operator=(const SystemParameters&);

    // Experiment name
    QString experimentName;
    // Database name
    QString datasetName;
    // Save path
    QString savePath;

    // Verbose mode flag
    bool verbose;

    //MODIF - Bujard - 18.03.2010
    // if regression parameters are activate
    bool regression;
    bool classification;
    // FIN - MODIF - Bujard - 18.03.2010

    // Fuzzy system parameters
    bool fixedVars;
    int nbRules;
    int nbVarPerRule;
    int nbInVars;
    int nbOutVars;
    int nbInSets;
    int nbOutSets;
    int inVarsCodeSize;
    int outVarsCodeSize;
    int inSetsCodeSize;
    int outSetsCodeSize;
    int inSetsPosCodeSize;
    int outSetPosCodeSize;
    int nbCooperators;
    float sensiW;
    float speciW;
    float accuracyW;
    float ppvW;
    float rmseW;

    //MODIF - Bujard - 18.03.2010
    // Add some indice, usefull for regression problems
    float rrseW;
    float raeW;
    // FIN - MODIF - Bujard - 18.03.2010

    /* MODIF - Bujard - 2.12.2010 */
    float mseW;
    float distanceThresholdW;
    float distanceMinThresholdW;
    bool threshActivated;
    float dontCareW;
    float overLearnW;
    /* FIN - MODIF - Bujard - 2.12.2010 */

    QVector<float> threshold;

    // Coevolution parameters
    // Population 1 : Membership functions
    int maxGenPop1;
    float maxFitPop1;
    int eliteSizePop1;
    int popSizePop1;
    float cxProbPop1;
    float mutFlipIndPop1;
    float mutFlipBitPop1;

    // Population 2 : Rules
    int maxGenPop2;
    float maxFitPop2;
    int eliteSizePop2;
    int popSizePop2;
    float cxProbPop2;
    float mutFlipIndPop2;
    float mutFlipBitPop2;

signals:
    void startRun();
    void scriptEnd();

public:
    static SystemParameters& getInstance()
    {
        static SystemParameters instance;
        return instance;
    }

    inline void setExperimentName(QString name) {experimentName = name;}
    inline void setDatasetName(QString name) {datasetName = name;}
    inline void setSavePath(QString path) {savePath = path;}
    inline void setVerbose(bool value) {verbose = value;}
    inline void setFixedVars(bool value) {fixedVars = value;}
    //MODIF - Bujard - 18.03.2010
    // if regression parameters are activate
    inline void setRegression     (bool value) {regression     = value;}
    inline void setClassification (bool value) {classification = value;}
    // FIN - MODIF - Bujard - 18.03.2010
    inline void setNbRules(int value) {nbRules = value;}
    inline void setNbVarPerRule(int value) {nbVarPerRule = value;}
    inline void setNbInVars(int value) {nbInVars = value;}
    inline void setNbOutVars(int value) {nbOutVars = value;
                                        threshold.resize(nbOutVars);}
    inline void setNbInSets(int value) {nbInSets = value;}
    inline void setNbOutSets(int value) {nbOutSets = value;}
    inline void setInVarsCodeSize(int value) {inVarsCodeSize = value;}
    inline void setOutVarsCodeSize(int value) {outVarsCodeSize = value;}
    inline void setInSetsCodeSize(int value) {inSetsCodeSize = value;}
    inline void setOutSetsCodeSize(int value) {outSetsCodeSize = value;}
    inline void setInSetsPosCodeSize(int value) {inSetsPosCodeSize = value;}
    inline void setOutSetPosCodeSize(int value) {outSetPosCodeSize = value;}
    inline void setSensiW(float value) {sensiW = value;}
    inline void setSpeciW(float value) {speciW = value;}
    inline void setAccuracyW(float value) {accuracyW = value;}
    inline void setPpvW(float value) {ppvW = value;}
    inline void setRmseW(float value) {rmseW = value;}
    inline void setRrseW     (float value) {rrseW     = value;}
    inline void setRaeW      (float value) {raeW      = value;}
    inline void setMseW      ( float value ) { mseW      = value; }
    inline void setDistanceThresholdW ( float value ) { distanceThresholdW = value; }
    inline void setDistanceMinThresholdW ( float value ) { distanceMinThresholdW = value; }
    inline void setThreshActivated (bool active) {threshActivated = active;}
    inline void setDontCareW ( float value ) { dontCareW = value; }
    inline void setOverLearnW ( float value ) { overLearnW = value; }
    inline void setThreshold(QVector<float> thresh) {threshold = thresh;}
    inline void setThresholdVal (int pos, float val) {threshold.replace(pos, val);}
    inline void resizeThreshold(int size) {threshold.resize(size);}

    inline void setMaxGenPop1(int value) {maxGenPop1 = value;}
    inline void setMaxFitPop1(float value) {maxFitPop1 = value;}
    inline void setEliteSizePop1(int value) {eliteSizePop1 = value;}
    inline void setPopSizePop1(int value) {popSizePop1 = value;}
    inline void setCxProbPop1(float value) {cxProbPop1 = value;}
    inline void setMutFlipIndPop1(float value) {mutFlipIndPop1 = value;}
    inline void setMutFlipBitPop1(float value) {mutFlipBitPop1 = value;}
    inline void setMaxGenPop2(int value) {maxGenPop2 = value;}
    inline void setMaxFitPop2(float value) {maxFitPop2 = value;}
    inline void setEliteSizePop2(int value) {eliteSizePop2 = value;}
    inline void setPopSizePop2(int value) {popSizePop2 = value;}
    inline void setCxProbPop2(float value) {cxProbPop2 = value;}
    inline void setMutFlipIndPop2(float value) {mutFlipIndPop2 = value;}
    inline void setMutFlipBitPop2(float value) {mutFlipBitPop2 = value;}
    inline void setNbCooperators(int value) {nbCooperators = value;}

    inline QString getExperimentName() {return experimentName;}
    inline QString getDatasetName() {return datasetName;}
    inline QString getSavePath() {return savePath;}
    inline bool getVerbose() {return verbose;}
    inline bool getFixedVars() {return fixedVars;}
    //MODIF - Bujard - 18.03.2010
    // if regression parameters are activate
    inline bool getRegression     () {return regression;}
    inline bool getClassification () {return classification;}
    // FIN - MODIF - Bujard - 18.03.2010
    inline int getNbRules() {return nbRules;}
    inline int getNbVarPerRule() {return nbVarPerRule;}
    inline int getNbInVars() {return nbInVars;}
    inline int getNbOutVars() {return nbOutVars;}
    inline int getNbInSets() {return nbInSets;}
    inline int getNbOutSets() {return nbOutSets;}
    inline int getInVarsCodeSize() {return inVarsCodeSize;}
    inline int getOutVarsCodeSize() {return outVarsCodeSize;}
    inline int getInSetsCodeSize() {return inSetsCodeSize;}
    inline int getOutSetsCodeSize() {return outSetsCodeSize;}
    inline int getInSetsPosCodeSize() {return inSetsPosCodeSize;}
    inline int getOutSetPosCodeSize() {return outSetPosCodeSize;}
    inline float getSensiW() {return sensiW;}
    inline float getSpeciW() {return speciW;}
    inline float getAccuracyW() {return accuracyW;}
    inline float getPpvW() {return ppvW;}
    inline float getRmseW() {return rmseW;}
    //MODIF - Bujard - 18.03.2010
    // Add some indice, usefull for regression problems
    inline float getRrseW     () {return rrseW;    }
    inline float getRaeW      () {return raeW;     }
    // FIN - MODIF - Bujard - 18.03.2010
        /* MODIF - Bujard - 2.12.2010 */
    inline float getMseW               () { return mseW; }
    inline float getDistanceThresholdW () { return distanceThresholdW; }
    inline float getDistanceMinThresholdW () { return distanceMinThresholdW; }
    inline bool getThreshActivated () {return threshActivated;}
    inline float getDontCareW () { return dontCareW; }
    inline float getOverLearnW () { return overLearnW; }
    /* FIN - MODIF - Bujard - 2.12.2010 */
    inline float getThresholdVal(int pos) {return threshold.at(pos);}
    inline int getMaxGenPop1() {return maxGenPop1;}
    inline float getMaxFitPop1() {return maxFitPop1;}
    inline int getEliteSizePop1() {return eliteSizePop1;}
    inline int getPopSizePop1() {return popSizePop1;}
    inline float getCxProbPop1() {return cxProbPop1;}
    inline float getMutFlipIndPop1() {return mutFlipIndPop1;}
    inline float getMutFlipBitPop1() {return mutFlipBitPop1;}
    inline int getMaxGenPop2() {return maxGenPop2;}
    inline float getMaxFitPop2() {return maxFitPop2;}
    inline int getEliteSizePop2() {return eliteSizePop2;}
    inline int getPopSizePop2() {return popSizePop2;}
    inline float getCxProbPop2() {return cxProbPop2;}
    inline float getMutFlipIndPop2() {return mutFlipIndPop2;}
    inline float getMutFlipBitPop2() {return mutFlipBitPop2;}
    inline int getNbCooperators() {return nbCooperators;}
};

#endif // SYSTEMPARAMETERS_H
