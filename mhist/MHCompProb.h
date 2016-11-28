#ifndef MARS_MHCompProb
#define MARS_MHCompProb

#ifndef MARS_MH
#include "MH.h"
#endif

class MParList;

class MHCompProb : public MH
{
private:
    Int_t fNumLoop;   //! Counter of the loop (two eventloops needed)

    TList *fRules;    // Rules describing the used data sets

    TList *fData;     //! MDataChain objects
    TList *fHists;    //! fixed bin size histograms
    TList *fHistVar;  // variable bin size histograms

    void Fill(TList &list);
    void SetBinningHistVar();

public:
    MHCompProb(Int_t nbins, const char *name=NULL, const char *title=NULL);
    ~MHCompProb();

    void Add(const char *rule, Int_t n, Float_t min, Float_t max);

    Bool_t SetupFill(const MParList *plist);
    Bool_t Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Finalize();

    const TList *GetRules() const   { return fRules; }
    TList *GetHistVar() const { return fHistVar; }

    ClassDef(MHCompProb, 1) // Histogram to be used for the calculation of the composite probabilities
};

#endif
