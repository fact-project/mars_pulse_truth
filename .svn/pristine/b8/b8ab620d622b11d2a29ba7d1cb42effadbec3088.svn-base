#ifndef MARS_MCalibrationChargeCam
#define MARS_MCalibrationChargeCam

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif

class MCalibrationQECam;
class MCalibrationChargeCam : public MCalibrationCam
{
private:
  
  Byte_t  fFlags;                         // Bit-field to hold the flags

  Float_t fNumPhotonsBlindPixelMethod;    // Average nr. photons from Blind Pixel Method (Inner Pixel)
  Float_t fNumPhotonsFFactorMethod;       // Average nr. photons from F-Factor Method (Inner Pixel)
  Float_t fNumPhotonsPINDiodeMethod;      // Average nr. photons from PIN Diode Method (Inner Pixel)
  Float_t fNumPhotonsBlindPixelMethodErr; // Error av. nr. photons from Blind Pixel Method
  Float_t fNumPhotonsFFactorMethodErr;    // Error av. nr. photons from F-Factor Method
  Float_t fNumPhotonsPINDiodeMethodErr;   // Error av. nr. photons from PIN Diode Method
  
  enum  { kFFactorMethodValid };

  void Add(const UInt_t a, const UInt_t b);
  void AddArea(const UInt_t a, const UInt_t b);
  void AddSector(const UInt_t a, const UInt_t b);
  
public:

  MCalibrationChargeCam(const char *name=NULL, const char *title=NULL);
  
  void   Clear ( Option_t *o="" );
  
  // Getters
  Bool_t  GetConversionFactorFFactor( Int_t ipx, Float_t &mean, Float_t &err, Float_t &ffactor );
  Bool_t  GetPixelContent           ( Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;

  Float_t GetNumPhotonsBlindPixelMethod   () const { return fNumPhotonsBlindPixelMethod;    }
  Float_t GetNumPhotonsFFactorMethod      () const { return fNumPhotonsFFactorMethod;       }     
  Float_t GetNumPhotonsPINDiodeMethod     () const { return fNumPhotonsPINDiodeMethod;      }    
  Float_t GetNumPhotonsBlindPixelMethodErr() const { return fNumPhotonsBlindPixelMethodErr; }
  Float_t GetNumPhotonsFFactorMethodErr   () const { return fNumPhotonsFFactorMethodErr;    }     
  Float_t GetNumPhotonsPINDiodeMethodErr  () const { return fNumPhotonsPINDiodeMethodErr;   }    
  Bool_t  IsFFactorMethodValid            () const;

  TArrayF GetAveragedConvFADC2PhotPerArea    (const MGeomCam &geom, const MCalibrationQECam &qecam,
                                              const UInt_t ai=0,  MBadPixelsCam *bad=NULL);
  TArrayF GetAveragedConvFADC2PhePerArea     (const MGeomCam &geom, const MCalibrationQECam &qecam,
                                              const UInt_t ai=0,  MBadPixelsCam *bad=NULL);
  TArrayF GetAveragedConvFADC2PhotPerSector  (const MGeomCam &geom, const MCalibrationQECam &qecam,
                                              const UInt_t sec=0, MBadPixelsCam *bad=NULL);
  TArrayF GetAveragedArrivalTimeMeanPerArea  (const MGeomCam &geom,
                                              const UInt_t ai=0,  MBadPixelsCam *bad=NULL);
  TArrayF GetAveragedArrivalTimeMeanPerSector(const MGeomCam &geom,
                                              const UInt_t sec=0, MBadPixelsCam *bad=NULL);
  TArrayF GetAveragedArrivalTimeRmsPerArea   (const MGeomCam &geom,
                                              const UInt_t ai=0,  MBadPixelsCam *bad=NULL);
  TArrayF GetAveragedArrivalTimeRmsPerSector (const MGeomCam &geom,
                                              const UInt_t sec=0, MBadPixelsCam *bad=NULL);

  Bool_t MergeHiLoConversionFactors(const MCalibrationChargeCam &cam) const;
  
  // Prints
  void   Print(Option_t *o="") const;

  // Setters   
  void  SetFFactorMethodValid           ( const Bool_t  b=kTRUE );
  void  SetNumPhotonsBlindPixelMethod   ( const Float_t f )  { fNumPhotonsBlindPixelMethod    = f; } 
  void  SetNumPhotonsFFactorMethod      ( const Float_t f )  { fNumPhotonsFFactorMethod       = f; }      
  void  SetNumPhotonsPINDiodeMethod     ( const Float_t f )  { fNumPhotonsPINDiodeMethod      = f; }   
  void  SetNumPhotonsBlindPixelMethodErr( const Float_t f )  { fNumPhotonsBlindPixelMethodErr = f; } 
  void  SetNumPhotonsFFactorMethodErr   ( const Float_t f )  { fNumPhotonsFFactorMethodErr    = f; }      
  void  SetNumPhotonsPINDiodeMethodErr  ( const Float_t f )  { fNumPhotonsPINDiodeMethodErr   = f; }   
  
  ClassDef(MCalibrationChargeCam, 5) // Container Charge Calibration Results Camera
};

#endif
