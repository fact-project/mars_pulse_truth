#ifndef MARS_MHMcDifRate
#define MARS_MHMcDifRate

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef ROOT_TH1
#include <TH1.h>
#endif

class TF1;
class MMcEvt;
class MHMcCollectionArea;

class MHMcDifRate : public MParContainer
{
private:

    TH1D fHist;           // histogram with the logarith of the energy

    // const MMcEvt *fMcEvt;  //! container to fill histogram from

public:
    MHMcDifRate(const char *name=NULL, const char *title=NULL);

    void SetName(const char *name);
    void SetTitle(const char *title);

    const TH1D *GetHist()       { return &fHist; }
    const TH1D *GetHist() const { return &fHist; }

    TH1 *GetHistByName(const TString name) { return &fHist; }

    void Draw(Option_t* option = "");

//    void Calc(const TH2D &hsel, const TH2D &hall);
    void Calc(const MHMcCollectionArea &cola, const TF1 &spect);

    ClassDef(MHMcDifRate, 1)  // Histogram container for differential trigger rate
};

#endif
