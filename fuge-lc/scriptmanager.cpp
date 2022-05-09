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
#include <QtScript>

#include "scriptmanager.h"
#include "systemparameters.h"

extern QSemaphore scriptSema;

struct ScriptManager::Imp
{
    QScriptEngine *engine;
    QScriptValue thisObject;
    QString fileName;
};

ScriptManager::ScriptManager()
{
    d_imp = new Imp();
    d_imp->engine = new QScriptEngine();
    d_imp->thisObject = d_imp->engine->newQObject(this);
}

ScriptManager::~ScriptManager()
{
    delete d_imp;
}

/**
  * Main method of the thread. It is executed when the thread is started.
  */
void ScriptManager::run()
{
    QScriptValue runFunc = d_imp->engine->evaluate("doRun");
    runFunc.call(d_imp->thisObject);
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
    d_imp->fileName = fileName;
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
    QFile file(d_imp->fileName);
    if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(0, "Error", "Could not open script file!");
        std::cout << "Script error : cannot find script file !" << std::endl;
        return;
    }

    QString strProgram = file.readAll();

    // Static check of the script
    if (! d_imp->engine->canEvaluate(strProgram) )
    {
        QMessageBox::critical(0, "Error", "script : canEvaluate returned false!");
        std::cout << "Script error : cannot evaluate script !" << std::endl;
        return;
    }

    // Evaluate the script
    d_imp->engine->evaluate(strProgram);

    // Check if we have an uncaught exception ?
    if (d_imp->engine->hasUncaughtException())
    {
        QScriptValue exception = d_imp->engine->uncaughtException();
        QMessageBox::critical(0, "Script error", QString("Script threw an uncaught exception : ") + exception.toString());
        std::cout << "Script error ! : script threw an uncaught exception : " << exception.toString().toStdString() << std::endl;
        return;
    }

    if (! d_imp->engine->canEvaluate("doSetParams") )
    {
        QMessageBox::critical(0, "Error", "canEvaluate returned false!");
        std::cout << "Script error : cannot evaluate script !" << std::endl;
        return;
    }


    QScriptValue setParamsFunc = d_imp->engine->evaluate("doSetParams", strProgram);
    if (d_imp->engine->hasUncaughtException())
    {
        QScriptValue exception = d_imp->engine->uncaughtException();
        QMessageBox::critical(0, "Script error", QString("Script threw an uncaught exception : ") + exception.toString());
        std::cout << "Script error ! : script threw an uncaught exception : " << exception.toString().toStdString() << std::endl;
        return;
    }

    QScriptSyntaxCheckResult result = d_imp->engine->checkSyntax("doSetParams");
    std::cout << result.errorMessage().toStdString() << std::endl;

    setParamsFunc.call(d_imp->thisObject);
}

/**
  * Evaluate and run the doRun() function of the script.
  */
void ScriptManager::runScript()
{
    QScriptValue runFunc = d_imp->engine->evaluate("doRun");
    runFunc.call(d_imp->thisObject);
}

/**
  * Start a single coevolution run.
  */
void ScriptManager::runEvo()
{
    
    QScriptValue setParamsFunc = d_imp->engine->evaluate("doSetParams");
    setParamsFunc.call(d_imp->thisObject);

    emit startRun();
    scriptSema.acquire();
}
