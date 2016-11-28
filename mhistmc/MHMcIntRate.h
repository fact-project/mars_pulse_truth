#ifndef MARS_MHMcIntRate
#define MARS_MHMcIntRate

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef ROOT_TH1
#include <TH1.h>
#endif

class MHMcDifRate;

class MHMcIntRate : public MParContainer
{
private:

    TH1D fHist;           // histogram with the logarith of the energy

    // const MMcEvt *fMcEvt;  //! container to fill histogram from

public:
    MHMcIntRate(const char *name=NULL, const char *title=NULL);

    void SetName(const char *name);
    void SetTitle(const char *title);

    const TH1D *GetHist()       { return &fHist; }
    const TH1D *GetHist() const { return &fHist; }

    TH1 *GetHistByName(const TString name) { return &fHist; }

    void Draw(Option_t* option = "");

    void Calc(const MHMcDifRate &rate);

    ClassDef(MHMcIntRate, 1)  // Histogram container for integral event rate
};

#endif
