#ifndef MARS_MCalibrationRelTimeCam
#define MARS_MCalibrationRelTimeCam

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif

class MCalibrationRelTimeCam : public MCalibrationCam
{
private:

  void Add(const UInt_t a, const UInt_t b);
  void AddArea(const UInt_t a, const UInt_t b);
  void AddSector(const UInt_t a, const UInt_t b);

public:

  MCalibrationRelTimeCam(const char *name=NULL, const char *title=NULL);

  // Prints
  void Print(Option_t *o="") const;
  
  // Others
  Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;

  ClassDef(MCalibrationRelTimeCam, 2)	// Container Rel. Arrival Time Calibration Results Camera
};

#endif
