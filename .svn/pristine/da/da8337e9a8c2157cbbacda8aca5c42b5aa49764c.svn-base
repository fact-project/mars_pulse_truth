#ifndef MARS_MHMcEfficiency
#define MARS_MHMcEfficiency

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif
#ifndef ROOT_TH2
#include <TH2.h>
#endif

class MMcEvt;
class MHMcEnergyImpact;

class MHMcEfficiency : public MParContainer
{
private:

    TH2D fHist;           // histogram with the logarith of the energy

    // const MMcEvt *fMcEvt;  //! container to fill histogram from

public:
    MHMcEfficiency(const char *name=NULL, const char *title=NULL);

    void SetName(const char *name);
    void SetTitle(const char *title);

    const TH2D *GetHist()       { return &fHist; }
    const TH2D *GetHist() const { return &fHist; }

    TH1 *GetHistByName(const TString name) { return &fHist; }

    void Draw(Option_t* option = "");

    void Calc(const TH2D &hsel, const TH2D &hall);
    void Calc(const MHMcEnergyImpact &mcsel, const MHMcEnergyImpact &mcall);

    ClassDef(MHMcEfficiency, 1)  // Histogram container for 2D trigger efficiency
};

#endif
