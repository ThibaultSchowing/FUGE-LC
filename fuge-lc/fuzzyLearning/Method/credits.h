#ifndef CREDITS_H
#define CREDITS_H

#include "learningmethod.h"

class Credits : public LearningMethod
{
public:
    Credits();
    ~Credits();
    void executeLearn(FuzzyVariable** inVars, int nbInVars,
                      FuzzyVariable** outVars, int nbOutVars,
                      FuzzyRule** rules, int nbRules, float learningFactor);
};

#endif // CREDITS_H
