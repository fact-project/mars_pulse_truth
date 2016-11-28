#ifndef MARS_MCalibrationBlindCam
#define MARS_MCalibrationBlindCam

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif

class MCalibrationBlindCam : public MCalibrationCam
{
private:

  void Add(const UInt_t a, const UInt_t b);

public:
  MCalibrationBlindCam(Int_t nblind=1,const char *name=NULL, const char *title=NULL);
  
  // Inits
  void  Init ( const MGeomCam &geom ) {}

  // Getter
  Bool_t IsFluxInsidePlexiglassAvailable () const;

  Float_t GetFluxInsidePlexiglass    () const;
  Float_t GetFluxInsidePlexiglassVar () const;
  Float_t GetFluxInsidePlexiglassRelVar() const;

  // Setter
  void  SetPulserColor( const MCalibrationCam::PulserColor_t col );
  
  ClassDef(MCalibrationBlindCam, 2) // Container Blind Pixels Calibration Results Camera
};

#endif
