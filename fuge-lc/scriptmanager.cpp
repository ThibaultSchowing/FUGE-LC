/**
  * @file   scriptmanager.cpp
  * @author Jean-Philippe Meylan <jean-philippe.meylan_at_heig-vd.ch>
  * @date   07.2009
  * @author Rochus Keller <me@rochus-keller.ch> (switched from QtScript to duktape)
  * @date   05.2022
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
#include <QtDebug>
#include <QDir>
#include <duktape.h>

#include "scriptmanager.h"
#include "systemparameters.h"

extern QSemaphore scriptSema;

static ScriptManager* s_this = 0;

struct ScriptManager::Imp
{
    duk_context* engine;
    QString fileName;
    bool scriptReady;
    Imp():engine(0),scriptReady(false){}

    void doRun()
    {
        duk_get_global_string (engine , "doRun" );
        duk_get_global_string(engine , "$this$");
        const duk_int_t res = duk_pcall_method(engine, 0 );
        if( res != DUK_EXEC_SUCCESS )
            qCritical() << duk_to_string(engine, -1);
        duk_pop(engine);
    }

    void doSetParams()
    {
        duk_get_global_string (engine , "doSetParams" );
        duk_get_global_string(engine , "$this$");
        const duk_int_t res = duk_pcall_method(engine, 0 );
        if( res != DUK_EXEC_SUCCESS )
            qCritical() << duk_to_string(engine, -1);
        duk_pop(engine);
    }
};

static duk_ret_t _setParams(duk_context * ctx)
{
    if( s_this == 0 )
        return 0;
    s_this->setParams(
            QString::fromUtf8(duk_safe_to_string(ctx,0)), //QString experimentName
            QString::fromUtf8(duk_safe_to_string(ctx,1)), //QString savePath
            duk_to_boolean(ctx,2), //bool fixedVars
            duk_to_int(ctx,3), //int nbRules
            duk_to_int(ctx,4), //int nbMaxVarPerRule
            duk_to_int(ctx,5), //int nbOutVars
            duk_to_int(ctx,6), //int nbInSets
            duk_to_int(ctx,7), //int nbOutSets
            duk_to_int(ctx,8), //int inVarsCodeSize
            duk_to_int(ctx,9), //int outVarsCodeSize
            duk_to_int(ctx,10), //int inSetsCodeSize
            duk_to_int(ctx,11), //int outSetsCodeSize
            duk_to_int(ctx,12), //int inSetsPosCodeSize
            duk_to_int(ctx,13), //int outSetPosCodeSize
            duk_to_int(ctx,14), //int maxGenPop1
            duk_to_number(ctx,15), //float maxFitPop1
            duk_to_int(ctx,16), //int elitePop1
            duk_to_int(ctx,17), //int popSizePop1
            duk_to_number(ctx,18), //float cxProbPop1
            duk_to_number(ctx,19), //float mutFlipIndPop1
            duk_to_number(ctx,20), //float mutFlipBitPop1
            duk_to_int(ctx,21), //int maxGenPop2
            duk_to_number(ctx,22), //float maxFitPop2
            duk_to_int(ctx,23), //int elitePop2
            duk_to_int(ctx,24), //int popSizePop2
            duk_to_number(ctx,25), //float cxProbPop2
            duk_to_number(ctx,26), //float mutFlipIndPop2
            duk_to_number(ctx,27), //float mutFlipBitPop2
            duk_to_number(ctx,28), //float sensiW
            duk_to_number(ctx,29), //float speciW
            duk_to_number(ctx,30), //float accuracyW
            duk_to_number(ctx,31), //float ppvW
            duk_to_number(ctx,32), //float rmseW
            duk_to_number(ctx,33), //float rrseW
            duk_to_number(ctx,34), //float raeW
            duk_to_number(ctx,35), //float mseW
            duk_to_number(ctx,36), //float distanceThresholdW
            duk_to_number(ctx,37), //float distanceMinThresholdW
            duk_to_number(ctx,38), //float dontCareW
            duk_to_number(ctx,39), //float overLearnW
            duk_to_number(ctx,40), //float thresh
            duk_to_boolean(ctx,41) //bool threshActivated
                );
    return 0;
}

static duk_ret_t _runEvo(duk_context * ctx)
{
    if( s_this == 0 )
        return 0;
    s_this->runEvo();
    return 0;
}

static duk_ret_t _print(duk_context * ctx)
{
    qDebug() << duk_safe_to_string(ctx, -1);
    return 0;
}

ScriptManager::ScriptManager()
{
    s_this = this;
    d_imp = new Imp();
    d_imp->engine = duk_create_heap_default();

    duk_push_object(d_imp->engine);

    duk_push_c_function ( d_imp->engine , _setParams , 42 );
    duk_put_prop_string ( d_imp->engine , - 2 , "setParams" );

    duk_push_c_function ( d_imp->engine , _runEvo , 0 );
    duk_put_prop_string ( d_imp->engine , - 2 , "runEvo" );

    duk_put_global_string ( d_imp->engine , "$this$" );

    duk_push_c_function ( d_imp->engine , _print , 1 );
    duk_put_global_string ( d_imp->engine , "print" );

}

ScriptManager::~ScriptManager()
{
    s_this = 0;
    if( d_imp->engine )
        duk_destroy_heap( d_imp->engine );
    delete d_imp;
}

/**
  * Main method of the thread. It is executed when the thread is started.
  */
void ScriptManager::run()
{
    if( !d_imp->scriptReady )
        return;
    d_imp->doRun();
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
        qCritical() << "Script error : cannot find script file !";
        return;
    }

    const QByteArray strProgram = file.readAll();

    // Check and evaluate the script
    if( duk_peval_string( d_imp->engine, strProgram.constData() ) != DUK_EXEC_SUCCESS )
    {
        d_imp->scriptReady = false;
        QMessageBox::critical(0, "Script error", duk_safe_to_string ( d_imp->engine , - 1 ) );
        duk_pop( d_imp->engine );
        return;
    }
    duk_pop( d_imp->engine );

    d_imp->scriptReady = true;
    d_imp->doSetParams();
}

/**
  * Evaluate and run the doRun() function of the script.
  */
void ScriptManager::runScript()
{
    if( !d_imp->scriptReady )
    {
        QMessageBox::critical(0, "Error calling doRun", "The script has errors and cannot be run");
        return;
    }
    d_imp->doRun();
}

bool ScriptManager::isScriptReady() const
{
    return d_imp->scriptReady;
}

/**
  * Start a single coevolution run.
  */
void ScriptManager::runEvo()
{
    d_imp->doSetParams();

    emit startRun();
    scriptSema.acquire();
}
