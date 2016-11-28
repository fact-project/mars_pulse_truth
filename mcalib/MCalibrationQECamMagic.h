#ifndef MARS_MCalibrationQECamMagic
#define MARS_MCalibrationQECamMagic

#ifndef MARS_MCalibrationQECam
#include "MCalibrationQECam.h"
#endif

class MCalibrationQECamMagic : public MCalibrationQECam
{
private:

  void CreateCorningReds();
  void CreateCorningBlues();

public:

  MCalibrationQECamMagic(const char *name=NULL, const char *title=NULL);

  ClassDef(MCalibrationQECamMagic, 1) // Container Quantum Efficieny for MAGIC
};

#endif





