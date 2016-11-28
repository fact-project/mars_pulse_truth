#ifndef MARS_MCalibrationQECam
#define MARS_MCalibrationQECam

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif

#ifndef ROOT_TArrayC
#include <TArrayC.h>
#endif

#ifndef MARS_MArrayD
#include "MArrayD.h"
#endif

class TGraphErrors;
class TH2D;
class MCalibrationQECam : public MCalibrationCam
{
private:

  static const Float_t gkPlexiglassQE   ;  //! Quantum Efficiency Plexiglass (now set to: 0.96)
  static const Float_t gkPlexiglassQEErr;  //! Uncertainty QE Plexiglass     (now set to: 0.01)

  TArrayC fFlags;                          //  Contains validity bits 

  enum { kBlindPixelMethodValid, kFFactorMethodValid,
         kPINDiodeMethodValid, kCombinedMethodValid }; // Possible validity bits

  void Add(const UInt_t a, const UInt_t b);
  void AddArea(const UInt_t a, const UInt_t b);
  void AddSector(const UInt_t a, const UInt_t b);

  
protected:

  MArrayD fCorningBlues;                   // Corning blues of the pixels (if available) 
  MArrayD fCorningReds;                    // Corning reds  of the pixels (if available) 

public:

  MCalibrationQECam(const char *name=NULL, const char *title=NULL);

  void Clear( Option_t *o=""  );
  void Copy ( TObject& object ) const;

  TGraphErrors *GetGraphQEvsCorningBlues() const;
  TGraphErrors *GetGraphQEvsCorningReds() const;  
  
  TH2D *GetHistQEvsCorningBlues( const Int_t nbins=50, const Axis_t first=6., const Axis_t last=17.) const;
  TH2D *GetHistQEvsCorningReds( const Int_t nbins=50, const Axis_t first=0., const Axis_t last=25.) const;  
  
  // Others
  Bool_t  GetPixelContent ( Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0 ) const;

  Float_t GetPlexiglassQE          () const { return gkPlexiglassQE; }
  Float_t GetPlexiglassQERelVar    () const; 

  const MArrayD &GetCorningBlues   () const { return fCorningBlues;  }
  const MArrayD &GetCorningReds    () const { return fCorningReds;   }
  
  Bool_t  IsBlindPixelMethodValid  () const;
  Bool_t  IsFFactorMethodValid     () const;  
  Bool_t  IsCombinedMethodValid    () const;  
  Bool_t  IsPINDiodeMethodValid    () const;  
          
  Bool_t  IsBlindPixelMethodValid  ( MCalibrationCam::PulserColor_t col ) const;
  Bool_t  IsFFactorMethodValid     ( MCalibrationCam::PulserColor_t col ) const;  
  Bool_t  IsCombinedMethodValid    ( MCalibrationCam::PulserColor_t col ) const;  
  Bool_t  IsPINDiodeMethodValid    ( MCalibrationCam::PulserColor_t col ) const;  

  // Prints
  void    Print(Option_t *o="")       const;
  
  // Setters (without color only for MC!)
  void    SetBlindPixelMethodValid ( const Bool_t  b=kTRUE );
  void    SetBlindPixelMethodValid ( const Bool_t  b, MCalibrationCam::PulserColor_t col);
  void    SetCombinedMethodValid   ( const Bool_t  b=kTRUE );
  void    SetCombinedMethodValid   ( const Bool_t  b, MCalibrationCam::PulserColor_t col);
  void    SetFFactorMethodValid    ( const Bool_t  b=kTRUE );  
  void    SetFFactorMethodValid    ( const Bool_t  b, MCalibrationCam::PulserColor_t col);  
  void    SetPINDiodeMethodValid   ( const Bool_t  b=kTRUE );  
  void    SetPINDiodeMethodValid   ( const Bool_t  b, MCalibrationCam::PulserColor_t col);  

  ClassDef(MCalibrationQECam, 2) // Container Quantum Efficieny Calibration Results Camera
};

#endif
