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

#include "systemparameters.h"

SystemParameters::SystemParameters()
{
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
}

SystemParameters::~SystemParameters()
{
}
