#ifndef MARS_MPointingPosInterpolate
#define MARS_MPointingPosInterpolate

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef ROOT_TSpline
#include <TSpline.h>
#endif

#ifndef MARS_MTime
#include "MTime.h"
#endif

class MTime;
class MPointingPos;
class MRawRunHeader;
class MDirIter;

class MPointingPosInterpolate : public MTask
{
public:

    enum TimeMode_t {
        kRunTime,
        kEventTime
    };

private:

  static const Int_t fgNumStartEvents; //! Default for fNumStartEvents (now set to: 10000)

  Int_t fNumStartEvents;               //  Start number of allowed events

  Bool_t fDebug;

  TString fFilename;
  MTime   fFirstDriveTime;
  MTime   fLastDriveTime;
  
  MTime         *fEvtTime;             //! Raw event time
  MPointingPos  *fPointingPos;         //! Telescope pointing postion
  MRawRunHeader *fRunHeader;           //! Run Header
  MDirIter      *fDirIter;             //! Dir Iter
  
  TSpline3* fSplineZd;                 //! Zd vs. time
  TSpline3* fSplineAz;                 //! Az vs. time

  Double_t  fRa;                       // RA of source
  Double_t  fDec;                      // Dec of source

  Int_t PreProcess(MParList *pList);
  Int_t Process();
  Bool_t ReadDriveReport();    

  TimeMode_t fTimeMode;

  Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
    
  MPointingPosInterpolate(const char *name=NULL, const char *title=NULL);

  ~MPointingPosInterpolate();

  void AddFiles(MDirIter *dir) { fDirIter = dir; }
  void AddFile(const char *name) { fFilename = name; }

  void SetTimeMode( TimeMode_t mode) { fTimeMode = mode; }
  void SetDebug( const Bool_t b=kTRUE) { fDebug = b; }

  void Clear(Option_t *o="");
  
  Int_t GetNumStartEvents() const { return fNumStartEvents; }
  void  SetNumStartEvents ( const Int_t i=fgNumStartEvents ) { fNumStartEvents = i; }

  ClassDef(MPointingPosInterpolate, 1)  // Interpolate the drive pointing positions
};

#endif
