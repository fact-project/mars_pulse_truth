#ifndef MARS_MCalibrationHiLoCam
#define MARS_MCalibrationHiLoCam

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif

class MCalibrationHiLoPix;

class MCalibrationHiLoCam : public MCalibrationCam
{
private:

  void Add(const UInt_t a, const UInt_t b);
  void AddArea(const UInt_t a, const UInt_t b);
  void AddSector(const UInt_t a, const UInt_t b);

  void PrintPix(const MCalibrationHiLoPix &pix, const char *type) const;

public:

  MCalibrationHiLoCam(const char *name=NULL, const char *title=NULL);

  // Prints
  void Print(Option_t *o="") const; //*MENU*

  // Others
  Bool_t GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type=0) const;

  ClassDef(MCalibrationHiLoCam, 1)	// Container Rel. Arrival Time Calibration Results Camera
};

#endif
