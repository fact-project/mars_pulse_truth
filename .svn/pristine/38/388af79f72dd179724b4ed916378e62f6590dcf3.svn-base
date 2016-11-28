#ifndef MARS_MHGausEvents
#define MARS_MHGausEvents

#ifndef ROOT_TH1
#include <TH1.h>
#endif

#ifndef ROOF_TF1
#include <TF1.h>
#endif

#ifndef MARS_MH
#include "MH.h"
#endif

#ifndef MARS_MArrayF
#include "MArrayF.h"
#endif

class TVirtualPad;
class TGraph;
class TH1F;
class TH1I;
class TF1;

class MHGausEvents : public MH
{
private:

  const static Int_t   fgNDFLimit;             //! Default for fNDFLimit             (now set to: 2)
  const static Float_t fgProbLimit;            //! Default for fProbLimit            (now set to: 0.001)
  const static Int_t   fgPowerProbabilityBins; //! Default for fPowerProbabilityBins (now set to: 20)

private:

  Int_t    fBinsAfterStripping;        // Bins for the Gauss Histogram after stripping off the zeros at both ends
  UInt_t   fCurrentSize;               // Current size of the array fEvents
  Float_t  fEventFrequency;            // Event frequency in Hertz (to be set)
  Byte_t   fFlags;                     // Bit field for the fit result bits
  Int_t    fPowerProbabilityBins;      // Bins for the projected power spectrum
  
  TH1I    *fHPowerProbability;         //! Fourier transform of fEvents projected on y-axis
  MArrayF *fPowerSpectrum;             //! Fourier transform of fEvents

  enum  { kGausFitOK,
          kExpFitOK,
          kFourierSpectrumOK,
          kExcluded };                 // Bits for information about fit results 
  
  MArrayF  fEvents;                    // Array which holds the entries of GausHist
  TF1     *fFExpFit;                   // Exponential fit for FHPowerProbability
  TGraph  *fGraphEvents;               //! TGraph to display the event array 
  TGraph  *fGraphPowerSpectrum;        //! TGraph to display the power spectrum array 
  Axis_t   fFirst;                     // Lower histogram edge  for fHGausHist (used by InitBins()) 
  Axis_t   fLast;                      // Upper histogram edge  for fHGausHist (used by InitBins()) 
  Int_t    fNbins;                     // Number histogram bins for fHGausHist (used by InitBins())
  Int_t    fNDFLimit;                  // NDF limit for judgement if fit is OK
  Float_t  fProbLimit;                 // Probability limit for judgement if fit is OK 

protected:
  TF1     *fFGausFit;                  // Gauss fit for fHGausHist
  TH1F     fHGausHist;                 // Histogram to hold the Gaussian distribution

  Double_t fMean;                      // Mean of the Gauss fit
  Double_t fMeanErr;                   // Error of the mean of the Gauss fit
  Double_t fSigma;                     // Sigma of the Gauss fit
  Double_t fSigmaErr;                  // Error of the sigma of the Gauss fit
  Double_t fProb;                      // Probability of the Gauss fit 

  // Setters
  void  SetBinsAfterStripping   ( const Int_t nbins=0   )                    { fBinsAfterStripping  =nbins; }
  void  SetPowerProbabilityBins ( const Int_t nbins=fgPowerProbabilityBins ) { fPowerProbabilityBins=nbins; }

public:

  MHGausEvents(const char* name=NULL, const char* title=NULL);
  ~MHGausEvents();

  void Clear(Option_t *o="");
  void Reset();  

  void CreateFourierSpectrum();         
  void CreateGraphEvents();             
  void CreateGraphPowerSpectrum(); 

  // Draws
  void Draw               ( Option_t *option="" ); // *MENU*
  void DrawEvents         ( Option_t *option="" ); // *MENU*
  void DrawPowerSpectrum  ( Option_t *option="" ); // *MENU*
  void DrawPowerProjection( Option_t *option="" ); // *MENU*

  // Fill
  void   FillArray        ( const Float_t f ); 
  Bool_t FillHist         ( const Float_t f ); 
  Bool_t FillHistAndArray ( const Float_t f ); 
  
  // Fits
  Bool_t FitGaus          ( Option_t *option="RQ0",         
			    const Double_t xmin=0., 
			    const Double_t xmax=0.); // *MENU*
  
  // Inits
  virtual void InitBins();
  
  // Getters
  const Double_t GetEntries()            const { return fHGausHist.GetEntries(); }
  const Double_t GetChiSquare()          const { return ( fFGausFit ? fFGausFit->GetChisquare() : 0.); }
  const Double_t GetExpChiSquare()       const { return ( fFExpFit  ? fFExpFit->GetChisquare()  : 0.); }
  const Int_t    GetExpNdf()             const { return ( fFExpFit  ? fFExpFit->GetNDF()        : 0 ); }
  const Double_t GetExpProb()            const { return ( fFExpFit  ? fFExpFit->GetProb()       : 0.); }
        MArrayF *GetEvents()                   { return &fEvents;            }  
  const MArrayF *GetEvents()             const { return &fEvents;            }
  const Float_t  GetEventFrequency ()    const { return fEventFrequency;     }
        TF1     *GetFExpFit()                  { return fFExpFit;            }
  const TF1     *GetFExpFit()            const { return fFExpFit;            } 
        TF1     *GetFGausFit()                 { return fFGausFit;           }
  const TF1     *GetFGausFit()           const { return fFGausFit;           } 
        TGraph  *GetGraphEvents()              { return fGraphEvents;        }
  const Double_t GetFirst()              const { return fFirst;              }
  const Double_t GetLast ()              const { return fLast ;              }  
  const TGraph  *GetGraphEvents()        const { return fGraphEvents;        }
        TGraph  *GetGraphPowerSpectrum()       { return fGraphPowerSpectrum; }
  const TGraph  *GetGraphPowerSpectrum() const { return fGraphPowerSpectrum; }
        TH1F    *GetHGausHist()                { return &fHGausHist;         }
  const TH1F    *GetHGausHist()          const { return &fHGausHist;         } 
        TH1I    *GetHPowerProbability()        { return fHPowerProbability;  }
  const TH1I    *GetHPowerProbability()  const { return fHPowerProbability;  } 
  const Double_t GetHistRms()            const { return fHGausHist.GetRMS(); }
  const Double_t GetMean()               const { return fMean;               }
  const Double_t GetMeanErr()            const { return fMeanErr;            }
  const Int_t    GetNdf()                const { return ( fFGausFit ? fFGausFit->GetNDF()       : 0);  }
  const Int_t    GetNbins()              const { return fNbins;              }
  const Double_t GetOffset()             const { return ( fFExpFit  ? fFExpFit->GetParameter(0) : 0.); }
        MArrayF *GetPowerSpectrum()            { return fPowerSpectrum;      }  
  const MArrayF *GetPowerSpectrum()      const { return fPowerSpectrum;      }
  const Double_t GetProb()               const { return fProb;               }
  const Double_t GetSigma()              const { return fSigma;              }
  const Double_t GetSigmaErr()           const { return fSigmaErr;           }
  const Double_t GetSlope()              const { return ( fFExpFit  ? fFExpFit->GetParameter(1) : 0.); }
  const Int_t    GetNDFLimit()           const { return fNDFLimit; }
  const Float_t  GetProbLimit()          const { return fProbLimit; }

  const Bool_t   IsValid()               const { return fMean!=0 || fSigma!=0; }
  const Bool_t   IsExcluded()            const { return TESTBIT(fFlags,kExcluded);          }
  const Bool_t   IsExpFitOK()            const { return TESTBIT(fFlags,kExpFitOK);          }
  const Bool_t   IsEmpty()               const { return !(fHGausHist.GetEntries());         }
  const Bool_t   IsFourierSpectrumOK()   const { return TESTBIT(fFlags,kFourierSpectrumOK); }
  const Bool_t   IsGausFitOK()           const { return TESTBIT(fFlags,kGausFitOK);         }
  const Bool_t   IsOnlyOverflow()        const { return fHGausHist.GetEntries()>0 && fHGausHist.GetEntries() == fHGausHist.GetBinContent(fNbins+1); }
  const Bool_t   IsOnlyUnderflow()       const { return fHGausHist.GetEntries()>0 && fHGausHist.GetEntries() == fHGausHist.GetBinContent(0);        }

  // Prints
  void Print(const Option_t *o="") const;       // *MENU*
  
  // Setters
  void  SetEventFrequency   ( const Float_t  f                   ) { fEventFrequency = f;   }
  void  SetExcluded         ( const Bool_t   b=kTRUE             ) { b ? SETBIT(fFlags,kExcluded)  : CLRBIT(fFlags,kExcluded); }
  void  SetExpFitOK         ( const Bool_t   b=kTRUE             ) { b ? SETBIT(fFlags,kExpFitOK)  : CLRBIT(fFlags,kExpFitOK); }
  void  SetFourierSpectrumOK( const Bool_t   b=kTRUE             ) { b ? SETBIT(fFlags,kFourierSpectrumOK) : CLRBIT(fFlags,kFourierSpectrumOK); }
  void  SetGausFitOK        ( const Bool_t   b=kTRUE             ) { b ? SETBIT(fFlags,kGausFitOK) : CLRBIT(fFlags,kGausFitOK);}
  void  SetMean             ( const Double_t d                   ) { fMean           = d;   }
  void  SetMeanErr          ( const Double_t d                   ) { fMeanErr        = d;   }
  void  SetNDFLimit         ( const Int_t    lim=fgNDFLimit      ) { fNDFLimit       = lim; }  
  void  SetProb             ( const Double_t d                   ) { fProb           = d;   }
  void  SetProbLimit        ( const Float_t  lim=fgProbLimit     ) { fProbLimit      = lim; }
  void  SetSigma            ( const Double_t d                   ) { fSigma          = d;   }
  void  SetSigmaErr         ( const Double_t d                   ) { fSigmaErr       = d;   }

  void SetBinning(Int_t i, Axis_t lo, Axis_t up) { fNbins=i; fFirst=lo; fLast=up; }

  // Simulates
  void  SimulateGausEvents(const Float_t mean, const Float_t sigma, const Int_t nevts=4096);     // *MENU*

  ClassDef(MHGausEvents, 4) // Base class for events with Gaussian distributed values
};

#endif
