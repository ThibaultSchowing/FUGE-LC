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

#include <iostream>

#include <QFile>
#include <QStandardPaths>
#include <QDir>

#include "systemparameters.h"

SystemParameters::SystemParameters()
{
    datasetName = "";
    fixedVars = false;
    verbose = false;
    //MODIF - Bujard - 18.03.2010
    //MODIF - Bujard - 01.04.2010
    // Add some indice, usefull for regression problems
    regression     = false;
    classification = false;
    //classification = false;
    // FIN - MODIF - Bujard - 18.03.2010
    savePath= "./";
    nbRules = -1;
    nbInVars = -1;
    nbOutVars = -1;
    nbInSets = -1;
    nbOutSets = -1;
    inVarsCodeSize = -1;
    outVarsCodeSize = -1;
    inSetsCodeSize = -1;
    outSetsCodeSize = -1;
    inSetsPosCodeSize = -1;
    outSetPosCodeSize = -1;
    sensiW = 0.0;
    speciW = 0.0;
    accuracyW = 0.0;
    ppvW = 0.0;
    rmseW = 0.0;
    //MODIF - Bujard - 18.03.2010
    // Add some indice, usefull for regression problems
    rrseW = 0.0;
    raeW = 0.0;
    mseW = 0.0;
    distanceThresholdW = 0.0;
    distanceMinThresholdW = 0.0;
    threshActivated = true;
    dontCareW = 0.0;
    overLearnW = 0.0;
    // FIN - MODIF - Bujard - 18.03.2010
    maxGenPop1 = -1;
    maxFitPop1 = 1.0;
    popSizePop1 = -1;
    cxProbPop1 = -1.0;
    mutFlipIndPop1 = -1.0;
    mutFlipBitPop1 = -1.0;
    maxGenPop2 = -1;
    maxFitPop2 = 1.0;
    popSizePop2 = -1;
    cxProbPop2 = -1.0;
    mutFlipIndPop2 = -1.0;
    mutFlipBitPop2 = -1.0;
    coevolutionary = true;
    learning = false;
    learningMethod = 0;
    initVarsMethod = 0;
    defaultFilePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/";
    globalFilesPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/";
    QDir fugeDir;
    QFile inifile(globalFilesPath + "/currentSession.ini");
    if (!fugeDir.exists(globalFilesPath)) {
        fugeDir.mkdir(globalFilesPath);
        writeIni();
    } else {
        readIni();
    }
    assert(fugeDir.exists(globalFilesPath)); // DEBUG

}

SystemParameters::~SystemParameters()
{
}

bool SystemParameters::newWorkFolder(const QString& path) {
    QDir workDir;
    if (path != "/" && workDir.exists(path)) {
        QString fuzzyPath = path + "fuzzySystems";
        if (!workDir.exists(fuzzyPath)) {
            workDir.mkdir(fuzzyPath);
        }
        QString configPath = path + "configs";
        if (!workDir.exists(configPath)) {
            workDir.mkdir(configPath);
        }
        QString scriptPath = path + "scripts";
        if (!workDir.exists(scriptPath)) {
            workDir.mkdir(scriptPath);
        }

        if (!workDir.exists(scriptPath) || !workDir.exists(configPath) || !workDir.exists(scriptPath)) {
            return false ;
        }

        savePath = path;
        datasetName = "";
        writeIni();

        return true;
    }

    return false;
}

void SystemParameters::writeIni() {
    QFile::remove(globalFilesPath + "currentSession.ini");
    QFile inifile(globalFilesPath + "currentSession.ini");
    if (!inifile.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "Could not create the file.";
    }else{
        QTextStream out(&inifile);
        if (!savePath.isEmpty())
            out << "savePath=" << savePath << "\n";
        if (!datasetName.isEmpty())
            out << "dataSetName=" << datasetName << "\n";
        inifile.close();
    }
}

void SystemParameters::readIni() {
    QFile inifile(globalFilesPath + "currentSession.ini");
    if (inifile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&inifile);

        QString line;
        QStringList content;
        while(!(line = in.readLine()).isEmpty()) {
            // Remove all spaces
            line = line.trimmed();
            line.replace(" ", "");
            content = line.split("=");

            if(content.size() == 2){
                if(content.at(0).compare("savePath", Qt::CaseInsensitive)==0) {
                    savePath = content.at(1);
                }
                else if(content.at(0).compare("dataSetName", Qt::CaseInsensitive)==0){
                    datasetName = content.at(1);
                }
            }
        }
        inifile.close();
    }
    else {
        qDebug() << "Could not open ini file";
    }
}
