#ifndef MARS_MCalibrationChargePINDiode
#define MARS_MCalibrationChargePINDiode

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif

#ifndef MARS_MCalibrationPix
#include "MCalibrationPix.h"
#endif

class MCalibrationChargePINDiode : public MCalibrationPix
{
private:

  static const Float_t fgChargeToPhotons;     //! Default for fChargeToPhotons
  static const Float_t fgChargeToPhotonsErr;  //! Default for fChargeToPhotonsVar
  static const Float_t gkPINDiodeQEGreen;     //! Quantum Efficiency at 520 nm
  static const Float_t gkPINDiodeQEBlue;      //! Quantum Efficiency at 460 nm
  static const Float_t gkPINDiodeQEUV;        //! Quantum Efficiency at 370 nm
  static const Float_t gkPINDiodeQECT1;       //! Quantum Efficiency at 370 nm
  static const Float_t gkPINDiodeQEGreenErr;  //! Uncertainty QE at 520 nm
  static const Float_t gkPINDiodeQEBlueErr;   //! Uncertainty QE at 460 nm
  static const Float_t gkPINDiodeQEUVErr;     //! Uncertainty QE at 370 nm
  static const Float_t gkPINDiodeQECT1Err;    //! Uncertainty QE at 370 nmu
  static const Float_t gkAbsorptionWindow;    //! Absorption of the protection window (incl. EMS-wires)
  static const Float_t gkSolidAngleRatio;     //! Solid angles ratio PIN Diode - inner pixel
  static const Float_t gkSolidAngleRatioErr;  //! Error solid angle ratio PIN Diode - inn. pix.

  Float_t fAbsTimeMean;               // Mean Absolute Arrival Time
  Float_t fAbsTimeRms;                // RMS Mean Absolute Arrival Time
  Byte_t  fCalibFlags;                // Bit-field for the class-own bits
  Float_t fChargeLimit;               // Limit (in units of PedRMS) for acceptance fitted mean charge
  Float_t fChargeErrLimit;            // Limit (in units of PedRMS) for acceptance fitted charge sigma
  Float_t fChargeRelErrLimit;         // Limit (in units of Error of fitted charge) for acceptance fitted mean  
  Float_t fChargeToPhotons;           // Mean conv. PIN Diode charge to number of incident photons
  Float_t fChargeToPhotonsVar;        // Variance of mean conv. PIN Diode charge to nr. incident photons
  Float_t fNumPhotons;                // Number photons incidident on PIN Diode
  Float_t fNumPhotonsVar;             // Variance nr. photons incid. on PIN Diode
  Float_t fFluxOutsidePlexiglass;     // Mean number photons in INNER PIXEL outside plexiglass
  Float_t fFluxOutsidePlexiglassVar;  // Error on nr. photons in INNER PIXEL outside plexiglass
  Float_t fPed;                       // Mean pedestal (from MPedestalPix)
  Float_t fPedRms;                    // Pedestal  RMS (from MPedestalPix)
  Float_t fRmsChargeMean;             // Mean of RMS of summed FADC slices distribution 
  Float_t fRmsChargeMeanErr;          // Error on Mean RMS summed FADC slices distribution 
  Float_t fRmsChargeSigma;            // Sigma of RMS of summed FADC slices distribution    
  Float_t fRmsChargeSigmaErr;         // Error on Sigma RMS summed FADC slices distribution 
  MCalibrationCam::PulserColor_t fColor; // Colour of the pulsed LEDs

  enum  { kOscillating,
          kChargeFitValid, kTimeFitValid, 
	  kFluxOutsidePlexiglassAvailable  }; // Possible bits to be set

  const Float_t GetPINDiodeQEGreenRelVar() const; 
  const Float_t GetPINDiodeQEBlueRelVar () const; 
  const Float_t GetPINDiodeQEUVRelVar   () const; 
  const Float_t GetPINDiodeQECT1RelVar  () const;
  const Float_t GetSolidAngleRatioRelVar() const; 

 public:

  MCalibrationChargePINDiode(const char *name=NULL, const char *title=NULL);
  ~MCalibrationChargePINDiode() {}
  
  void   Clear(Option_t *o="");
  Bool_t CalcFluxOutsidePlexiglass();

  // Getters
  Float_t GetAbsTimeMean                  () const { return fAbsTimeMean;           }
  Float_t GetAbsTimeRms                   () const { return fAbsTimeRms;            }
  MCalibrationCam::PulserColor_t GetColor () const { return fColor;                 }
  Float_t GetFluxOutsidePlexiglass        () const { return fFluxOutsidePlexiglass; }
  Float_t GetFluxOutsidePlexiglassErr     () const;
  Float_t GetFluxOutsidePlexiglassRelVar  () const;  
  Float_t GetNumPhotons                   () const { return fNumPhotons;            } 
  Float_t GetNumPhotonsErr                () const;
  Float_t GetNumPhotonsRelVar             () const;
  Float_t GetPed                          () const { return fPed;                   }
  Float_t GetPedRms                       () const { return fPedRms;                }

  Bool_t  IsChargeFitValid                () const;
  Bool_t  IsTimeFitValid                  () const;
  Bool_t  IsOscillating                   () const;
  Bool_t  IsFluxOutsidePlexiglassAvailable() const;
  
  // Setters
  void SetAbsTimeMean        ( const Float_t f      )                 { fAbsTimeMean        = f;   }
  void SetAbsTimeRms         ( const Float_t f      )                 { fAbsTimeRms         = f;   }
  void SetChargeToPhotons    ( const Float_t f=fgChargeToPhotons    ) { fChargeToPhotons    = f;   }  
  void SetChargeToPhotonsErr ( const Float_t f=fgChargeToPhotonsErr ) { fChargeToPhotonsVar = f*f; }  
  void SetColor              ( const MCalibrationCam::PulserColor_t color) { fColor = color;   }
  void SetPedestal           (       Float_t ped, Float_t pedrms    );
  void SetRmsChargeMean      ( const Float_t f      )                 { fRmsChargeMean      = f;   }
  void SetRmsChargeMeanErr   ( const Float_t f      )                 { fRmsChargeMeanErr   = f;   }
  void SetRmsChargeSigma     ( const Float_t f      )                 { fRmsChargeSigma     = f;   }
  void SetRmsChargeSigmaErr  ( const Float_t f      )                 { fRmsChargeSigmaErr  = f;   }
  void SetOscillating        ( const Bool_t b=kTRUE );
  void SetChargeFitValid     ( const Bool_t b=kTRUE );
  void SetTimeFitValid       ( const Bool_t b=kTRUE );
  void SetFluxOutsidePlexiglassAvailable ( const Bool_t b = kTRUE );

  ClassDef(MCalibrationChargePINDiode, 1)	// Container Charge Calibration Results PIN Diode
};

#endif   /* MARS_MCalibrationChargePINDiode */









