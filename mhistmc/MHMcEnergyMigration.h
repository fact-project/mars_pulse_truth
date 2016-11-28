#ifndef MARS_MHMcEnergyMigration
#define MARS_MHMcEnergyMigration

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef ROOT_TH3
#include <TH3.h>
#endif

#ifndef ROOT_TH2
#include <TH2.h>
#endif

class MMcEvt;
class MEnergyEst;
class MParList;

class MHMcEnergyMigration : public MH
{
private:
    MMcEvt      *fMcEvt; //!
    MEnergyEst  *fEnergy; //!

    TH3D        fHist;
    TH3D        fHist2;
    TH2D        fHistImp;

public:
    MHMcEnergyMigration(const char *name=NULL, const char *title=NULL);

    Bool_t SetupFill(const MParList *pList);
    Bool_t Fill(const MParContainer *par, const Stat_t w=1);

    const TH3D *GetHist() { return &fHist; }
    const TH3D *GetHist() const { return &fHist; }
    TH1 *GetHistByName(const TString name) { return &fHist; }

    const TH3D *GetHist2() { return &fHist2; }
    const TH2D *GetHistImp() { return &fHistImp; }

    void Draw(Option_t *option="");

    ClassDef(MHMcEnergyMigration, 1) //3D-histogram   E-true E-est Theta

};

#endif
