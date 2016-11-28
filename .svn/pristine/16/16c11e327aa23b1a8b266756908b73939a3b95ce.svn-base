#ifndef MARS_MHThetaSq
#define MARS_MHThetaSq

#ifndef MARS_MHAlpha
#include "MHAlpha.h"
#endif

class MTaskList;

class MHThetaSq : public MHAlpha
{
private:
    UInt_t fNumBinsSignal;
    UInt_t fNumBinsTotal;

    Bool_t      GetParameter(const MParList &pl);
    Double_t    GetVal() const;
    const char *GetParameterRule() const
    {
        return "ThetaSquared.fVal";
    }

    Bool_t SetupFill(const MParList *pl);

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

public:
    MHThetaSq(const char *name=NULL, const char *title=NULL);

    void InitMapping(MHMatrix *mat, Int_t type=0);

    void SetNumBinsSignal(UInt_t n) { fNumBinsSignal=TMath::Max(n, 1U); }
    void SetNumBinsTotal(UInt_t n)  { fNumBinsTotal =TMath::Max(n, 1U); }

    ClassDef(MHThetaSq, 2) // Theta-Plot which is fitted online
};

#endif
