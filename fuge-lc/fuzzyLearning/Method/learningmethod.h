#ifndef LEARNINGMETHOD_H
#define LEARNINGMETHOD_H

#include "fuzzyrule.h"
#include "fuzzyvariable.h"

class LearningMethod
{
public:
    LearningMethod();
    virtual ~LearningMethod();
    virtual void executeLearn(FuzzyVariable** inVars, int nbInVars,
                              FuzzyVariable** outVars, int nbOutVars,
                              FuzzyRule** rules, int nbRules, float learningFactor) = 0;
};

#endif // LEARNINGMETHOD_H
