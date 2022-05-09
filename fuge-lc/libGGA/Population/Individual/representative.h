#ifndef REPRESENTATIVE_H
#define REPRESENTATIVE_H

#include <Qt>
#include "popentity.h"

class Representative : public PopEntity
{
public:
    Representative(quint32 lenght);
    Representative(Genotype *genotype);
    Representative(Representative *representative);
};

#endif // REPRESENTATIVE_H
