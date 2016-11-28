#ifndef MARS_MHCalibrationChargePINDiode
#define MARS_MHCalibrationChargePINDiode


#ifndef MARS_MHCalibrationChargePix
#include "MHCalibrationChargePix.h"
#endif

class TH1F;
class MExtractedSignalPINDiode;
class MCalibrationChargePINDiode;
class MHCalibrationChargePINDiode : public MHCalibrationChargePix
{
private:

  static const Axis_t  fgAbsTimeFirst;    //! Default for fAbsTimeFirst    (now set to: -0.5  )
  static const Axis_t  fgAbsTimeLast;     //! Default for fAbsTimeLast     (now set to: 29.5  )
  static const Int_t   fgAbsTimeNbins;    //! Default for fAbsTimeNBins    (now set to: 30    )
  static const Int_t   fgChargeNbins;     //! Default for fNBins           (now set to: 200   )
  static const Axis_t  fgChargeFirst;     //! Default for fFirst           (now set to: -0.5  )
  static const Axis_t  fgChargeLast;      //! Default for fLast            (now set to: 199.5 )
  static const Int_t   fgRmsChargeNbins;  //! Default for fRmsChargeNBins  (now set to: 100   )
  static const Axis_t  fgRmsChargeFirst;  //! Default for fRmsChargeFirst  (now set to: 0.    )
  static const Axis_t  fgRmsChargeLast;   //! Default for fRmsChargeLast   (now set to: 100.  )
  static const Float_t fgTimeLowerLimit;  //! Default for fTimeLowerLimit    (now set to: 1.) 
  static const Float_t fgTimeUpperLimit;  //! Default for fTimeUpperLimit    (now set to: 2.)

  static const TString gsHistName;        //! Default Histogram names
  static const TString gsHistTitle;       //! Default Histogram titles
  static const TString gsHistXTitle;      //! Default Histogram x-axis titles
  static const TString gsHistYTitle;      //! Default Histogram y-axis titles
  
  static const TString gsAbsHistName;     //! Default Histogram names abs.times
  static const TString gsAbsHistTitle;    //! Default Histogram titles abs.times
  static const TString gsAbsHistXTitle;   //! Default Histogram x-axis titles abs.times
  static const TString gsAbsHistYTitle;   //! Default Histogram y-axis titles abs.times
  
  TString fHistName;                      // Histogram names
  TString fHistTitle;                     // Histogram titles
  TString fHistXTitle;                    // Histogram x-axis titles
  TString fHistYTitle;                    // Histogram y-axis titles
  
  TString fAbsHistName;                   // Histogram names abs.times
  TString fAbsHistTitle;                  // Histogram titles abs. times
  TString fAbsHistXTitle;                 // Histogram x-axis titles abs. times
  TString fAbsHistYTitle;                 // Histogram y-axis titles abs. times
  
  MCalibrationChargePINDiode *fPINDiode;  //! Storage container of the results
  MExtractedSignalPINDiode   *fSigPIN;    //! Storage container of extracted signal

  TH1F fHRmsCharge;                       //  Histogram containing Variance of summed FADC slices

  Axis_t  fRmsChargeFirst;                //  Lower bound bin used for the fHRmsCharge
  Axis_t  fRmsChargeLast;                 //  Upper bound bin used for the fHRmsCharge
  Int_t   fRmsChargeNbins;                //  Number of  bins used for the fHRmsCharge
  Float_t fRmsChargeMean;                 //  Mean of the Gauss fit               
  Float_t fRmsChargeMeanErr;              //  Error of the mean of the Gauss fit 
  Float_t fRmsChargeSigma;                //  Sigma of the Gauss fit             
  Float_t fRmsChargeSigmaErr;             //  Error of the sigma of the Gauss fit
  Float_t fTimeLowerLimit;                //  Limit dist. to first signal slice (units: FADC slices) 
  Float_t fTimeUpperLimit;                //  Limit dist. to last signal slice  (units: FADC slices) 

  Int_t   fExclusionMean;                 //  Number of events excluded by deviating mean
  Int_t   fExclusionSigma;                //  Number of events excluded by deviating sigma
  Int_t   fExclusionChi2;                 //  Number of events excluded by deviating chi2
  
public:

  MHCalibrationChargePINDiode(const char *name=NULL, const char *title=NULL);
  ~MHCalibrationChargePINDiode(){}

  Bool_t SetupFill(const MParList *pList);
  Bool_t ReInit   (      MParList *pList);
  Int_t  Fill     (const MParContainer *par, const Stat_t w=1);
  Bool_t Finalize ();
  
  // Draw
  void Draw(Option_t *opt="");
  
  // Getters
        TH1F *GetHRmsCharge()                  { return &fHRmsCharge;       }
  const TH1F *GetHRmsCharge()            const { return &fHRmsCharge;       }
  Float_t     GetRmsChargeMean()      const { return fRmsChargeMean;     }
  Float_t     GetRmsChargeMeanErr()   const { return fRmsChargeMeanErr;  }
  Float_t     GetRmsChargeSigma()     const { return fRmsChargeSigma;    }
  Float_t     GetRmsChargeSigmaErr()  const { return fRmsChargeSigmaErr; }

  Int_t       GetExclusionMean()      const { return fExclusionMean;     }
  Int_t       GetExclusionSigma()     const { return fExclusionSigma;    }
  Int_t       GetExclusionChi2()      const { return fExclusionChi2;     }
  
  
  // Fill histos
  Bool_t FillRmsCharge(const Float_t q);

  // Fits
  Bool_t FitRmsCharge(Option_t *option="RQ0");

  // Setters
  void SetBinningRmsCharge(Int_t n, Axis_t lo, Axis_t up) { fRmsChargeNbins=n; fRmsChargeFirst=lo; fRmsChargeLast=up; }

  void SetTimeLowerLimit ( const Float_t f=fgTimeLowerLimit     ) { fTimeLowerLimit = f;     }
  void SetTimeUpperLimit ( const Float_t f=fgTimeUpperLimit     ) { fTimeUpperLimit = f;     }

  void SetAbsHistName  ( const char *name )  { fAbsHistName  = name;  }
  void SetAbsHistTitle ( const char *name )  { fAbsHistTitle = name;  }
  void SetAbsHistXTitle( const char *name )  { fAbsHistXTitle = name; }
  void SetAbsHistYTitle( const char *name )  { fAbsHistYTitle = name; }
   
  void SetHistName  ( const char *name )  { fHistName  = name;  }
  void SetHistTitle ( const char *name )  { fHistTitle = name;  }
  void SetHistXTitle( const char *name )  { fHistXTitle = name; }
  void SetHistYTitle( const char *name )  { fHistYTitle = name; }
   
  ClassDef(MHCalibrationChargePINDiode, 2)  // Histogram class for Charge PIN Diode Calibration 
};

#endif

