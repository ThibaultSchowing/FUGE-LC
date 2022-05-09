#ifndef POPULATION_H
#define POPULATION_H

#include <vector>
#include <QMutexLocker>
#include <QDebug>

#include "popentity.h"
#include "entityselection.h"
#include "randomgenerator.h"

using namespace std;
class Population
{
public:
    Population(QString name, quint32 size, quint32 individualsLength);
    Population(Population *population, QString name = QString());
    ~Population();

    quint32 getSize();
    void replace(vector<PopEntity *> newPopulation);
    void replace(vector<PopEntity *> base, vector<PopEntity *> generatedPartPopulation);
    void randomizePopulation();

    void setRepresentativesCopy(vector<PopEntity *> representatives, int quantity);
    vector<PopEntity *> getRepresentativesCopy();
    QString getName();

    PopEntity *getEntityCopyFrom(quint32 pos);
    PopEntity *getEntityAt(quint32 pos);

    vector<PopEntity *> getSomeEntityCopy(EntitySelection *entitySelection, quint32 count);
    vector<PopEntity *> getSomeEntity(EntitySelection *entitySelection, quint32 count);
    vector<PopEntity *> getAllEntitiesCopy();
    vector<PopEntity *> getAllEntities();
private:
    vector<PopEntity *> representatives;
    vector<PopEntity *> entityList;
    QMutex mutex;
    QString name;

};

#endif // POPULATION_H
