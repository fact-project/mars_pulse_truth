#ifndef MARS_MJOptimizeEnergy
#define MARS_MJOptimizeEnergy

#ifndef MARS_MJOptimize
#include "MJOptimize.h"
#endif

class MTask;

class MJOptimizeEnergy : public MJOptimize
{
private:
    Bool_t fOptimLog;

public:
    MJOptimizeEnergy() : MJOptimize(), fOptimLog(kFALSE) { }

    void EnableOptimLog(Bool_t b=kTRUE) { fOptimLog=b; }

    // Special optimizing routines
    Bool_t RunEnergy(const char *fname, const char *rule, MTask *weights=0);
    Bool_t RunEnergy(const char *rule, MTask *weights=0)
    {
        return RunEnergy(0, rule, weights);
    }

    ClassDef(MJOptimizeEnergy, 0) // Class for optimization of the Supercuts
};

#endif
