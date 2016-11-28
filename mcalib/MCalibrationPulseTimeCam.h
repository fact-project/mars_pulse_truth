#ifndef MARS_MCalibrationPulseTimeCam
#define MARS_MCalibrationPulseTimeCam

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif

class MCalibrationPulseTimeCam : public MCalibrationCam
{
public:

  MCalibrationPulseTimeCam(const char *name=NULL, const char *title=NULL);

  // Prints
  void Print(Option_t *o="") const;
  
  // Others
  Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;

  ClassDef(MCalibrationPulseTimeCam, 2)	// Container Pulse Time Results Camera
};

#endif
