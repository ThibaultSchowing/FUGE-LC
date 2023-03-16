#ifndef LEARNINGENGINE_H
#define LEARNINGENGINE_H

#include <Qt>
#include "learningmethod.h"
#include <vector>
#include "fuzzyrule.h"

using namespace std;
class LearningEngine
{
public:

    LearningEngine();
    LearningEngine(LearningMethod *learningMethod, float learningFactor);
    ~LearningEngine();
    void setLearningMethod(LearningMethod *learningMethod);

    void setInOutVars(FuzzyVariable** inVars, int nbInVars,
                      FuzzyVariable** outVars, int nbOutVars);
    void setRules(FuzzyRule** rules, int nbRules);

    void resetLearningEngineInformation();

    void learn();

private:
    LearningMethod *learningMethod;

protected:
    FuzzyVariable** inVars;
    int nbInVars;
    FuzzyVariable** outVars;
    int nbOutVars;
    FuzzyRule** rules;
    int nbRules;

    float learningFactor;
};

#endif // LEARNINGENGINE_H
