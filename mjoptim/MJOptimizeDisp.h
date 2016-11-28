#ifndef MARS_MJOptimizeDisp
#define MARS_MJOptimizeDisp

#ifndef MARS_MJOptimize
#include "MJOptimize.h"
#endif

class MTask;

class MJOptimizeDisp : public MJOptimize
{
private:
    Bool_t fUseThetaSq;

public:
    MJOptimizeDisp() : MJOptimize(), fUseThetaSq(kFALSE) { }

    void EnableThetaSq(Bool_t b=kTRUE) { fUseThetaSq=b; }

    // Special optimizing routines
    Bool_t RunDisp(const char *fname, const char *rule, MTask *weights=0);
    Bool_t RunDisp(const char *rule, MTask *weights=0)
    {
        return RunDisp(0, rule, weights);
    }

    ClassDef(MJOptimizeDisp, 0) // Class for optimization of the Supercuts
};

#endif
