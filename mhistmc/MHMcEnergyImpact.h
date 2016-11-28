#ifndef MARS_MHMcEnergyImpact
#define MARS_MHMcEnergyImpact

#ifndef MARS_MH
#include "MH.h"
#endif
#ifndef ROOT_TH2
#include <TH2.h>
#endif

class MMcEvt;

class MHMcEnergyImpact : public MH
{
private:
    TH2D fHist; // histogram with the logarith of the energy

public:
    MHMcEnergyImpact(const char *name=NULL, const char *title=NULL);

    void SetName(const char *name);
    void SetTitle(const char *title);

    Bool_t SetupFill(const MParList *pList);
    Bool_t Fill(const MParContainer *pcont=NULL, const Stat_t w=1);

    const TH2D *GetHist()       { return &fHist; }
    const TH2D *GetHist() const { return &fHist; }

    TH1 *GetHistByName(const TString name) { return &fHist; }

    void Draw(Option_t* option = "");

    ClassDef(MHMcEnergyImpact, 1)  // Histogram container for 2D histogram in Energy and Impact
};

#endif
