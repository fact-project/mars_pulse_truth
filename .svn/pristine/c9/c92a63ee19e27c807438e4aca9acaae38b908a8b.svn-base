#ifndef MARS_MCalibrationChargePix
#define MARS_MCalibrationChargePix

#ifndef MARS_MCalibrationPix
#include "MCalibrationPix.h"
#endif

class MCalibrationChargePix : public MCalibrationPix
{
private:

  static const Float_t gkElectronicPedRms;     //! Electronic component of ped. RMS from opt. receiver on (now set to: 1.)
  static const Float_t gkElectronicPedRmsErr;  //! Error Electr. component ped. RMS (now set to: 0.35)
  static const Float_t gkFFactor;              //! Laboratory F-factor PMTs         (now set to: 1.15)
  static const Float_t gkFFactorErr;           //! Laboratory F-factor Error PMTs   (now set to: 0.02)

  static const Float_t fgConversionHiLo;         //! Default fConversionHiLo          (now set to: 10.)
  static const Float_t fgConversionHiLoErr;      //! Default fConversionHiLoVar       (now set to: 2.5)
  static const Float_t fgConversionHiLoSigma;    //! Default fConversionHiLoSigma     (now set to: 2.5)
  static const Float_t fgPheFFactorMethodLimit;  //! Default fPheFFactorMethodLimit   (now set to: 5.)
  static const Float_t fgConvFFactorRelErrLimit; //! Default fConvFFactorRelErrLimit  (now set to: 0.35)  
  
  Float_t fAbsTimeMean;                     // Mean Absolute Arrival Time
  Float_t fAbsTimeRms;                      // RMS Mean Absolute Arrival Time
  Byte_t  fCalibFlags;                      // Bit-field for the class-own bits
  Float_t fConversionHiLo;                  // Conversion factor betw. Hi Gain and Lo Gain  
  Float_t fConversionHiLoVar;               // Variance Conversion factor betw. Hi and Lo Gain
  Float_t fConversionHiLoSigma;             // Sigma of conversion factor betw. Hi and Lo Gain
  Float_t fConvFFactorRelVarLimit;          // Limit for acceptance rel. variance Conversion FADC2Phe
  Float_t fLoGainPedRmsSquare;              // Pedestal RMS square of Low Gain
  Float_t fLoGainPedRmsSquareVar;           // Pedestal RMS square Variance of Low Gain
  Float_t fMeanConvFADC2Phe;                // Conversion factor (F-factor method)
  Float_t fMeanConvFADC2PheStatVar;         // Variance conversion factor, only stat. error
  Float_t fMeanConvFADC2PheSystVar;         // Variance conversion factor, only syst. error
  Float_t fMeanFFactorFADC2Phot;            // Total mean F-Factor to photons (F-factor method)
  Float_t fMeanFFactorFADC2PhotVar;         // Variance mean F-Factor photons, only stat. error
  Float_t fPed;                             // Pedestal (from MPedestalPix) times number FADC slices
  Float_t fPedVar;                          // Variance of pedestal 
  Float_t fPedRms;                          // Pedestal RMS (from MPedestalPix) times sqrt nr. FADC slices
  Float_t fPedRmsVar;                       // Pedestal RMS (from MPedestalPix) times sqrt nr. FADC slices
  Float_t fPheFFactorMethod;                // Number Phe's calculated  with F-factor method)
  Float_t fPheFFactorMethodStatVar;         // Variance number of Phe's, only stat. error
  Float_t fPheFFactorMethodSystVar;         // Variance number of Phe's, only syst. error
  Float_t fPheFFactorMethodLimit;           // Min. number Photo-electrons for pix to be accepted.
  Float_t fRSigmaSquare;                    // Square of Reduced sigma
  Float_t fRSigmaSquareVar;                 // Variance Reduced sigma

  Int_t   fNumSaturated;                    // Number of (high-gain) saturated events
  
  enum  { kFFactorMethodValid   };          // Possible bits to be set

  const Float_t GetElectronicPedRmsRelVar() const;
  const Float_t GetConversionHiLoRelVar()  const;
  const Float_t GetFFactorRelVar()     const;
  
public:
  MCalibrationChargePix(const char *name=NULL, const char *title=NULL);

  // TObject
  void Clear(Option_t *o="");
  void Copy(TObject& object) const;

  // Setter
  /*
  void SetConvFFactorRelErrLimit   ( const Float_t f=fgConvFFactorRelErrLimit) { fConvFFactorRelVarLimit = f*f;}
  void SetMeanConvFADC2Phe      ( const Float_t f)                          { fMeanConvFADC2Phe       = f; }
  void SetMeanConvFADC2PheVar   ( const Float_t f)                          { fMeanConvFADC2PheVar    = f; }
  void SetMeanFFactorFADC2Phot  ( const Float_t f)                          { fMeanFFactorFADC2Phot   = f; }
  void SetPheFFactorMethod      ( const Float_t f)                          { fPheFFactorMethod       = f; }
  void SetPheFFactorMethodVar   ( const Float_t f)                          { fPheFFactorMethodVar    = f; }  
  void SetPheFFactorMethodLimit ( const Float_t f=fgPheFFactorMethodLimit ) { fPheFFactorMethodLimit  = f; }
  void SetNumSaturated          ( const Int_t   i)                          { fNumSaturated           = i; }
  */
  void SetFFactorMethodValid     (const Bool_t b = kTRUE );
  void SetPedestal               (const Float_t ped, const Float_t pedrms, const Float_t pederr);
  void SetPed                   ( const Float_t ped, const Float_t pederr);  
  void SetPedRMS              ( const Float_t pedrms, const Float_t pedrmserr);  

  void SetAbsTimeMean            (const Float_t f)                          { fAbsTimeMean            = f; }
  void SetAbsTimeRms             (const Float_t f)                          { fAbsTimeRms             = f; }
  void SetConversionHiLo         (const Float_t c=fgConversionHiLo        ) { fConversionHiLo         = c; }
  void SetConversionHiLoErr      (const Float_t e=fgConversionHiLoErr     ) { fConversionHiLoVar      = e*e;}
  void SetConversionHiLoSigma    (const Float_t s=fgConversionHiLoSigma   ) { fConversionHiLoSigma    = s; }
  void SetConvFFactorRelErrLimit (const Float_t f=fgConvFFactorRelErrLimit) { fConvFFactorRelVarLimit = f*f;}
  void SetMeanConvFADC2Phe       (const Float_t f)                          { fMeanConvFADC2Phe       = f; }
  void SetMeanConvFADC2PheVar    (const Float_t f)                          { fMeanConvFADC2PheStatVar= f; }
  void SetMeanConvFADC2PheSystVar(const Float_t f)                          { fMeanConvFADC2PheSystVar= f; }
  void SetMeanFFactorFADC2Phot   (const Float_t f)                          { fMeanFFactorFADC2Phot   = f; }
  void SetNumSaturated           (const Int_t   i)                          { fNumSaturated           = i; }
  void SetPheFFactorMethod       (const Float_t f)                          { fPheFFactorMethod       = f; }
  void SetPheFFactorMethodVar    (const Float_t f)                          { fPheFFactorMethodStatVar= f; }
  void SetPheFFactorMethodSystVar(const Float_t f)                          { fPheFFactorMethodSystVar= f; }
  void SetPheFFactorMethodLimit  (const Float_t f=fgPheFFactorMethodLimit ) { fPheFFactorMethodLimit  = f; }
  
  // Getters
  Float_t GetAbsTimeMean        () const { return fAbsTimeMean;    }
  Float_t GetAbsTimeRms         () const { return fAbsTimeRms;     }

  Float_t GetConversionHiLo     () const { return fConversionHiLo; }
  Float_t GetConversionHiLoErr  () const;
  Float_t GetConversionHiLoSigma() const { return fConversionHiLoSigma; }

  Float_t GetConvertedMean      () const { return IsHiGainSaturation() ? fLoGainMean * fConversionHiLo : fHiGainMean; }
  Float_t GetConvertedMeanErr   () const;

  Float_t GetConvertedSigma() const { return IsHiGainSaturation() ? fLoGainSigma * fConversionHiLo : fHiGainSigma; }
  Float_t GetConvertedSigmaErr() const;

  Float_t GetConvertedRSigma() const;
  Float_t GetConvertedRSigmaErr() const;
  Float_t GetConvertedRSigmaSquare() const
  {
      if (fRSigmaSquare < 0)
          return -1;
      return IsHiGainSaturation() ? fRSigmaSquare*fConversionHiLo*fConversionHiLo : fRSigmaSquare ;
  }

  Float_t GetMeanConvFADC2Phe() const { return fMeanConvFADC2Phe; }
  Float_t GetMeanConvFADC2PheErr() const;
  Float_t GetMeanConvFADC2PheSystErr() const;
  Float_t GetMeanConvFADC2PheTotErr() const;

  Float_t GetFFactorFADC2Phe        () const { return gkFFactor;   }
  Float_t GetMeanConvFADC2PheVar    () const { return fMeanConvFADC2PheStatVar; }
  Float_t GetMeanConvFADC2PheSystVar() const { return fMeanConvFADC2PheSystVar; }

  Float_t GetMeanFFactorFADC2Phot   () const { return fMeanFFactorFADC2Phot;    }
  Float_t GetMeanFFactorFADC2PhotErr() const;
  Float_t GetMeanFFactorFADC2PhotVar() const { return fMeanFFactorFADC2PhotVar; }

  Int_t   GetNumSaturated           () const { return fNumSaturated;            }

  Float_t GetPed() const { return fPed; }
  Float_t GetPedErr() const;
  Float_t GetPedRms() const;
  Float_t GetPedRmsErr() const;

  Float_t GetPheFFactorMethod() const { return fPheFFactorMethod; }
  Float_t GetPheFFactorMethodErr() const;
  Float_t GetPheFFactorMethodSystErr() const;
  Float_t GetPheFFactorMethodTotErr() const;

  Float_t GetPheFFactorMethodVar    () const { return fPheFFactorMethodStatVar; }
  Float_t GetPheFFactorMethodSystVar() const { return fPheFFactorMethodSystVar; }
  Float_t GetPheFFactorMethodRelVar () const { return fPheFFactorMethodStatVar<=0 ? -1 : fPheFFactorMethodStatVar / (fPheFFactorMethod * fPheFFactorMethod); }
  Float_t GetPheFFactorMethodRelSystVar() const { return fPheFFactorMethodSystVar<=0 ? -1. : fPheFFactorMethodSystVar / (fPheFFactorMethod * fPheFFactorMethod); }

  Float_t GetRSigma() const;
  Float_t GetRSigmaErr() const;
  Float_t GetRSigmaRelVar() const;
  Float_t GetRSigmaPerCharge() const;
  Float_t GetRSigmaPerChargeErr() const;

  Bool_t IsFFactorMethodValid() const;

  // Calculations
  void   CalcLoGainPedestal(const Float_t logainsamples);
  Bool_t CalcReducedSigma(const Float_t extractorres=0);
  Bool_t CalcFFactor();
  Bool_t CalcConvFFactor();
  Bool_t CalcMeanFFactor(const Float_t nphotons, const Float_t nphotonsrelvar);
  
  ClassDef(MCalibrationChargePix, 4)	// Container Charge Calibration Results Pixel
};

#endif

