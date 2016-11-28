#ifndef MARS_MHCollectionArea
#define MARS_MHCollectionArea

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef ROOT_TH1
#include <TH1.h>
#endif

#ifndef ROOT_TH2
#include <TH2.h>
#endif

class MMcEvt;
class MParameterD;
class MMcRunHeader;

class MHCollectionArea : public MH
{
private:
    MMcEvt *fMcEvt;             //!
    MMcRunHeader *fHeader;      //!
    //MParameterD *fEnergy;     //!

    TH2D fHistSel;
    TH2D fHistAll;

    TH1D fHEnergy;

    Float_t fMcAreaRadius; // [m] Radius of circle within which the cores of Corsika events have been uniformly distributed.

    UInt_t fTotalNumSimulatedShowers;
    UInt_t fCorsikaVersion;
    Bool_t fAllEvtsTriggered;
    Bool_t fIsExtern;

    void GetImpactMax();
    void Calc(TH2D &hsel, TH2D &hall);
    void CalcEfficiency();

public:
    MHCollectionArea(const char *name=NULL, const char *title=NULL);

    Bool_t SetupFill(const MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t weight=1);
    Bool_t Finalize();

    void Draw(Option_t *option="");
    void Paint(Option_t *option="");

    void SetHistAll(const TH2D &h) { h.Copy(fHistAll); fIsExtern=kTRUE; }

    TH2D &GetHistAll()             { return fHistAll; }
    const TH2D &GetHistAll() const { return fHistAll; }
    const TH2D &GetHistSel() const { return fHistSel; }
    const TH1D &GetHEnergy() const { return fHEnergy; }

    Double_t GetEntries() const { return fHistAll.Integral(); }
    Double_t GetCollectionAreaEff() const { return fHEnergy.Integral(); }
    Double_t GetCollectionAreaAbs() const;

    void SetMcAreaRadius(Float_t x) { fMcAreaRadius = x; }

    ClassDef(MHCollectionArea, 2)  // Data Container to calculate Collection Area
};

#endif
