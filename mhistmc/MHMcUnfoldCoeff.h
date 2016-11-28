#ifndef MARS_MHMcUnfoldCoeff
#define MARS_MHMcUnfoldCoeff

#ifndef MARS_MH
#include "MH.h"
#endif

class TF1;
class TH1D;
class TH2D;
class MBinning;

class MHMcUnfoldCoeff : public MH
{
 private:

  TH1D* fHistAll;    // histogram of all events (fine bins)
  TH1D* fHistWeight; // histogram of weights    (fine bins)

  TH2D* fHistMcE;    // histogram of MC energy for survivors (coarse bins)
  TH2D* fHistEstE;   // histogram of estimated energy for survivors (coarse bins)
  TH2D* fHistCoeff;  // histogram of coefficients for iterative unfolding

  Int_t fNsubbins;   // number of subbins per coarse bin
  Double_t fEmin;    // [GeV] absolute minimum energy where weights are computed
  Double_t fEmax;    // [GeV] absolute maximum energy where weights are computed
  Int_t fNumMin;     // minimum number of events in a fine bin required to compute the weight

  MBinning* fFineBinning; // fine binning (used for weights)
  
 public:

  MHMcUnfoldCoeff(const char *name=NULL, const char *title=NULL);

  virtual ~MHMcUnfoldCoeff();

  void FillAll(Double_t energy);
  void FillSel(Double_t mcenergy,Double_t estenergy,Double_t theta);
  void Draw(Option_t* option="");
  void ComputeWeights(TF1* spectrum);
  void ComputeCoefficients();

  void SetCoarseBinnings(const MBinning& binsEnergy,const MBinning& binsTheta);
  void SetNsubbins(Int_t n)   {fNsubbins=n;}
  void SetEmax(Double_t emax) {fEmax=emax;}
  void SetEmin(Double_t emin) {fEmin=emin;}
  void SetNumMin(Int_t n)     {fNumMin=n;}
    
  const TH1D* GetHistAll()    const {return fHistAll;}
  const TH1D* GetHistWeight() const {return fHistWeight;}
  const TH2D* GetHistMcE()    const {return fHistMcE;}
  const TH2D* GetHistEstE()   const {return fHistEstE;}
  const TH2D* GetHistCoeff()  const {return fHistCoeff;}

  ClassDef(MHMcUnfoldCoeff, 1) // Data Container to store Unfolding Coefficients
};

#endif

