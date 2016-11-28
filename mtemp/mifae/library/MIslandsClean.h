#ifndef MARS_MIslandsClean
#define MARS_MIslandsClean

#ifndef MARS_MGTask
#include "MGTask.h"
#endif

#ifndef ROOT_TArrayF
#include <TArrayF.h>
#endif

class MGeomCam;
class MSigmabar;
class MCerPhotPix;
class MCerPhotEvt;
class MPedestalCam;
class MArrivalTimeCam;
class MArrivalTimePix;
class MIslands;
class MImgIsland;

class MIslandsClean : public MGTask
{
 public: 

  typedef enum {
    kTiming,
    kNoTiming
  } IslandCleaningMethod_t;
  
 private:
    const MGeomCam    *fCam;        //!
    MCerPhotEvt       *fEvt;        //!
    MSigmabar         *fSgb;        //!
    MPedestalCam      *fPed;        //!
    MArrivalTimeCam   *fTime;       //!
    MIslands          *fIsl;        //!   
   
    TString           fIslName;

    // IslandCleaningMethod_t fIslandCleaningMethod;
    Int_t fIslandCleaningMethod;
   
    Float_t fIslCleanThres;

    Int_t PreProcess(MParList *plist);
    Int_t Process();
           
 public:
    MIslandsClean(const Float_t newThres=50, const char *name=NULL, const char *title=NULL);
    
    void SetInputName(TString inname)    {fIslName = inname;}
    
    //void SetMethod(IslandCleaningMethod_t m)   {fIslandCleaningMethod = m;}
    void SetMethod(Int_t m)   {fIslandCleaningMethod = m;}
    
    ClassDef(MIslandsClean, 0)        // task doing the island cleaning
}; 

#endif
