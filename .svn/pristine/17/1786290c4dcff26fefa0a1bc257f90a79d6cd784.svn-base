#ifndef MARS_MHRanForest
#define MARS_MHRanForest

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif

class TH1D;
class TGraph;
class MParList;
class MMcEvt;
class MRanForest;

class MHRanForest : public MH
{
private:
    const MMcEvt *fMcEvt;           //!
    const MRanForest *fRanForest;   //!

    TArrayF fSigma;                 //!
    Int_t fNumEvent;                //!
    TGraph *fGraphSigma;            //->

public:
    MHRanForest(Int_t nbins=100, const char *name=NULL, const char *title=NULL);
    ~MHRanForest();

    TGraph *GetGraphSigma() const  { return fGraphSigma; }

    Bool_t SetupFill(const MParList *plist);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Finalize();

    void Draw(Option_t *opt="");

    ClassDef(MHRanForest, 1) // Histogram showing variance of estimated Hadronness
};

#endif
