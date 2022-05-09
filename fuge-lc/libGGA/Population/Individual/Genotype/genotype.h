#ifndef GENOTYPE_H
#define GENOTYPE_H
#include <Qt>
#include <QBitArray>
#include <QString>
#include <QDebug>

#include <memory>

using namespace std;
class Genotype
{
public:
    Genotype(quint32 length);
    Genotype(QBitArray *data);
    Genotype(Genotype *genotype);
    ~Genotype();

    Genotype *getCopy();

    quint32 getLength();
    QBitArray *getData();
    QBitArray *getDataCopy();
    void setData(QBitArray *data);
private:
    quint32 length;
    QBitArray *data;

};

#endif // GENOTYPE_H
