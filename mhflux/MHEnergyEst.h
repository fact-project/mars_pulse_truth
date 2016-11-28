#ifndef MARS_MHEnergyEst
#define MARS_MHEnergyEst

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
class MParList;
class MParameterD;
class MHMatrix;

class MHEnergyEst : public MH
{
private:
    //TString      fNameEnergy;
    //TString      fNameResult;

    MMcEvt      *fMcEvt;  //!
    MParameterD *fEnergy; //!
    MParameterD *fResult; //!

    Int_t        fMap[100]; // FIXME!
    MHMatrix    *fMatrix; //!

    TH3D fHEnergy;
    TH2D fHResolutionEst;
    TH2D fHResolutionMC;
    TH2D fHImpact;

    Double_t fChisq;
    Double_t fBias;

    TH1 *MakeProj(const char *how);
    TH1 *MakeProf(TH2 &h);
    void UpdateProf(TH2 &h, Bool_t logy);

    Double_t GetVal(Int_t i) const;
    void     CalcChisq(Double_t &chisq, Double_t &prob) const;

    Bool_t SetupFill(const MParList *pList);
    Int_t  Fill(const MParContainer *par, const Stat_t w=1);
    Bool_t Finalize();

public:
    MHEnergyEst(const char *name=NULL, const char *title=NULL);

    const TH3D &GetHEnergy() const { return fHEnergy; }
    void GetWeights(TH1D &hist) const;

    void InitMapping(MHMatrix *mat);
    void StopMapping();

    void Paint(Option_t *opt="");
    void Draw(Option_t *option="");
    void Print(Option_t *o="") const;

    ClassDef(MHEnergyEst, 2) // Histogram for the result of the energy reconstruction
};

#endif
