#ifndef MARS_MCalibrationBlindCamOneOldStyle
#define MARS_MCalibrationBlindCamOneOldStyle

#ifndef MARS_MCalibrationBlindCam
#include "MCalibrationBlindCam.h"
#endif

class MCalibrationBlindCamOneOldStyle : public MCalibrationBlindCam
{
private:

  void CreatePixs();
  void CreateAreas();
  void CreateAtts();
  void CreateQEs();
  void CreateCollEffs();
  
public:

  MCalibrationBlindCamOneOldStyle(const char *name=NULL);
  
  ClassDef(MCalibrationBlindCamOneOldStyle, 1) // Container Blind Pixel Calibration Results - until run 31693
};

#endif
