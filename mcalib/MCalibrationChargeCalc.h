#ifndef MARS_MCalibrationChargeCalc
#define MARS_MCalibrationChargeCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef ROOT_TArrayC
#include <TArrayC.h>
#endif

#ifndef MARS_MBadPixelsPix
#include "MBadPixelsPix.h"
#endif

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif

class MCalibrationPattern;
class MPedestalCam;
class MPedestalPix;
class MCalibrationChargePINDiode;
class MHCalibrationChargeBlindCam;
class MCalibrationBlindCam;
class MCalibrationChargePix;
class MCalibrationChargeCam;
class MHCalibrationChargeCam;
class MCalibrationQECam;
class MGeomCam;
class MExtractedSignalCam;
class MBadPixelsCam;
class MExtractor;

class MCalibrationChargeCalc : public MTask
{
private:

  static const Float_t fgChargeLimit;          //! Default for fChargeLimit        (now set to: 2.5)
  static const Float_t fgChargeErrLimit;       //! Default for fChargeErrLimit     (now set to: 0.)
  static const Float_t fgChargeRelErrLimit;    //! Default for fChargeRelErrLimit  (now set to: 1.)
  static const Float_t fgLambdaCheckLimit;     //! Default for fLambdaCheckLimit   (now set to: 0.2)
  static const Float_t fgLambdaErrLimit;       //! Default for fLabmdaErrLimit     (now set to: 0.5)
  static const Float_t fgPheErrLowerLimit;     //! Default for fPheErrLowerLimit   (now set to: 9.0)
  static const Float_t fgPheErrUpperLimit;     //! Default for fPheErrUpperLimit   (now set to: 4.5)
  static const Float_t fgFFactorErrLimit;      //! Default for fFFactorErrLimit    (now set to: 4.5)
  static const Float_t fgArrTimeRmsLimit;      //! Default for fArrTimeRmsLimit    (now set to: 3.5)
  static const Float_t fgUnsuitablesLimit;     //! Default for fUnsuitableLimit    (now set to: 0.1)
  static const Float_t fgUnreliablesLimit;     //! Default for fUnreliableLimit    (now set to: 0.3)
 
  static const char *fgNamePedestalCam;      //! "MPedestalCam"
  
  // Variables
  Float_t fArrTimeRmsLimit;                    // Limit acceptance RMS of absolute arrival times
  Float_t fChargeLimit;                        // Limit acceptance mean charge  (in units of PedRMS)
  Float_t fChargeErrLimit;                     // Limit acceptance charge error (in abs. numbers)
  Float_t fChargeRelErrLimit;                  // Limit acceptance rel. error mean (in abs. numbers)
  Float_t fFFactorErrLimit;                    // Limit acceptance F-Factor w.r.t. area idx mean
  Float_t fLambdaCheckLimit;                   // Limit rel. diff. lambda and lambdacheck in Blind Pixel
  Float_t fLambdaErrLimit;                     // Limit acceptance lambda error in Blind Pixel
  Float_t fNumHiGainSamples;                   // Number High-Gain FADC slices used by extractor
  Float_t fNumLoGainSamples;                   // Number Low -Gain FADC slices used by extractor
  Float_t fPheErrLowerLimit;                   // Lower limit acceptance nr. phe's w.r.t. area idx mean (in sigmas)
  Float_t fPheErrUpperLimit;                   // Upper limit acceptance nr. phe's w.r.t. area idx mean (in sigmas)
  Float_t fSqrtHiGainSamples;                  // Square root nr. High-Gain FADC slices used by extractor
  Float_t fSqrtLoGainSamples;                  // Square root nr. Low -Gain FADC slices used by extractor 
  Float_t fUnsuitablesLimit;                   // Limit for relative number of unsuitable pixels
  Float_t fUnreliablesLimit;                   // Limit for relative number of unreliable pixels

  Float_t fExternalNumPhes;                    // External mean number of photo-electrons set from outside
  Float_t fExternalNumPhesRelVar;              // External rel. var. number of photo-electrons set from outside  

  MCalibrationCam::PulserColor_t fPulserColor; // Calibration LEDs colour 
  Float_t fStrength;                           // Calibration LEDs strength 

  Int_t   fNumInnerFFactorMethodUsed;          // Number of inner pixels used for F-Factor Method calibration

  TString fNamePedestalCam;                    // Name of the 'MPedestalCam' container

  Int_t   fNumProcessed;                       // Number of processed events (for Intensity calibration)

  Bool_t fContinousCalibration;

  // Pointers
  MBadPixelsCam                  *fBadPixels;      //!  Bad Pixels
  MCalibrationChargeCam          *fCam;            //!  Calibrated Charges results of all pixels
  MHCalibrationChargeCam         *fHCam;           //!  Charges histograms of all pixels   
  MCalibrationBlindCam           *fBlindCam;       //!  Calibrated Charges of the Blind Pixels
  MHCalibrationChargeBlindCam    *fHBlindCam;      //!  Charges histograms of the Blind Pixels  
  MCalibrationChargePINDiode     *fPINDiode;       //!  Calibrated Charges of the PIN Diode
  MCalibrationQECam              *fQECam;          //!  Calibrated Quantum Efficiencies of all pixels 
  MGeomCam                       *fGeom;           //!  Camera geometry
  MExtractedSignalCam            *fSignal;         //! Extracted Signal
  MCalibrationPattern            *fCalibPattern;   //! Calibration DM pattern
  MPedestalCam                   *fPedestals;      //! Pedestals all pixels (calculated previously from ped.file)
  MExtractor                     *fExtractor;      //! Signal Extractor

  // enums
  enum  Check_t
    {
      kCheckDeadPixels,
      kCheckExtractionWindow,
      kCheckHistOverflow,
      kCheckDeviatingBehavior,
      kCheckOscillations,
      kCheckArrivalTimes
    };                                         // Possible Checks

  Byte_t fCheckFlags;                          // Bit-field to hold the possible check flags

  enum  FitResult_t { kPheFitOK,
                      kFFactorFitOK,
                      kBlindPixelFitOK,
                      kBlindPixelPedFitOK,
                      kPINDiodeFitOK };        // Possible Fit Result flags

  TArrayC fResultFlags;                        // Bit-fields for the fitting results   (one field per area index)
  TArrayC fBlindPixelFlags;                    // Bit-fields for the blind pixel flags (one field per blind pixel)
  TArrayC fPINDiodeFlags;                      // Bit-fields for the PIN Diode flags   (one field per PIN Diode  )

  enum  { kDebug,
          kUseExtractorRes,
          kUseUnreliables,
          kUseExternalNumPhes };               // Possible general flags

  Byte_t fFlags;                               // Bit-field to hold the general flags

  // functions
  void   FinalizeBadPixels       ();
  Bool_t FinalizeBlindCam        ();  
  void   FinalizeBlindPixelQECam ();
  Bool_t FinalizeCharges         ( MCalibrationChargePix &cal, MBadPixelsPix &bad, const char* what);
  void   FinalizeCombinedQECam   ();
  void   FinalizeFFactorQECam    ();  
  Bool_t FinalizeFFactorMethod   ();
  void   FinalizePedestals       ( const MPedestalPix    &ped, MCalibrationChargePix &cal, const Int_t aidx );
  Bool_t FinalizePINDiode        ();
  void   FinalizePINDiodeQECam   ();
  Bool_t FinalizeUnsuitablePixels();

  void FinalizeAbsTimes();

  const char* GetOutputFile();

  // Query checks
  Bool_t IsCheckDeadPixels       () const { return TESTBIT(fCheckFlags,kCheckDeadPixels);        }
  Bool_t IsCheckDeviatingBehavior() const { return TESTBIT(fCheckFlags,kCheckDeviatingBehavior); }
  Bool_t IsCheckExtractionWindow () const { return TESTBIT(fCheckFlags,kCheckExtractionWindow);  }
  Bool_t IsCheckHistOverflow     () const { return TESTBIT(fCheckFlags,kCheckHistOverflow);      }
  Bool_t IsCheckOscillations     () const { return TESTBIT(fCheckFlags,kCheckOscillations);      }
  Bool_t IsCheckArrivalTimes     () const { return TESTBIT(fCheckFlags,kCheckArrivalTimes);      }

  Bool_t IsDebug                 () const { return TESTBIT(fFlags,kDebug);                       }
  Bool_t IsUseExtractorRes       () const { return TESTBIT(fFlags,kUseExtractorRes);             }
  Bool_t IsUseUnreliables        () const { return TESTBIT(fFlags,kUseUnreliables);              }
  Bool_t IsUseExternalNumPhes    () const { return TESTBIT(fFlags,kUseExternalNumPhes);          }

  void   PrintUncalibrated( MBadPixelsPix::UncalibratedType_t typ, const char *text) const;

  // Global fit results
  void   SetPheFitOK          (const Int_t aidx, const Bool_t b=kTRUE) { b ? SETBIT(fResultFlags[aidx], kPheFitOK) : CLRBIT(fResultFlags[aidx], kPheFitOK);      }
  void   SetFFactorFitOK      (const Int_t aidx, const Bool_t b=kTRUE) { b ? SETBIT(fResultFlags[aidx], kFFactorFitOK) : CLRBIT(fResultFlags[aidx], kFFactorFitOK);  }
  void   SetBlindPixelFitOK   (const Int_t  idx, const Bool_t b=kTRUE) { b ? SETBIT(fBlindPixelFlags[idx], kBlindPixelFitOK) : CLRBIT(fBlindPixelFlags[idx], kBlindPixelFitOK); }
  void   SetBlindPixelPedFitOK(const Int_t  idx, const Bool_t b=kTRUE) { b ? SETBIT(fBlindPixelFlags[idx], kBlindPixelPedFitOK): CLRBIT(fBlindPixelFlags[idx], kBlindPixelPedFitOK); }
  void   SetPINDiodeFitOK     (const Int_t  idx, const Bool_t b=kTRUE) { b ? SETBIT(fPINDiodeFlags[idx], kPINDiodeFitOK): CLRBIT(fPINDiodeFlags[idx], kPINDiodeFitOK); }

  Int_t  PreProcess (MParList *pList);
  Bool_t ReInit     (MParList *pList); 
  Int_t  Process    ();
  Int_t  PostProcess();
  Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
  MCalibrationChargeCalc(const char *name=NULL, const char *title=NULL);

  void Clear(const Option_t *o="");

  void ResetNumProcessed() { fNumProcessed=0; }

  Int_t Finalize();

  void SetChargeLimit      (const Float_t f=fgChargeLimit      ) { fChargeLimit       = f; }
  void SetChargeErrLimit   (const Float_t f=fgChargeErrLimit   ) { fChargeErrLimit    = f; }
  void SetChargeRelErrLimit(const Float_t f=fgChargeRelErrLimit) { fChargeRelErrLimit = f; }

  // Checks
  void SetCheckArrivalTimes(const Bool_t b=kTRUE)      { b ? SETBIT(fCheckFlags,kCheckArrivalTimes)      : CLRBIT(fCheckFlags,kCheckArrivalTimes); }
  void SetCheckDeadPixels(const Bool_t b=kTRUE)        { b ? SETBIT(fCheckFlags,kCheckDeadPixels)        : CLRBIT(fCheckFlags,kCheckDeadPixels); }
  void SetCheckDeviatingBehavior(const Bool_t b=kTRUE) { b ? SETBIT(fCheckFlags,kCheckDeviatingBehavior) : CLRBIT(fCheckFlags,kCheckDeviatingBehavior); }
  void SetCheckExtractionWindow(const Bool_t b=kTRUE)  { b ? SETBIT(fCheckFlags,kCheckExtractionWindow)  : CLRBIT(fCheckFlags,kCheckExtractionWindow); }
  void SetCheckHistOverflow(const Bool_t b=kTRUE)      { b ? SETBIT(fCheckFlags,kCheckHistOverflow)      : CLRBIT(fCheckFlags,kCheckHistOverflow); }
  void SetCheckOscillations(const Bool_t b=kTRUE)      { b ? SETBIT(fCheckFlags,kCheckOscillations)      : CLRBIT(fCheckFlags,kCheckOscillations); }
  void SetDebug(const Bool_t b=kTRUE)                  { b ? SETBIT(fFlags, kDebug)                      : CLRBIT(fFlags, kDebug); }
  void SetUseExtractorRes(const Bool_t b=kTRUE)        { b ? SETBIT(fFlags, kUseExtractorRes)            : CLRBIT(fFlags, kUseExtractorRes); }
  void SetUseUnreliables(const Bool_t b=kTRUE)         { b ? SETBIT(fFlags, kUseUnreliables)             : CLRBIT(fFlags, kUseUnreliables); }
  void SetUseExternalNumPhes(const Bool_t b=kTRUE)     { b ? SETBIT(fFlags, kUseExternalNumPhes)         : CLRBIT(fFlags, kUseExternalNumPhes); }

  void SetContinousCalibration(const Bool_t b=kTRUE)   { fContinousCalibration = b; }

  // pointers
  void SetPedestals(MPedestalCam *cam) { fPedestals=cam; }
  void SetExtractor(MExtractor   *ext) { fExtractor=ext; }

  // limits
  void SetArrTimeRmsLimit (const Float_t f=fgArrTimeRmsLimit ) { fArrTimeRmsLimit   = f; }
  void SetFFactorErrLimit (const Float_t f=fgFFactorErrLimit ) { fFFactorErrLimit   = f; }
  void SetLambdaErrLimit  (const Float_t f=fgLambdaErrLimit  ) { fLambdaErrLimit    = f; }
  void SetLambdaCheckLimit(const Float_t f=fgLambdaCheckLimit) { fLambdaCheckLimit  = f; }
  void SetUnsuitablesLimit(const Float_t f=fgUnsuitablesLimit) { fUnsuitablesLimit  = f; }
  void SetUnreliablesLimit(const Float_t f=fgUnreliablesLimit) { fUnreliablesLimit  = f; }

  // others
  void SetExternalNumPhes      ( const Float_t f=0.                       ) { fExternalNumPhes       = f; }
  void SetExternalNumPhesRelVar( const Float_t f=0.                       ) { fExternalNumPhesRelVar = f; }  
  void SetNamePedestalCam      ( const char *name=fgNamePedestalCam       ) { fNamePedestalCam    = name; }
  void SetPheErrLowerLimit     ( const Float_t f=fgPheErrLowerLimit       ) { fPheErrLowerLimit  = f;    }
  void SetPheErrUpperLimit     ( const Float_t f=fgPheErrUpperLimit       ) { fPheErrUpperLimit  = f;    }    
  void SetPulserColor          ( const MCalibrationCam::PulserColor_t col ) { fPulserColor       = col;  }

  ClassDef(MCalibrationChargeCalc, 6)   // Task calculating Calibration Containers and Quantum Efficiencies
};

#endif
