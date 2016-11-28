#ifndef MARS_MCalibColorSteer
#define MARS_MCalibColorSteer

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif


class MParList;
class MGeomCam;
class MCalibrationPattern;
class MRawRunHeader;
class MCalibrationChargeCalc;
class MCalibrationChargeCam;
class MCalibrationRelTimeCalc;
class MCalibrationRelTimeCam;
class MCalibrationBlindCam;
class MCalibrationQECam;
class MBadPixelsCam;

class MCalibColorSteer : public MTask
{
private:
  MCalibrationPattern             *fCalibPattern;    //!
  MRawRunHeader                   *fRunHeader;       //!
  MGeomCam                        *fGeom;            //!
  MParList                        *fParList;         //!
  MCalibrationChargeCam           *fCharge;          //!
  MCalibrationRelTimeCam          *fRelTimeCam;      //!
  MCalibrationBlindCam            *fBlindCam;        //!
  MCalibrationQECam               *fQECam;           //!
  MBadPixelsCam                   *fBad;             //!

  MCalibrationChargeCalc          *fChargeCalc;      //!
  MCalibrationRelTimeCalc         *fRelTimeCalc;     //!

  MCalibrationCam::PulserColor_t   fColor;
  Float_t                          fStrength;

  Bool_t                           fHistCopy;        // Decide whether MHCalibrationCams will get copied

  Int_t  PreProcess(MParList *pList);
  Int_t  Process();
  Int_t  PostProcess();

  Bool_t ReInitialize();
  Bool_t Finalize(const char* name);

  //void    CopyHist(const char* name);
  TString GetNamePattern();

public:
    MCalibColorSteer(const char *name=NULL, const char *title=NULL);

    void SetHistCopy(const Bool_t b=kTRUE) { fHistCopy = b; }

    ClassDef(MCalibColorSteer, 1) // Task to steer the processing of multiple calibration colours
};
    
#endif

