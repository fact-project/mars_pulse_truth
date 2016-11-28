#ifndef MARS_MIslandsCalc
#define MARS_MIslandsCalc

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
class MGeomCam;
class MIslands;
class MImgIsland;

class MIslandsCalc : public MGTask
{
 private:
    const MGeomCam    *fCam;        //!
    MCerPhotEvt       *fEvt;        //!
    MSigmabar         *fSgb;        //!
    MPedestalCam      *fPed;        //!
    MArrivalTimeCam   *fTime;       //!
    MGeomCam          *fGeomCam;

    MIslands          *fIsl;        //!   output container to store result
  
    TString           fIslName;     //    name of the 'MIslands' container
  
    Int_t  fIslandAlgorithm;

    Int_t PreProcess(MParList *plist);
    Int_t Process();
    Int_t IslandPar();               //
    void  Calc1(Int_t&,const Int_t,const Int_t,Int_t**,Int_t*);    // algorithm of counting islands #1
    void  Calc2(Int_t&,const Int_t,const Int_t,Int_t**,Int_t*);    // algorithm of counting islands #2
    
           
 public:
    MIslandsCalc(const char* name=NULL, const char* title=NULL);
    void SetOutputName(TString outname)   { fIslName = outname; }
    
    void SetAlgorithm(Int_t m)   {fIslandAlgorithm = m;}
    
    ClassDef(MIslandsCalc, 0)        // task doing the image cleaning
}; 

#endif
