#ifndef MARS_MMcSpectrumWeight
#define MARS_MMcSpectrumWeight

#ifndef MARS_MTask
#include "MTask.h"
#endif

class TF1;
class TH1;
class TH1D;
class TSpline3;
class MParList;
class MMcEvt;
class MHillas;
class MParameterD;
class MPointingPos;
class MMcCorsikaRunHeader;

class MMcSpectrumWeight : public MTask
{
private:
    const MMcEvt  *fMcEvt;   // Pointer to the container with the MC energy
    const MHillas *fHillas;
    MParameterD   *fWeight;  // Pointer to the output MWeight container
    MPointingPos  *fPointing;

    TString fNameWeight;    // Name of the MWeight container
    TString fNameMcEvt;     // Name of the MMcEvt container

    TF1      *fFunc;        // Function calculating the weights
    TH1      *fWeightsZd;   // Set additional ZA weights
    TH1      *fWeightsSize; // Set additional ZA weights
//    TSpline3 *fWeightsSize;

    Double_t fOldSlope;     // Slope of energy spectrum generated with Corsika
    Double_t fNewSlope;     // Slope of the new spectrum (if it is a power law)

    Double_t fEnergyMin;    // Minimum energy simulated
    Double_t fEnergyMax;    // Maximum energy simulated

    Double_t fNorm;         // Normalization constant (additional normalization constant)
    Double_t fNormEnergy;   // Energy at which the spectra are normalized (default -1 means the integral is used)

    TString fFormula;       // Text Formula for new spectrum: eg. "pow(MMcEvt.fEnergy, -2.0)"

    Bool_t fAllowChange;

    // MMcSpectrumWeight
    void Init(const char *name, const char *title);
    TString ReplaceX(TString) const;

    // MTask
    Bool_t ReInit(MParList *plist); 
    Int_t  PreProcess(MParList *pList);
    Int_t  Process();

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
    MMcSpectrumWeight(const char *name=NULL, const char *title=NULL);
    ~MMcSpectrumWeight();

    // Setter
    void SetNameWeight(const char *n="MWeight") { fNameWeight = n; }
    void SetNameMcEvt(const char *n="MMcEvt") { fNameMcEvt = n; }
    void SetNewSlope(Double_t s=-1) { fNewSlope = s; }
    void SetNorm(Double_t s=1) { fNorm = s; }
    void SetNormEnergy(Double_t s=1) { fNormEnergy = s; }
    void SetFormula(const char *f="") { fFormula = f; }
    void SetEnergyRange(Double_t min=-2, Double_t max=-1) { fEnergyMin=min; fEnergyMax=max; }
    void SetOldSlope(Double_t s=-2.6) { fOldSlope=s; }
    void SetWeightsZd(TH1 *h=0) { fWeightsZd = h; }
    void SetWeightsSize(TH1D *h=0);

    Bool_t Set(const MMcCorsikaRunHeader &h);

    // Getter
    TString GetFormulaSpecOld(const char *name) const;
    TString GetFormulaSpecNew(const char *name) const;
    TString GetFormulaWeights(const char *name) const;
    TString GetFormulaSpecOld() const { return GetFormulaSpecOld(fNameMcEvt); }
    TString GetFormulaSpecNew() const { return GetFormulaSpecNew(fNameMcEvt); }
    TString GetFormulaWeights() const { return GetFormulaWeights(fNameMcEvt); }

    TString GetFormulaSpecOldX() const { return ReplaceX(GetFormulaSpecOld()); }
    TString GetFormulaSpecNewX() const { return ReplaceX(GetFormulaSpecNew()); }
    TString GetFormulaWeightsX() const { return ReplaceX(GetFormulaWeights()); }

    Double_t GetSpecNewIntegral(Double_t emin, Double_t emax) const;
    Double_t GetSpecOldIntegral(Double_t emin, Double_t emax) const;

    Double_t GetSpecNewIntegral() const { return GetSpecNewIntegral(fEnergyMin, fEnergyMax); }
    Double_t GetSpecOldIntegral() const { return GetSpecOldIntegral(fEnergyMin, fEnergyMax); }

    Double_t CalcSpecNew(Double_t e) const;
    Double_t CalcSpecOld(Double_t e) const;

    Double_t GetEnergyMin() const { return fEnergyMin; }
    Double_t GetEnergyMax() const { return fEnergyMax; }

    // Functions
    void CompleteEnergySpectrum(TH1 &h, Double_t emin, Double_t scale=0) const;

    // TObject
    void Print(Option_t *o="") const;

    ClassDef(MMcSpectrumWeight, 0) // Task to calculate weights to change the energy spectrum
};

#endif 
