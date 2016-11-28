#ifndef MARS_MEnergyEstimate
#define MARS_MEnergyEstimate

#ifndef MARS_MParameterCalc
#include "MParameterCalc.h"
#endif

class MEnergyEstimate : public MParameterCalc
{
public:
    MEnergyEstimate(const char *name=NULL, const char *title=NULL);

    ClassDef(MEnergyEstimate, 1) // Task to estimate the energy by a rule
};

#endif

