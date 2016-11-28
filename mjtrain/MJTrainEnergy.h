#ifndef MARS_MJTrainEnergy
#define MARS_MJTrainEnergy

#ifndef MARS_MJTrainRanForest
#include "MJTrainRanForest.h"
#endif

class MDataSet;

class MJTrainEnergy : public MJTrainRanForest
{
private:
    TString fTrainParameter;
    TString fResultFunction;

    // To be moved to a base class
    TString fNameOutput;

public:
    MJTrainEnergy() : fNameOutput("MEnergyEst") { SetTrainLin(); }

    void SetTrainLog()     { SetTrainFunc("log(MMcEvt.fEnergy)", "exp(x)"); }
    void SetTrainLin()     { SetTrainFunc("MMcEvt.fEnergy", "x"); }
    void SetTrainExpSize() { SetTrainFunc("log(MMcEvt.fEnergy)/log(MHillas.fSize)", "MHillas.fSize^x"); }

    void SetTrainFunc(const char *par, const char *res)
    {
        fTrainParameter = par;
        fResultFunction = res;
    }

    Bool_t Train(const char *out, const MDataSet &set, Int_t num);

    ClassDef(MJTrainEnergy, 0)//Class to train Random Forest energy estimator
};


#endif
