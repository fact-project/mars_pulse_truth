#ifndef MARS_MFEventSelector
#define MARS_MFEventSelector

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

class MParList;

class MFEventSelector : public MFilter
{
private:
    Int_t   fNumTotalEvts;    // Number of total events from which are selected
    Int_t   fNumSelectEvts;   // Number of events to be selected
    Float_t fSelRatio;        // Selection Probability

    Int_t   fNumSelectedEvts; //! Number of events which have been selected

    Bool_t  fResult;          //! Reseult of a single selection

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

    enum { kNumTotalFromFile = BIT(14) };

public:
    MFEventSelector(const char *name=NULL, const char *title=NULL);

    Bool_t IsExpressionTrue() const { return fResult; }

    void SetNumTotalEvts(Int_t n)  { fNumTotalEvts = n; ResetBit(kNumTotalFromFile); }
    void SetNumSelectEvts(Int_t n) { fNumSelectEvts = n; }
    void SetSelectionRatio(Float_t f);

    ClassDef(MFEventSelector, 0) // A filter to do a random selection of events
};

#endif
