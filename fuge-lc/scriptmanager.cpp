/**
  * @file   scriptmanager.cpp
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

#include <iostream>

#include <QFile>
#include <QMessageBox>
#include <QSemaphore>

#include "scriptmanager.h"
#include "systemparameters.h"

extern QSemaphore scriptSema;

ScriptManager::ScriptManager()
{
    engine = new QScriptEngine();
    thisObject = engine->newQObject(this);
}

/**
  * Main method of the thread. It is executed when the thread is started.
  */
void ScriptManager::run()
{
    QScriptValue runFunc = engine->evaluate("doRun");
    runFunc.call(thisObject);
    //runEvo();
    emit scriptFinished();
}

/**
  * Sets the name of the script file to be evaluated.
  *
  * @param fileName Name of the script file.
  */
void ScriptManager::setScriptFileName(QString fileName)
{
    this->fileName = fileName;
}

/**
  * Read all the parameters of the script. This function must be called in
  * the script file.
  */
void ScriptManager::setParams(QString experimentName,
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
                    // MODIF - BUJARD Alexandre - 15.04.2010
                    float rrseW,
                    float raeW,
                    //float coefCorrW,
                    float mseW,
                    //float msePower;
                    float distanceThresholdW,
                    float distanceMinThresholdW,
                    float dontCareW,
                    float overLearnW,
                    // FIN - MODIF - BUJARD Alexandre - 15.04.2010

                    float thresh,
                    bool threshActivated)
{
    SystemParameters& sysParams = SystemParameters::getInstance();

    // Ensure the save path directory exists. If not we create one.
    QDir savePathDir;
    if (!savePathDir.exists(savePath)) {
        savePathDir.mkdir(savePath);
    }

    // Retrieve parameters from script
    sysParams.setExperimentName(experimentName);
    sysParams.setSavePath(savePath+"/");
    sysParams.setFixedVars(fixedVars);
    sysParams.setNbRules(nbRules);
    sysParams.setNbVarPerRule(nbMaxVarPerRule);
    sysParams.setNbOutVars(nbOutVars);
    sysParams.setNbInSets(nbInSets);
    sysParams.setNbOutSets(nbOutSets);
    sysParams.setInVarsCodeSize(inVarsCodeSize);
    sysParams.setOutVarsCodeSize(outVarsCodeSize);
    sysParams.setInSetsCodeSize(inSetsCodeSize);
    sysParams.setOutSetsCodeSize(outSetsCodeSize);
    sysParams.setInSetsPosCodeSize(inSetsPosCodeSize);
    sysParams.setOutSetPosCodeSize(outSetPosCodeSize);
    sysParams.setMaxGenPop1(maxGenPop1);
    sysParams.setMaxFitPop1(maxFitPop1);
    sysParams.setEliteSizePop1(elitePop1);
    sysParams.setPopSizePop1(popSizePop1);
    sysParams.setCxProbPop1(cxProbPop1);
    sysParams.setMutFlipIndPop1(mutFlipIndPop1);
    sysParams.setMutFlipBitPop1(mutFlipBitPop1);
    sysParams.setMaxGenPop2(maxGenPop2);
    sysParams.setMaxFitPop2(maxFitPop2);
    sysParams.setEliteSizePop2(elitePop2);
    sysParams.setPopSizePop2(popSizePop2);
    sysParams.setCxProbPop2(cxProbPop2);
    sysParams.setMutFlipIndPop2(mutFlipIndPop2);
    sysParams.setMutFlipBitPop2(mutFlipBitPop2);
    sysParams.setSensiW(sensiW);
    sysParams.setSpeciW(speciW);
    sysParams.setAccuracyW(accuracyW);
    sysParams.setPpvW(ppvW);
    sysParams.setRmseW(rmseW);
    sysParams.setRrseW(rrseW);
    sysParams.setRaeW(raeW);
    sysParams.setMseW(mseW);
    sysParams.setDistanceThresholdW(distanceThresholdW);
    sysParams.setDistanceMinThresholdW(distanceMinThresholdW);
    sysParams.setDontCareW(dontCareW);
    sysParams.setOverLearnW(overLearnW);
    /*QVector<float> threshold;
    threshold.resize(1);
    threshold.replace(0, thresh);
    sysParams.setThreshold(threshold);*/
    for (int i = 0; i < nbOutVars; i++)
        sysParams.setThresholdVal(i, thresh);
    sysParams.setThreshActivated(threshActivated);
}

/**
  * Read the contents of the script file.
  */
void ScriptManager::readScript()
{
    QFile file(fileName);
    if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(0, "Error", "Could not open script file!");
        std::cout << "Script error : cannot find script file !" << std::endl;
        return;
    }

    QString strProgram = file.readAll();

    // Static check of the script
    if (! engine->canEvaluate(strProgram) )
    {
        QMessageBox::critical(0, "Error", "script : canEvaluate returned false!");
        std::cout << "Script error : cannot evaluate script !" << std::endl;
        return;
    }

    // Evaluate the script
    engine->evaluate(strProgram);

    // Check if we have an uncaught exception ?
    if (engine->hasUncaughtException())
    {
        QScriptValue exception = engine->uncaughtException();
        QMessageBox::critical(0, "Script error", QString("Script threw an uncaught exception : ") + exception.toString());
        std::cout << "Script error ! : script threw an uncaught exception : " << exception.toString().toStdString() << std::endl;
        return;
    }

    if (! engine->canEvaluate("doSetParams") )
    {
        QMessageBox::critical(0, "Error", "canEvaluate returned false!");
        std::cout << "Script error : cannot evaluate script !" << std::endl;
        return;
    }


    QScriptValue setParamsFunc = engine->evaluate("doSetParams", strProgram);
    if (engine->hasUncaughtException())
    {
        QScriptValue exception = engine->uncaughtException();
        QMessageBox::critical(0, "Script error", QString("Script threw an uncaught exception : ") + exception.toString());
        std::cout << "Script error ! : script threw an uncaught exception : " << exception.toString().toStdString() << std::endl;
        return;
    }

    QScriptSyntaxCheckResult result = engine->checkSyntax("doSetParams");
    std::cout << result.errorMessage().toStdString() << std::endl;

    setParamsFunc.call(thisObject);
}

/**
  * Evaluate and run the doRun() function of the script.
  */
void ScriptManager::runScript()
{
    QScriptValue runFunc = engine->evaluate("doRun");
    runFunc.call(thisObject);
}

/**
  * Start a single coevolution run.
  */
void ScriptManager::runEvo()
{
    
    QScriptValue setParamsFunc = engine->evaluate("doSetParams");
    setParamsFunc.call(thisObject);

    emit startRun();
    scriptSema.acquire();
}
