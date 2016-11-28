#ifndef MARS_MJTrainImpact
#define MARS_MJTrainImpact

#ifndef MARS_MJTrainRanForest
#include "MJTrainRanForest.h"
#endif

class MDataSet;

class MJTrainImpact : public MJTrainRanForest
{
private:
    TString fTrainParameter;
    TString fResultFunction;

    // To be moved to a base class
    TString fNameOutput;

public:
    MJTrainImpact() : fNameOutput("MImpactEst") { SetTrainSq(); }

    void SetTrainSq()  { SetTrainFunc("MMcEvt.fImpact^2", "sqrt(x)"); }
    void SetTrainLin() { SetTrainFunc("MMcEvt.fEnergy", "x"); }

    void SetTrainFunc(const char *par, const char *res)
    {
        fTrainParameter = par;
        fResultFunction = res;
    }

    Bool_t Train(const char *out, const MDataSet &set, Int_t num);

    ClassDef(MJTrainImpact, 0)//Class to train Random Forest impact estimator
};


#endif
