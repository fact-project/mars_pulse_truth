#ifndef MARS_MCalibrationIntensityChargeCam
#define MARS_MCalibrationIntensityChargeCam

#ifndef MARS_MCalibrationIntensityCam
#include "MCalibrationIntensityCam.h"
#endif

#ifndef MARS_MCalibrationChargeCam
#include "MCalibrationChargeCam.h"
#endif

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif

class TGraphErrors;
class TH2F;
class TH1F;
class MGeomCam;
class MCalibrationIntensityChargeCam : public MCalibrationIntensityCam
{
private:

  void Add(const UInt_t a, const UInt_t b);

public:

  MCalibrationIntensityChargeCam(const char *name=NULL, const char *title=NULL);

  Int_t CountNumValidEntries(const UInt_t pixid, const MCalibrationCam::PulserColor_t col=MCalibrationCam::kNONE) const;
  
  // Graphs

  TGraphErrors *GetRazmikPlot( const UInt_t pixid );
  TGraphErrors *GetPheVsCharge( const UInt_t pixid, const MCalibrationCam::PulserColor_t col=MCalibrationCam::kNONE);
  TGraphErrors *GetPhePerCharge( const UInt_t pixid, const MGeomCam &geom, const MCalibrationCam::PulserColor_t col=MCalibrationCam::kNONE);
  TGraphErrors *GetPhePerChargePerArea( const Int_t aidx, const MGeomCam &geom, const MCalibrationCam::PulserColor_t col=MCalibrationCam::kNONE);
  TGraphErrors *GetPheVsChargePerArea( const Int_t aidx, const MCalibrationCam::PulserColor_t col=MCalibrationCam::kNONE);
  TH2F         *GetRazmikPlotResults( const Int_t aidx, const MGeomCam &geom );

  TGraphErrors *GetChargePerAreaVsTime( const Int_t aidx, const MGeomCam &geom );  
  TGraphErrors *GetPhePerAreaVsTime( const Int_t aidx, const MGeomCam &geom );
  TGraphErrors *GetPhotVsTime( const Option_t *method="FFactor" );

  TGraphErrors *GetVarPerAreaVsTime( const Int_t aidx, const MGeomCam &geom, const Option_t *varname );
  TGraphErrors *GetVarVsTime( const Int_t pixid , const Option_t *varname );
  TH1F         *GetVarFluctuations( const Int_t aidx, const MGeomCam &geom, const Option_t *varname);
  
  // Draws
  void DrawRazmikPlot( const UInt_t pixid );
  void DrawPheVsCharge( const UInt_t pixid, const MCalibrationCam::PulserColor_t col=MCalibrationCam::kNONE);       // *MENU*
  void DrawPhePerCharge( const UInt_t pixid, const MCalibrationCam::PulserColor_t col=MCalibrationCam::kNONE);      // *MENU*
  void DrawPhePerChargePerArea( const Int_t aidx, const MCalibrationCam::PulserColor_t col=MCalibrationCam::kNONE); // *MENU*
  void DrawPheVsChargePerArea( const Int_t aidx, const MCalibrationCam::PulserColor_t col=MCalibrationCam::kNONE);  // *MENU*
  void DrawRazmikPlotResults( const Int_t aidx );                          // *MENU*

  void DrawChargePerAreaVsTime( const Int_t aidx);                         // *MENU*
  void DrawPhePerAreaVsTime( const Int_t aidx );                           // *MENU*
  void DrawPhotVsTime( const Option_t *method="FFactor");                  // *MENU*

  void DrawVarPerAreaVsTime( const Int_t aidx, const Option_t *varname );  // *MENU*
  void DrawVarVsTime( const Int_t pixid , const Option_t *varname );       // *MENU*
  void DrawVarFluctuations( const Int_t aidx, const Option_t *varname);    // *MENU*
  

  ClassDef(MCalibrationIntensityChargeCam, 1) // Container Intensity Charge Calibration Results Camera
};

#endif
