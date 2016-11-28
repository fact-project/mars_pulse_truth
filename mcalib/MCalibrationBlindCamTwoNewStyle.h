#ifndef MARS_MCalibrationBlindCamTwoNewStyle
#define MARS_MCalibrationBlindCamTwoNewStyle

#ifndef MARS_MCalibrationBlindCam
#include "MCalibrationBlindCam.h"
#endif

class MCalibrationBlindCamTwoNewStyle : public MCalibrationBlindCam
{
private:

  void CreatePixs();
  void CreateAreas();
  void CreateAtts();
  void CreateQEs();
  void CreateCollEffs();
  
public:

  MCalibrationBlindCamTwoNewStyle(const char *name=NULL);

  ClassDef(MCalibrationBlindCamTwoNewStyle, 1) // Container Blind Pixel Calibration Results - after run 31693
};

#endif
