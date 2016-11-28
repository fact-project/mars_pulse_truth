#ifndef MARS_MCalibCalcFromPast
#define MARS_MCalibCalcFromPast

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif

class MParList;
class MGeomCam;
class MRawRunHeader;
class MPedCalcFromLoGain;
class MCalibrationChargeCalc;
class MCalibrationRelTimeCalc;
class MCalibrateData;
class MCalibrationChargeCam;
class MCalibrationBlindCam;
class MCalibrationQECam;

class MBadPixelsCam;

class MCalibCalcFromPast : public MTask
{
private:
  static const UInt_t fgNumEventsDump;               //! Default for fNumEventsDump
  static const UInt_t fgNumPhesDump;                 //! Default for fNumPhesDump
  
  MGeomCam                        *fGeom;            //! Camera Geometry
  MParList                        *fParList;         //! Parameter List
  MRawRunHeader                   *fRunHeader;       //! Run header storing the run number
  MCalibrationChargeCam           *fCharge;          //! Intensity Charge Cam (to be created)

  MCalibrationBlindCam            *fBlindCam;        //! Blind  Cam
  MCalibrationQECam               *fQECam;           //! QE Cam
  MBadPixelsCam                   *fBadPixels;       //! Bad Pixels Cam

  MCalibrationChargeCalc          *fChargeCalc;      //! Charge Calibration Task
  MCalibrationRelTimeCalc         *fRelTimeCalc;     //! Rel. Times Calibratio Task
  MCalibrateData                  *fCalibrate;       //! Data Calibration Task
                                                     
  UInt_t fNumCam;              //! Number of currently used Calibration Cam
  UInt_t fNumEventsDump;       // Number of event after which the MCalibrationCams gets updated
  UInt_t fNumEvents;           //! Event counter only for calibration events
  UInt_t fNumPhesDump;         // Number of cams after which the number of phes gets averaged
  UInt_t fNumPhes;             //! Event counter for photo-electron updates

  Float_t fMeanPhes;
  Float_t fMeanPhesRelVar;

  Bool_t fUpdateWithFFactorMethod; // Update the interlaced calib events with the full F-Factor method
  Bool_t fUpdateNumPhes;           // Update the number of photo-electrons only after fNumPhesDump number of Cams

  TArrayF fPhes;
  TArrayF fPhesVar;

  Int_t fNumFails;          //! How often got the update skipped?

  // MTask
  Int_t  PreProcess(MParList *pList);
  Int_t  Process();
  Int_t  PostProcess();
  Bool_t ReInit(MParList *pList);

  // MCalibCalcFromPast
  Bool_t ReInitialize();
  void Finalize(const char* name, Bool_t finalize=kTRUE);

  Bool_t UpdateMeanPhes();

  // MParContainer
  Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
  MCalibCalcFromPast(const char *name=NULL, const char *title=NULL);

  // Getter
  UInt_t GetNumEventsDump() const   {  return fNumEventsDump;   }

  // Setter
  void SetNumEventsDump( const UInt_t i=fgNumEventsDump )   { fNumEventsDump = i; }
  void SetNumPhesDump  ( const UInt_t i=fgNumPhesDump   )   { fNumPhesDump   = i; }
  
  void SetCalibrate    ( MCalibrateData          *c )  { fCalibrate   = c; }
  void SetChargeCalc   ( MCalibrationChargeCalc  *c )  { fChargeCalc  = c; }
  void SetRelTimeCalc  ( MCalibrationRelTimeCalc *c )  { fRelTimeCalc = c; }
  void SetUpdateNumPhes      ( const Bool_t b=kTRUE )  { fUpdateNumPhes           = b;  }

  void SetUpdateWithFFactorMethod(const Bool_t b=kTRUE){ fUpdateWithFFactorMethod = b; }
  
  ClassDef(MCalibCalcFromPast, 2) // Task to steer the processing of interlace calibration events
};

#endif

