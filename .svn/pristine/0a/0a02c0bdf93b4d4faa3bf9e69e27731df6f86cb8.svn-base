#ifndef MARS_MCalibrationPedCam
#define MARS_MCalibrationPedCam

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif

class MCalibrationPedCam : public MCalibrationCam
{
public:

  MCalibrationPedCam(const char *name=NULL, const char *title=NULL);
  ~MCalibrationPedCam() {}

  Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;
  
  ClassDef(MCalibrationPedCam, 1)	// Container Pedestal Calibration Results Camera
};

#endif
