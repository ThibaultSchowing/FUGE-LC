/**
  * @file   computethread.h
  * @author Jean-Philippe Meylan <jean-philippe.meylan_at_heig-vd.ch>
  * @author ReDS (Reconfigurable and embedded digital systems) <www.reds.ch>
  * @author HEIG-VD (Haute école d'ingénierie et de gestion) <www.heig-vd.ch>
  * @author Yvan Da Silva <yvan.dasilva_at_heig-vd.ch>
  * @date   06.2012
  * @date   03.2010
  * @section LICENSE
  *
  * This application is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 3.0 of the License, or (at your option) any later version.
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

#ifndef COMPUTETHREAD_H
#define COMPUTETHREAD_H

#include <QThread>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStringList>
#include <QString>
#include <QMutex>

#include "coevolution.h"
#include "fuzzysystem.h"
#include "systemparameters.h"
#include "evolutionengine.h"
#include "timertool.h"

class CoEvolution;

class ComputeThread : public QThread
{
    Q_OBJECT

public:
    ComputeThread();
    virtual ~ComputeThread();
    void setFuzzySystem(FuzzySystem *fSystemLeft, FuzzySystem *fSystemRight);

    // Performance variable. Available as static for Coevolution.
    static int nbRules;
    static int membersGenSize;
    static int ruleGenSize;
    static int nbVarPerRule;
    static int inSetsCodeSize;
    static int nbOutVars;
    static int outSetsCodeSize;
    static FuzzySystem* bestFSystem;
    static QString bestFuzzySystemDescription;
    static qreal bestFitness;
    static void saveFuzzyAndFitness(FuzzySystem *fSystem, qreal fitness);
    static void saveSystemStats(QString name, qreal minFitness, qreal maxFitness, qreal meanFitness, qreal standardDeviation, int populationSize, int generation);
    static SystemParameters *sysParams;
    static bool stop;
protected:
    void run();
    static QMutex mutex;
    static QMutex mutex2;

private:
    FuzzySystem* fSystemLeft;
    FuzzySystem* fSystemRight;

    QList<QStringList>* listFile;
    int counter;
    bool fuzzySystemLoaded;

    CoEvolution *leftEvolution;
    CoEvolution *rightEvolution;

    struct POPULATION_CONFIG_TYPE
    {
        quint32 maxGeneration, eliteKeepSize, populationSize, genotypeSize;
        qreal crossoverProbability, mutationIndividualProbability, mutationPerBitProbability;
        quint8 loggerFileLevel, loggerConsoleLevel;
    }rules, vars;

    struct timeval startTime, endTime;
    qreal elapsedTime;

    void loadConfiguration(POPULATION_CONFIG_TYPE &config, QString fileNameWithDirPath);

signals:
    void computeFinished();
    void doSaveSystem(QString fileName);

public slots:
    void onSaveSystem(QString fileName);
    void onStopEvo();
};

#endif // COMPUTETHREAD_H
