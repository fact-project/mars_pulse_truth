#ifndef MARS_MTFitLoop
#define MARS_MTFitLoop

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TArrayC
#include <TArrayC.h>
#endif

class MEvtLoop;

class MTFitLoop : public MParContainer
{
private:
    Int_t fDebug;     // -1 no output, 0 MTFitLoop output, 1 PrintStatistics output
    Int_t fNumEvents;

    static void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);
    virtual Double_t Fcn(Int_t &npar, Double_t *gin, Double_t *par, Int_t iflag);

    MEvtLoop *fEvtLoop;

    TString   fParametersName;
    TString   fFitParameter;

    TArrayC   fFixedParams;

    Int_t fNum;
    Int_t fMaxIterations;

public:
    MTFitLoop(const Int_t num=0);

    void Optimize(MEvtLoop &loop, TArrayD &pars);

    void SetNameParameters(const char *parm) { fParametersName = parm; }
    void SetFitParameter(const char *parm)   { fFitParameter   = parm; }

    void SetFixedParameters(const TArrayC &c) { fFixedParams = c; }
    void SetMaxIterations(Int_t maxiter=500) {fMaxIterations = maxiter;} // for debugging

    void SetDebug(Int_t n)     { fDebug = n; }
    void SetNumEvents(Int_t n) { fNumEvents = n; }

    ClassDef(MTFitLoop, 0) // Class which can optimize a value (chi^2, significance, etc) calculated in an eventloop
};

#endif
