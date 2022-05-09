/**
  * @file   coevevalop.hpp
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
  * @class CoevEvalOp
  * @brief This class implements the evaluation operator for the coevolution. Two methods are
  * mandatory for Beagle : makeSets and evaluateSets. Basically, this class receives the
  * two populations and is responsible to evaluate the fitness of all individuals.
  */

#ifndef CoevEvalOp_hpp
#define CoevEvalOp_hpp

#include <vector>
#include <stdint.h>
#include <QList>
#include <QObject>
#include <cmath>
#include <algorithm>
#include <QTime>

#include "../fuzzy/fuzzysystem.h"
#include "../EvolutionEngine/evolutionengine.h"
#include "../Population/population.h"
#include "../Population/Individual/popentity.h"
#include "../fuzzy/fuzzymembershipsgenome.h"
#include "../fuzzy/fuzzyrulegenome.h"
#include "../systemparameters.h"
#include "../fugemain.h"
#include "../computethread.h"

#include "coevcooperator.h"
#include "coevstats.h"
#include "qbitarrayutility.h"

// Number of cooperators to be used
#define COOP_SIZE 2

class CoEvolution : public QThread, public EvolutionEngine {

    Q_OBJECT
public:
    CoEvolution(FuzzySystem *fSystem, QMutex *leftLock, QMutex *rightLock, Population* left, Population* right, quint32 generationCount, qreal crossoverProbability, qreal mutationProbability, qreal mutationPerBitProbability, quint32 eliteSize, quint32 cooperatorsCount, QObject *parent = 0);

    ~CoEvolution();

    void run();
    bool evaluatePopulation(Population* population, quint32 generation);
    void onSaveSystem(QString fileName);

signals :
    void fitnessThreshReached();
    void sendData();

protected:
    static SystemParameters *sysParams;
    void calcFitness(PopEntity *inInd1, PopEntity *inInd2);
    float fixedToFloat(quint32 fixedInt, int pointPos) const;

private:
    FuzzySystem *fSystem;
    QMutex *leftLock;
    QMutex *rightLock;
    Population *left;
    Population *right;
    quint32 generationCount;
    quint32 nextBasePopulationCount;
    quint32 eliteSize;
    quint32 cooperatorsCount;
    qreal fitness;

    QString fileName;
    qreal finalFit;
    bool isFirst;
    bool needToSave;
};

#endif // CoevEvalOp_hpp
