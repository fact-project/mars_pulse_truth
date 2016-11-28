#ifndef MARS_MEffAreaAndCoeffCalc
#define MARS_MEffAreaAndCoeffCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

class TF1;
class TH1F;
class TH2F;
class MHillas;
class MMcEvt;

#include "TChain.h"

class MEffAreaAndCoeffCalc
{
 private:

  TF1* fSpec;        // function used to parametrize the spectrum

  Float_t fEmin;    // Minimum energy in GeV
  Float_t fEmax;    // Maximum energy in GeV
  Int_t fEbins;      // number of bins to build spectrum
  Int_t fEsubbins;   // number of subbins per big bin (to compute weights, eff areas...)

  Float_t fLogEWmin; // Log Minimum energy for weights (in GeV)
  Float_t fLogEWmax; // Log Maximum energy for weights (in GeV)
  Int_t    fEWbins;   // Number of bins for weights

  Int_t fNTbins;      // Number of bins in zenith angle
  Double_t* fTbin;     // array containing bin boundaries (size must be fNTbins+)

  Double_t* fWeight; // array containing weights
  TH2F* fCoeff;      // histogram containing unfolding coefficients
  TH2F* fEffA;       // histogram containing effective areas

  TChain* fCini;     // chain for initial MC files (before trigger)
  TChain* fCcut;     // chain for surviving MC events (after cuts)

  MHillas* fHillas;  // pointer to the MHillas Branch
  MMcEvt*  fMcEvt;   // pointer to the MMcEvt Branch

  //  TFile* fFile; // output file (for debugging only)

 protected:

  void FillOriginalSpectrum();
  void ComputeCoefficients();
  void ComputeWeights();
  void ComputeEffectiveAreas();

 public:

  MEffAreaAndCoeffCalc();

  virtual ~MEffAreaAndCoeffCalc();

  void SetFunction(const Char_t* chfunc, Float_t emin=0., Float_t emax=0.);
  void SetFunction(TF1*);
  void SetEbins(Int_t i)    {fEbins=i;}
  void SetEsubbins(Int_t i) {fEsubbins=i;}
  void SetEmin(Float_t x)   {fEmin=x;}
  void SetEmax(Float_t x)   {fEmax=x;}

  void SetThetaBinning(Int_t n, const Double_t* binlist);

  void AddFile(const Char_t* name) {fCini->Add(name); fCcut->Add(name);}

  TH2F* GetEffectiveAreaHisto() {return fEffA;}
  TH2F* GetCoefficientHisto()   {return fCoeff;}

  void ComputeAllFactors();

  ClassDef(MEffAreaAndCoeffCalc, 0) // task to compute the Effective areas and Coefficients for the unfolding 
};

#endif

