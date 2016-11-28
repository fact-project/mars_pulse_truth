#ifndef MARS_MHThreshold
#define MARS_MHThreshold

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef ROOT_TH1
#include <TH1.h>
#endif

class MMcEvt;

class MHThreshold : public MH
{
private:
    const MMcEvt *fMcEvt; //! Pointer to MC energy

    TH1D fHEnergy;

public:
    MHThreshold(const char *name=NULL, const char *title=NULL);

    Bool_t SetupFill(const MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t weight=1);

    void Draw(Option_t *option="");
    void Paint(Option_t *option="");

    ClassDef(MHThreshold, 1)  // Data Container to calculate threshold
};

#endif
