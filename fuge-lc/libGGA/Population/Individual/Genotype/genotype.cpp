#include "genotype.h"

Genotype::~Genotype(){
    if(data != 0)
        delete data;
}

Genotype::Genotype(quint32 length) : length(length),
    data(new QBitArray(length, false))
{

}

Genotype::Genotype(QBitArray *data)
{

    this->data = new QBitArray(*data);
    length = this->data->size();
}

Genotype::Genotype(Genotype *genotype)
{
    Genotype(genotype->getData());
}
Genotype *Genotype::getCopy(){
    return new Genotype(data);
}

quint32 Genotype::getLength(){
    return length;
}

QBitArray* Genotype::getData(){
    return data;
}

QBitArray* Genotype::getDataCopy(){
    return new QBitArray(*data);
}

void Genotype::setData(QBitArray *data){
    if(this->data!=NULL)
        delete data;
    this->data = data;
}
