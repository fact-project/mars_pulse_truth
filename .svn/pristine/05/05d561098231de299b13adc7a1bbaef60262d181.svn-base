#ifndef MARS_MHCalibrationChargeBlindPix
#define MARS_MHCalibrationChargeBlindPix

#ifndef MARS_MHCalibrationPix
#include "MHCalibrationPix.h"
#endif

#include <TMath.h>

#ifndef ROOT_TF1
#include <TF1.h>
#endif

#ifndef ROOT_TVector
#include <TVector.h>
#endif

class TH1F;
class TPaveText;
class TText;
class MExtractedSignalBlindPixel;
class MRawEvtPixelIter;

class MHCalibrationChargeBlindPix : public MHCalibrationPix
{
private:

  static const Float_t  fgNumSinglePheLimit; //! Default for fNumSinglePheLimit (now set to: 50)
  static const Float_t  gkSignalInitializer; //! Signal initializer (-9999.)
  
  static const Double_t gkElectronicAmp;     // Electronic Amplification after the PMT (in FADC counts/N_e)
  static const Double_t gkElectronicAmpErr;  // Error of the electronic amplification

  Float_t fSinglePheCut;                     // Value of summed FADC slices upon which event considered as single-phe
  Float_t fNumSinglePheLimit;                // Minimum number of single-phe events 

//  TVector fASinglePheFADCSlices;             //! Averaged FADC slice entries supposed single-phe events
//  TVector fAPedestalFADCSlices;              //! Averaged FADC slice entries supposed pedestal   events
 
  TF1 *fSinglePheFit;                        // Single Phe Fit (Gaussians convoluted with Poisson) 

  UInt_t  fNumSinglePhes;                    // Number of entries in fASinglePheFADCSlices 
  UInt_t  fNumPedestals;                     // Number of entries in fAPedestalFADCSlices  

  Double_t  fLambda;                         // Poisson mean from Single-phe fit 
  Double_t  fLambdaCheck;                    // Poisson mean from Pedestal fit alone
  Double_t  fMu0;                            // Mean of the pedestal
  Double_t  fMu1;                            // Mean of single-phe peak
  Double_t  fSigma0;                         // Sigma of the pedestal
  Double_t  fSigma1;                         // Sigma of single-phe peak
  Double_t  fLambdaErr;                      // Error of Poisson mean from Single-phe fit 
  Double_t  fLambdaCheckErr;                 // Error of Poisson mean from Pedestal fit alone 
  Double_t  fMu0Err;                         // Error of  Mean of the pedestal    
  Double_t  fMu1Err;                         // Error of  Mean of single-phe peak 
  Double_t  fSigma0Err;                      // Error of  Sigma of the pedestal   
  Double_t  fSigma1Err;                      // Error of  Sigma of single-phe peak
  Double_t  fChisquare;                      // Chisquare of single-phe fit 
  Int_t     fNDF;                            // Ndof of single-phe fit 
  Double_t  fProb;                           // Probability of singleo-phe fit
				     
  Byte_t    fFlags;                          // Bit-field for the flags
  enum { kSinglePheFitOK, kPedestalFitOK };  // Possible bits to be set

  static Double_t FitFuncMichele(Double_t *x, Double_t *par);
  static Double_t PoissonKto4(Double_t *x, Double_t *par);
  static Double_t PoissonKto5(Double_t *x, Double_t *par);
  static Double_t PoissonKto6(Double_t *x, Double_t *par);
  static Double_t Polya(Double_t *x, Double_t *par);

public:

  enum FitFunc_t { kEPoisson4, kEPoisson5,
                   kEPoisson6, kEPoisson7,
                   kEPolya, kEMichele };    // Possible fit functions types 

private:  

  FitFunc_t fFitFunc;
  
  TPaveText *fFitLegend;                     //! Some legend to display the fit results
  TH1F      *fHSinglePheFADCSlices;          // A histogram created and deleted only in Draw()
  TH1F      *fHPedestalFADCSlices;           // A histogram created and deleted only in Draw()

  // Fit
  Bool_t InitFit();
  void   ExitFit();  
  
  void DrawLegend(Option_t *opt="");
  
public:

  MHCalibrationChargeBlindPix(const char *name=NULL, const char *title=NULL);
  ~MHCalibrationChargeBlindPix();

  void Clear(Option_t *o="");  
  void Reset() {}
  
  // Getters
  const Double_t GetLambda        ()  const { return fLambda;         }
  const Double_t GetLambdaCheck   ()  const { return fLambdaCheck;    }
  const Double_t GetMu0           ()  const { return fMu0;            }
  const Double_t GetMu1           ()  const { return fMu1;            }
  const Double_t GetSigma0        ()  const { return fSigma0;         }
  const Double_t GetSigma1        ()  const { return fSigma1;         }
  const Double_t GetLambdaErr     ()  const { return fLambdaErr;      }
  const Double_t GetLambdaCheckErr()  const { return fLambdaCheckErr; }
  const Double_t GetMu0Err        ()  const { return fMu0Err;         }
  const Double_t GetMu1Err        ()  const { return fMu1Err;         }
  const Double_t GetSigma0Err     ()  const { return fSigma0Err;      }
  const Double_t GetSigma1Err     ()  const { return fSigma1Err;      }
  const Float_t  GetSinglePheCut  ()  const { return fSinglePheCut;   }
 
//  TVector &GetASinglePheFADCSlices()             { return fASinglePheFADCSlices;  }
//  const TVector &GetASinglePheFADCSlices() const { return fASinglePheFADCSlices;  }

//  TVector &GetAPedestalFADCSlices()              { return fAPedestalFADCSlices;  }
//  const TVector &GetAPedestalFADCSlices()  const { return fAPedestalFADCSlices;  }

  const Bool_t  IsSinglePheFitOK()         const;
  const Bool_t  IsPedestalFitOK()          const;
  
  // Setters
  void SetFitFunc        ( const FitFunc_t func )                   { fFitFunc = func;  }
  void SetSinglePheCut      ( const Float_t cut = 0.               )    { fSinglePheCut      = cut;  }
  void SetNumSinglePheLimit ( const Float_t lim =fgNumSinglePheLimit )    { fNumSinglePheLimit = lim;  }

  void SetSinglePheFitOK    ( const Bool_t b=kTRUE);
  void SetPedestalFitOK     ( const Bool_t b=kTRUE);
  
  // Fill histos
//  void  FillSinglePheFADCSlices(const MRawEvtPixelIter &iter);
//  void  FillPedestalFADCSlices( const MRawEvtPixelIter &iter);
//  void  FinalizeSinglePheSpectrum();
  
  // Draws
  void Draw(Option_t *opt="");

  // Fits
  Bool_t FitSinglePhe (Option_t *opt="RL0+Q");
  void   FitPedestal  (Option_t *opt="RL0+Q");

  // Simulation
  Bool_t SimulateSinglePhe(const Double_t lambda,
                           const Double_t mu0,    const Double_t mu1,
                           const Double_t sigma0, const Double_t sigma1);
  
  ClassDef(MHCalibrationChargeBlindPix, 1)  // Histogram class for Charge Blind Pixel Calibration
};

#endif  /* MARS_MHCalibrationChargeBlindPix */
