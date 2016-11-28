#ifndef MARS_MJTrainRanForest
#define MARS_MJTrainRanForest

#ifndef MARS_MJOptimizeBase
#include "MJOptimizeBase.h"
#endif

class MTask;
class MFilter;

class MJTrainRanForest : public MJOptimizeBase
{
protected:
    UShort_t fNumTrees;
    UShort_t fNdSize;
    UShort_t fNumTry;

public:
    MJTrainRanForest()
    {
        fNumTrees = 100; //100
        fNumTry   = 0;   //3   0 means: in MRanForest estimated best value will be calculated
        fNdSize   = 1;   //1
    }

    void SetNumTrees(UShort_t n=100)   { fNumTrees = n; }
    void SetNdSize(UShort_t n=5)       { fNdSize   = n; }
    void SetNumTry(UShort_t n=0)       { fNumTry   = n; }

    ClassDef(MJTrainRanForest, 0)//Base class for Random Forest training classes
};

#endif
