#ifndef MARS_MCalibrationIntensityRelTimeCam
#define MARS_MCalibrationIntensityRelTimeCam

#ifndef MARS_MCalibrationIntensityCam
#include "MCalibrationIntensityCam.h"
#endif

#ifndef MARS_MCalibrationIntensityChargeCam
#include "MCalibrationIntensityChargeCam.h"
#endif

class TGraphErrors;
class MCalibrationIntensityRelTimeCam : public MCalibrationIntensityCam
{
private:

  void Add(const UInt_t from, const UInt_t to);

public:

  MCalibrationIntensityRelTimeCam(const char *name=NULL, const char *title=NULL);

  TGraphErrors *GetTimeResVsCharge( const UInt_t pixid, const MCalibrationIntensityChargeCam &chargecam,
                                    const MCalibrationCam::PulserColor_t col=MCalibrationCam::kNONE);

  TGraphErrors *GetTimeResVsChargePerArea( const Int_t aidx, const MCalibrationIntensityChargeCam &chargecam,
                                           const MGeomCam &geom,
                                           const MCalibrationCam::PulserColor_t col=MCalibrationCam::kNONE);

  TGraphErrors *GetTimeResVsSqrtPhePerArea( const Int_t aidx, const MCalibrationIntensityChargeCam &chargecam,
                                           const MGeomCam &geom,
                                           const MCalibrationCam::PulserColor_t col=MCalibrationCam::kNONE);

  ClassDef(MCalibrationIntensityRelTimeCam, 1) // Container Intensity Rel.Times Calibration Results Camera
};

#endif
