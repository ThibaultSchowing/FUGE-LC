/**
  * @file   computethread.cpp
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
  * @class ComputeThread
  *
  * @brief This class implements a thread which controlls the execution of the population
  * evolvers.
  */

#include "computethread.h"
int ComputeThread::nbRules = 0;
int ComputeThread::membersGenSize = 0;
int ComputeThread::ruleGenSize = 0;
int ComputeThread::nbVarPerRule = 0;
int ComputeThread::inSetsCodeSize = 0;
int ComputeThread::nbOutVars = 0;
int ComputeThread::outSetsCodeSize = 0;
FuzzySystem* ComputeThread::bestFSystem = 0;
qreal ComputeThread::bestFitness = 0.0;
QString ComputeThread::bestFuzzySystemDescription = "";
SystemParameters *ComputeThread::sysParams = NULL;
QMutex ComputeThread::mutex;
QMutex ComputeThread::mutex2;
bool ComputeThread::stop = false;

ComputeThread::ComputeThread()
{
    fuzzySystemLoaded = false;

}

ComputeThread::~ComputeThread()
{
}

/**
  * Load a fuzzy system which will be passed to the populations evolvers.
  *
  *@param fSystem Pointer to an existing fuzzy system
  */
void ComputeThread::setFuzzySystem(FuzzySystem *fSystemLeft, FuzzySystem *fSystemRight)
{
    this->fSystemLeft = fSystemLeft;
    this->fSystemRight = fSystemRight;
    fuzzySystemLoaded = true;
}

/**
  * Starts the thread.
  */
void ComputeThread::run()
{
    // Begin timer
    gettimeofday(&startTime, NULL);

    ComputeThread::stop = false;
    ComputeThread::bestFitness = 0.0;
    ComputeThread::bestFSystem = 0;
    ComputeThread::bestFuzzySystemDescription.clear();
    // Define some usefull vars to speed up the build fuzzy process.
    ComputeThread::nbRules = fSystemLeft->getNbRules();
    ComputeThread::membersGenSize = fSystemLeft->getMembershipsBitStringSize();
    ComputeThread::ruleGenSize = fSystemLeft->getRuleMaxBitStringSize();
    ComputeThread::nbVarPerRule = fSystemLeft->getNbVarPerRule();
    ComputeThread::inSetsCodeSize = fSystemLeft->getInSetsCodeSize();
    ComputeThread::nbOutVars = fSystemLeft->getNbOutVars();
    ComputeThread::outSetsCodeSize = fSystemLeft->getOutSetsCodeSize();
    ComputeThread::sysParams = &SystemParameters::getInstance();


    qDebug() << "RUN : ComputeThread;";
    try {
        QString coevMembershipConfig = QCoreApplication::applicationDirPath()+QString("/coev-memberships.conf");
        qDebug() << coevMembershipConfig;
        QString coevRulesConfig = QCoreApplication::applicationDirPath()+QString("/coev-rules.conf");
        qDebug() << coevRulesConfig;
        loadConfiguration(vars, coevMembershipConfig);
        loadConfiguration(rules, coevRulesConfig);
        ComputeThread::sysParams = &SystemParameters::getInstance();

        vars.genotypeSize = fSystemLeft->getMembershipsBitStringSize();
        rules.genotypeSize = ((fSystemLeft->getRuleBitStringSize())*fSystemLeft->getNbRules())+fSystemLeft->getDefaultRulesBitStringSize();

        Population *popVar = new Population("MEMBERSHIPS", sysParams->getPopSizePop1(), vars.genotypeSize);
        Population *popRules = new Population("RULES", sysParams->getPopSizePop2(), rules.genotypeSize);

        QMutex *leftLock = new QMutex();
        QMutex *rightLock = new QMutex();

        leftEvolution = new CoEvolution(fSystemLeft, leftLock, rightLock, popVar, popRules, sysParams->getMaxGenPop1(),  sysParams->getCxProbPop1(),sysParams->getMutFlipIndPop1(), sysParams->getMutFlipBitPop1(),sysParams->getEliteSizePop1(), sysParams->getNbCooperators());
        rightEvolution = new CoEvolution(fSystemRight, rightLock, leftLock, popRules, popVar, sysParams->getMaxGenPop1(), sysParams->getCxProbPop2(), sysParams->getMutFlipIndPop2(), sysParams->getMutFlipBitPop2(),sysParams->getEliteSizePop1(), sysParams->getNbCooperators());

        connect(leftEvolution,SIGNAL(fitnessThreshReached()), this, SLOT(onStopEvo()));
        connect(rightEvolution,SIGNAL(fitnessThreshReached()), this, SLOT(onStopEvo()));

        qDebug() << "END_GetInstance";

        // 3. start evolution
        qDebug() << "Start Evolution";
        leftEvolution->start();
        rightEvolution->start();

        qDebug() << "Start waiting Evolution";
        rightEvolution->wait();
        leftEvolution->wait();
        qDebug() << "End waiting Evolution";

//        if(bestFSystem != fSystemLeft && fSystemLeft != 0)
//            delete fSystemLeft;
//        else if(bestFSystem != fSystemRight && fSystemRight != 0)
//            delete fSystemRight;

        delete leftLock;
        delete rightLock;
        delete popVar;
        delete popRules;
        delete leftEvolution;
        delete rightEvolution;

    }

    catch(std::exception& inException) {
        std::cerr << "Standard exception catched:" << std::endl << std::flush;
        std::cerr << inException.what() << std::endl << std::flush;
    }

    // End Timer
    gettimeofday(&endTime, NULL);
    elapsedTime = TimerTool::getElapsedTime(startTime,endTime);
    qDebug() << "ElapsedTime in seconds : " << elapsedTime / 1000 ;

    emit computeFinished();
}

/**
  * Slot called when a request to save the system is performed
  */
void ComputeThread::onSaveSystem(QString fileName)
{
    ComputeThread::bestFSystem->saveToFile(fileName, ComputeThread::bestFitness);
}

/**
  * Load configuration
  */
void ComputeThread::loadConfiguration(POPULATION_CONFIG_TYPE &config, QString fileNameWithDirPath){
    // 1. Set encoding, that is a bitstring of bits
    //quint32 genotypeSize=fSystem->getMembershipsBitStringSize();
    // Try to read config file

    QFile file(fileNameWithDirPath);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Could open config";
        QTextStream in(&file);

        QString line;
        QStringList content;
        while((line = in.readLine()) != NULL)
        {
            line = line.trimmed();
            line.replace(" ", "");
            content = line.split("=");

            if(content.size() == 2){
                if(content.at(0).compare("max_generation", Qt::CaseInsensitive)==0)
                    config.maxGeneration = content.at(1).toUInt();
                else if(content.at(0).compare("elite_keepsize", Qt::CaseInsensitive)==0)
                    config.eliteKeepSize = content.at(1).toUInt();
                else if(content.at(0).compare("pop_size", Qt::CaseInsensitive)==0)
                    config.populationSize = content.at(1).toUInt();
                else if(content.at(0).compare("crossover_probability", Qt::CaseInsensitive)==0)
                    config.crossoverProbability = content.at(1).toFloat();
                else if(content.at(0).compare("mutationIndividual_probability", Qt::CaseInsensitive)==0)
                    config.mutationIndividualProbability = content.at(1).toFloat();
                else if(content.at(0).compare("mutationPerBit_probability", Qt::CaseInsensitive)==0)
                    config.mutationPerBitProbability = content.at(1).toFloat();
                else if(content.at(0).compare("loggerFile_level", Qt::CaseInsensitive)==0)
                    config.loggerFileLevel = content.at(1).toUInt();
                else if(content.at(0).compare("loggerConsole_level", Qt::CaseInsensitive)==0)
                    config.loggerConsoleLevel = content.at(1).toUInt();
            }
        }
        file.close();
    }
    else{
        qDebug() << "Could not open config file";
    }
}

/**
  * Request to stop the evolution.
  */
void ComputeThread::onStopEvo()
{
//    leftCoevEvalOperator->terminate();
//    rightCoevEvalOperator->terminate();
    ComputeThread::stop = true;
}

void ComputeThread::saveFuzzyAndFitness(FuzzySystem *fSystem, qreal fitness){
    QMutexLocker locker(&mutex2);
    if(fitness >= ComputeThread::bestFitness){
        ComputeThread::bestFitness = fitness;
        ComputeThread::bestFSystem = fSystem;
        ComputeThread::bestFuzzySystemDescription.clear();
        ComputeThread::bestFuzzySystemDescription.append(ComputeThread::bestFSystem->getSystemDescritpion());

        CoevStats& fitStats = CoevStats::getInstance();
        fitStats.setSensi(ComputeThread::bestFSystem->getSensitivity());
        fitStats.setSpeci(ComputeThread::bestFSystem->getSpecificity());
        fitStats.setAccu(ComputeThread::bestFSystem->getAccuracy());
        fitStats.setPpv(ComputeThread::bestFSystem->getPpv());
        fitStats.setRmse(ComputeThread::bestFSystem->getRmse());
        fitStats.setRrse(ComputeThread::bestFSystem->getRrse());
        fitStats.setRae(ComputeThread::bestFSystem->getRae());
        fitStats.setMse(ComputeThread::bestFSystem->getMse());
        fitStats.setDistanceThreshold(ComputeThread::bestFSystem->getDistanceThreshold());
        fitStats.setDistanceMinThreshold(ComputeThread::bestFSystem->getDistanceMinThreshold());
        fitStats.setDontCare(ComputeThread::bestFSystem->getDontCare());
        fitStats.setOverLearn(ComputeThread::bestFSystem->getOverLearn());
        fitStats.setFitMaxPop1(ComputeThread::bestFitness);
        fitStats.setBestSysDesc(ComputeThread::bestFuzzySystemDescription);
// Haven't found code that use this.
//        if (fitness > coevStats.getFitMaxPop1()) {
//            QString fileName;
//            // Ensure that a temp directory exists. If not we create one.
//            QDir tempDir;
//            if (!tempDir.exists(sysParams->getSavePath()+"temp")) {
//                tempDir.mkdir(sysParams->getSavePath()+"temp");
//            }
//            fileName = QString(sysParams->getSavePath()+"temp/currentBest_") + QString::number(getpid()) + QString(".ffs");
//            ComputeThread::bestFSystem->saveToFile(fileName, fitness);

//        }
    }
}

void ComputeThread::saveSystemStats(QString name, qreal minFitness, qreal maxFitness, qreal meanFitness, qreal standardDeviation, int populationSize, int generation){
    QMutexLocker locker(&mutex);
    //TODO CHANGE THE VALUES AND ADD CORRECT PARAMS
    CoevStats& fitStats = CoevStats::getInstance();

    if(name == "RULES"){
        fitStats.setFitMaxPop2(maxFitness);
        fitStats.setFitAvgPop2(meanFitness);
        fitStats.setFitMinPop2(minFitness);
        fitStats.setFitStdPop2(standardDeviation);
        fitStats.setSizePop2(populationSize);

    }else{
        fitStats.setFitMaxPop1(maxFitness);
        fitStats.setFitAvgPop1(meanFitness);
        fitStats.setFitMinPop1(minFitness);
        fitStats.setFitStdPop1(standardDeviation);
        fitStats.setSizePop1(populationSize);
    }
    fitStats.setGenNumber(generation);
    // Send Data to graph.
    fitStats.transmitData(name);


}
