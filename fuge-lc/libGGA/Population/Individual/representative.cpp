#include "representative.h"

Representative::Representative(quint32 lenght) : PopEntity(lenght)
{
}

Representative::Representative(Genotype *genotype) : PopEntity(genotype)
{
}

Representative::Representative(Representative *representative) : PopEntity(representative)
{
}
