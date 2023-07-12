#ifndef WINNERTAKESALL_H
#define WINNERTAKESALL_H

#include "learningmethod.h"
#include <QDebug>

class WinnerTakesAll : public LearningMethod
{
public:
    WinnerTakesAll();
    ~WinnerTakesAll();
    void executeLearn(FuzzyVariable** inVars, int nbInVars,
                      FuzzyVariable** outVars, int nbOutVars,
                      FuzzyRule** rules, int nbRules, float learningFactor);
};

#endif // WINNERTAKESALL_H
