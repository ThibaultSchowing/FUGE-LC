#include "population.h"
Population::~Population(){
    for(int i = 0; i < entityList.size(); i++)
        delete entityList.at(i);
    entityList.clear();
}

Population::Population(QString name, quint32 size, quint32 individualsLength) : name(name)
{
    for(int i = 0; i < entityList.size(); i++)
        delete entityList.at(i);
    entityList.clear();
    for(quint32 i = 0; i < size; i++)
    {
        entityList.push_back(new PopEntity(individualsLength));
    }
    randomizePopulation();
}

Population::Population(Population *population, QString name)
{
    for(int i = 0; i < entityList.size(); i++)
        delete entityList.at(i);
    entityList.clear();
    for(quint32 i = 0; i < population->getSize(); i++)
    {
        entityList.push_back(population->getEntityCopyFrom(i));
    }
    if(name.isEmpty())
        this->name = population->getName();
}

void Population::setRepresentativesCopy(vector<PopEntity *> representatives, int quantity){
    int nbCooperator = quantity;

    QMutexLocker locker(&mutex);
    for(int i = 0; i < this->representatives.size(); i++)
        delete this->representatives[i];
    this->representatives.clear();

    // Random Cooperator (Working but not needed cooperators are the best ones only)
    //        if(nbCooperator > 1){
    //            nbCooperator--;
    //            int pos = RandomGenerator::getGeneratorInstance()->random(0,entityList.size()-1);
    //            this->representatives.push_back(new PopEntity(entityList.at(pos)));
    //        }


    vector<PopEntity *>::iterator it;
    for(it=representatives.begin(); it!= representatives.begin()+nbCooperator; it++){
        this->representatives.push_back(new PopEntity(*it));
    }

}

vector<PopEntity *> Population::getRepresentativesCopy(){
    QMutexLocker locker(&mutex);
    vector<PopEntity *> temp;
    vector<PopEntity *>::iterator it;
    for(it=representatives.begin(); it!=representatives.end(); it++)
        temp.push_back(new PopEntity(*it));
    return temp;
}

QString Population::getName(){
    return name;
}

void Population::replace(vector<PopEntity *> newPopulation)
{
    QMutexLocker locker(&mutex);
    for(int i = 0; i < entityList.size(); i++)
        delete entityList.at(i);
    entityList.clear();

    vector<PopEntity *>::iterator it;
    for(it=newPopulation.begin(); it!=newPopulation.end(); it++)
    {
        entityList.push_back(*it);
    }
}

void Population::replace(vector<PopEntity *> base, vector<PopEntity *> generatedPartPopulation)
{
    replace(generatedPartPopulation);

    vector<PopEntity *>::iterator it;
    for(it=base.begin(); it!=base.end(); it++)
    {
        entityList.push_back(*it);
    }
}

quint32 Population::getSize()
{
    return entityList.size();
}

void Population::randomizePopulation()
{
    QBitArray *data;
    bool status = false;
    for(quint32 i = 0; i < getSize(); i++)
    {
        entityList.at(i)->setFitness(0.f);
        data = entityList.at(i)->getGenotype()->getData();
        for(int j=0; j < data->size(); j++)
        {
            status = RandomGenerator::getGeneratorInstance()->random(0,1);
            data->setBit(j,status);
        }
    }
}

// Individual functions
PopEntity *Population::getEntityCopyFrom(quint32 pos)
{
    return getEntityAt(pos)->getCopy();
}

PopEntity *Population::getEntityAt(quint32 pos)
{
    return entityList.at(pos);
}

vector<PopEntity *> Population::getSomeEntityCopy(EntitySelection *entitySelection, quint32 count)
{
    vector<PopEntity *>::iterator it;
    vector<PopEntity *> temp;
    vector<PopEntity *> selected = entitySelection->selectEntities(count, entityList);

    for(it=selected.begin(); it!=selected.end(); it++){
        temp.push_back(new PopEntity(*it));
    }
    return temp;
}

vector<PopEntity *> Population::getSomeEntity(EntitySelection *entitySelection, quint32 count)
{
    return entitySelection->selectEntities(count, entityList);
}

vector<PopEntity *> Population::getAllEntitiesCopy()
{
    vector<PopEntity *>::iterator it;
    vector<PopEntity *> temp;
    for(it=entityList.begin(); it!=entityList.end(); it++){
        temp.push_back(new PopEntity(*it));
    }
    return temp;
}
vector<PopEntity *> Population::getAllEntities()
{
    return entityList;
}
