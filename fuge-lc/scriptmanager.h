/**
  * @file   scriptmanager.h
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
  * @class ScriptManager
  *
  * @brief This class is resopnsible to manage the script execution of the application.
  * The parameters are read from the scriptfile and the doRun() function is evaluated.
  */

#ifndef SCRIPTMANAGER_H
#define SCRIPTMANAGER_H

#include <QThread>

class ScriptManager : public QThread
{
    Q_OBJECT

public:
    ScriptManager();
    ~ScriptManager();

    void setScriptFileName(QString fileName);
    void readScript();
    void runScript();
    bool isScriptReady() const;

protected:
    void run();

signals:
    void startRun();
    void scriptFinished();

public slots:
    void runEvo();
    void setParams(QString experimentName,
                QString savePath,
                bool fixedVars,
                int nbRules,
                int nbMaxVarPerRule,
                int nbOutVars,
                int nbInSets,
                int nbOutSets,
                int inVarsCodeSize,
                int outVarsCodeSize,
                int inSetsCodeSize,
                int outSetsCodeSize,
                int inSetsPosCodeSize,
                int outSetPosCodeSize,
                int maxGenPop1,
                float maxFitPop1,
                int elitePop1,
                int popSizePop1,
                float cxProbPop1,
                float mutFlipIndPop1,
                float mutFlipBitPop1,
                int maxGenPop2,
                float maxFitPop2,
                int elitePop2,
                int popSizePop2,
                float cxProbPop2,
                float mutFlipIndPop2,
                float mutFlipBitPop2,
                float sensiW,
                float speciW,
                float accuracyW,
                float ppvW,
                float rmseW,
                float rrseW,
                float raeW,
                float mseW,
                float distanceThresholdW,
                float distanceMinThresholdW,
                float dontCareW,
                float overLearnW,
                float thresh,
                bool threshActivated);

private:
    struct Imp;
    Imp* d_imp;
};

#endif // SCRIPTMANAGER_H
