#ifndef MARS_MCalibrationRelTimeCalc
#define MARS_MCalibrationRelTimeCalc

/////////////////////////////////////////////////////////////////////////////
//
// MCalibrationRelTimeCalc
//
// Integrates the time slices of the all pixels of a calibration event
// and substract the pedestal value
//
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef MARS_MBadPixelsPix
#include "MBadPixelsPix.h"
#endif

class MCalibrationRelTimeCam;
class MGeomCam;
class MBadPixelsCam;

class MCalibrationRelTimeCalc : public MTask
{
private:

  static const Float_t fgRelTimeResolutionLimit; //! Default for fRelTimeResolutionLimit (now set to: 0.75)
 
  // Variables
  Float_t fRelTimeResolutionLimit;               //  Limit acceptance rel. time resolution (in FADC slices)
  
  // Pointers
  MBadPixelsCam              *fBadPixels;        //  Bad Pixels 
  MCalibrationRelTimeCam     *fCam;              //  Calibrated RelTimes of all pixels 
  MGeomCam                   *fGeom;             //! Camera geometry

  // enums
  enum  Check_t
  {
      kCheckHistOverflow,
      kCheckDeviatingBehavior,
      kCheckOscillations,
      kCheckFitResults
  };                                             // Possible Checks

  Byte_t fCheckFlags;                            // Bit-field to hold the possible check flags

  enum  { kDebug };                              //  Possible flags

  Byte_t  fFlags;                                //  Bit-field for the general flags
  
  // functions
  const char* GetOutputFile       ();
  void   FinalizeAverageResolution();
  void   FinalizeRelTimes         ();
  void   FinalizeBadPixels        ();
  void   FinalizeUnsuitablePixels ();

  void   PrintUncalibrated(const char *text, Int_t in, Int_t out) const;
  void   PrintUncalibrated(MBadPixelsPix::UncalibratedType_t typ, const char *text) const;

  // Query checks
  Bool_t IsCheckDeviatingBehavior() const { return TESTBIT(fCheckFlags,kCheckDeviatingBehavior); }
  Bool_t IsCheckHistOverflow     () const { return TESTBIT(fCheckFlags,kCheckHistOverflow);      }
  Bool_t IsCheckOscillations     () const { return TESTBIT(fCheckFlags,kCheckOscillations);      }
  Bool_t IsCheckFitResults       () const { return TESTBIT(fCheckFlags,kCheckFitResults);        }

  // MTask
  Bool_t ReInit     (MParList *pList); 
  Int_t  Process    () { return kTRUE; }
  Int_t  PostProcess();

  // MParContainer
  Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
  MCalibrationRelTimeCalc(const char *name=NULL, const char *title=NULL);

  // TObject
  void Clear(const Option_t *o="");

  // MCalibrationRelTimeCalc
  Int_t Finalize();
  
  // Getter
  Bool_t IsDebug() const  {  return TESTBIT(fFlags,kDebug); }

  // Setter
  void SetRelTimeResolutionLimit( const Float_t f=fgRelTimeResolutionLimit ) { fRelTimeResolutionLimit = f; }

  // Checks
  void SetCheckFitResults(const Bool_t b=kTRUE)        { b ? SETBIT(fCheckFlags,kCheckFitResults) : CLRBIT(fCheckFlags,kCheckFitResults); }
  void SetCheckDeviatingBehavior(const Bool_t b=kTRUE) { b ? SETBIT(fCheckFlags,kCheckDeviatingBehavior) : CLRBIT(fCheckFlags,kCheckDeviatingBehavior); }
  void SetCheckHistOverflow(const Bool_t b=kTRUE)      { b ? SETBIT(fCheckFlags,kCheckHistOverflow) : CLRBIT(fCheckFlags,kCheckHistOverflow); }
  void SetCheckOscillations(const Bool_t b=kTRUE)      { b ? SETBIT(fCheckFlags,kCheckOscillations) : CLRBIT(fCheckFlags,kCheckOscillations); }
  void SetDebug(const Bool_t b=kTRUE)                  { b ? SETBIT(fFlags, kDebug) : CLRBIT(fFlags, kDebug); }

  ClassDef(MCalibrationRelTimeCalc, 3)   // Task finalizing the relative time Calibration
};

#endif
