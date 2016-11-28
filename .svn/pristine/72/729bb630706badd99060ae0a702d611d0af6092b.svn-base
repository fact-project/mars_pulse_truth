#ifndef MARS_MFEnergySlope
#define MARS_MFEnergySlope

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

class MMcEvt;
class MParList;
class MMcCorsikaRunHeader;

class MFEnergySlope : public MFilter
{
private:
    //Int_t fNumSelectedEvts; // counter for number of selected events

    MMcEvt *fEvt;           //! Events used to determin energy slope

    Float_t fNewSlope;      // New slope set by user
    Float_t fMcSlope;       //! Original energy slope from MC data

    Float_t fMcMinEnergy;   //! Starting energy of MC data
    Float_t fMcMaxEnergy;   //! Ending energy of MC data

    Float_t fN0;            //! Normalization factor

    Bool_t  fResult;        //! Result returned by IsExpressionTrue

    // MTask
    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Process();

    // MFilter
    Bool_t IsExpressionTrue() const { return fResult; }

public:
    MFEnergySlope(const char *name=NULL, const char *title=NULL);
    MFEnergySlope(Float_t slope, const char *name=NULL, const char *title=NULL);
    MFEnergySlope(Float_t slope, Float_t emin, const char *name=NULL, const char *title=NULL);

    // Setter
    void SetNewSlope(Float_t f)    { fNewSlope = TMath::Abs(f); }
    void SetMcSlope(Float_t f)     { fMcSlope  = TMath::Abs(f); }

    void SetMcMinEnergy(Float_t f) { fMcMinEnergy = f; }
    void SetMcMaxEnergy(Float_t f) { fMcMaxEnergy = f; }

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    ClassDef(MFEnergySlope, 0) // A Filter to select events with a given energy slope
};

#endif






