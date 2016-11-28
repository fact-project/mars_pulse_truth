#ifndef MARS_MCalibColorSet
#define MARS_MCalibColorSet

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef MARS_MCalibrationCam
#include "MCalibrationCam.h"
#endif

class MParList;
class MCalibrationPattern;
class MRawEvtHeader;

class MCalibColorSet : public MTask
{
private:

  static const Int_t gkIFAEBoxInaugurationRun; //! Run number of first IFAE box calibration (set to: 20113)
  static const UInt_t gkFirstRunWithFinalBits; //! Run number of first functionning digital modules 
  
  MCalibrationPattern *fPattern;               //! Calibration Pattern with the pulse pattern information
  MRawEvtHeader       *fHeader;                //! Event header with the project name
  
  MCalibrationCam::PulserColor_t fColor;       //  Pulser Color to be set
  Float_t                        fStrength;    //  Pulser Strength to be set

  Bool_t fIsValid;                             //  Have to set the pulse pattern?
  Bool_t fIsExplicitColor;                     //  Is colour set explicitely from outside (for MC)? 
  
  void CheckAndSet(const TString &str, const char *regexp, MCalibrationCam::PulserColor_t col, Float_t strength);

  Bool_t ReInit(MParList *pList);
  Int_t  PreProcess(MParList *pList);
  Int_t  Process();

  Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print);
  
public:

  MCalibColorSet(const char *name=NULL, const char *title=NULL);
  
  void Clear(const Option_t *o="");

  void SetExplicitColor( const MCalibrationCam::PulserColor_t col) 
    {
      fIsExplicitColor = kTRUE;
      fColor    = col;
      fStrength = 10.;
    }
  
  ClassDef(MCalibColorSet, 0) // Task to workaround missing colors
};
    
#endif

