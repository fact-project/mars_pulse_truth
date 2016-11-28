#ifndef MARS_MImgCleanStd
#define MARS_MImgCleanStd

#ifndef MARS_MGTask
#include "MGTask.h"
#endif

class MGeomCam;
class MGeom;
class MSignalCam;
class MPedPhotCam;
class MArrivalTime;
class MCameraData;
//class MRawRunHeader;

class MGGroupFrame;

class MImgCleanStd : public MGTask
{
public:
    typedef enum {
        kStandard,
        kScaled,
        kDemocratic,
        kProbability,
        kAbsolute,
        kTime,
    } CleaningMethod_t;

private:
    static const TString gsNamePedPhotCam; // default name of the 'MPedPhotCam' container
    static const TString gsNameGeomCam;    // default name of the 'MGeomCam' container
    static const TString gsNameSignalCam;  // default name of the 'MSignalCam' container

    const MGeomCam      *fCam;  //!
          MSignalCam    *fEvt;  //!
          MPedPhotCam   *fPed;  //!
          MCameraData   *fData; //!
//          MRawRunHeader *fHeader; //!

    CleaningMethod_t fCleaningMethod;

    Float_t  fCleanLvl0;
    Float_t  fCleanLvl1;
    Float_t  fCleanLvl2;

    Float_t  fTimeLvl1;
    Float_t  fTimeLvl2;

    UShort_t fCleanRings;
    Bool_t   fKeepIsolatedPixels;
    Int_t    fRecoverIsolatedPixels;
    Int_t    fPostCleanType;

    TString  fNamePedPhotCam; // name of the 'MPedPhotCam' container
    TString  fNameGeomCam;    // name of the 'MGeomCam' container
    TString  fNameSignalCam;  // name of the 'MSignalCam' container

    // MImgCleanStd
    Bool_t HasCoreNeighbors(const MGeom &gpix) const;
    Bool_t HasUsedNeighbors(const MGeom &gpix) const;
    void   SetUsedNeighbors(const MGeom &gpix, Int_t r=1) const;
    Int_t  DoCleaning(Float_t &size) const;
    void   ResetCleaning() const;
    Int_t  RecoverIsolatedPixels(Float_t &size) const;
    void   CleanTime(Int_t n, Double_t lvl) const;

    void CleanStepTime() const;

    // MGTask, MTask, MParContainer
    void    CreateGuiElements(MGGroupFrame *f);
    Int_t   ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    void    StreamPrimitive(std::ostream &out) const;

    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:
    MImgCleanStd(const Float_t lvl1=3.0, const Float_t lvl2=2.5,
                 const char *name=NULL, const char *title=NULL);
    void Print(Option_t *o="") const;

    Float_t GetCleanLvl0() const { return fCleanLvl0; }
    Float_t GetCleanLvl1() const { return fCleanLvl1; }
    Float_t GetCleanLvl2() const { return fCleanLvl2; }

    Float_t GetTimeLvl1() const { return fTimeLvl1; }
    Float_t GetTimeLvl2() const { return fTimeLvl2; }

    void SetCleanLvl0(Float_t lvl) { fCleanLvl0=lvl; }
    void SetCleanLvl1(Float_t lvl) { fCleanLvl1=lvl; }
    void SetCleanLvl2(Float_t lvl) { fCleanLvl2=lvl; }

    void SetTimeLvl1(Float_t lvl) { fTimeLvl1=lvl; }
    void SetTimeLvl2(Float_t lvl) { fTimeLvl2=lvl; }

    void SetCleanRings(UShort_t r) { fCleanRings=r; }
    UShort_t GetCleanRings() const { return fCleanRings;}

    void SetPostCleanType(Int_t t) { fPostCleanType=t; }

    void SetMethod(CleaningMethod_t m) { fCleaningMethod = m; }
    void SetKeepIsolatedPixels(Bool_t b=kTRUE) { fKeepIsolatedPixels=b; }
    void SetRecoverIsolatedPixels(Int_t n=-1) { fRecoverIsolatedPixels=n; }

    Bool_t ProcessMessage(Int_t msg, Int_t submsg, Long_t param1, Long_t param2);

    void SetNamePedPhotCam(const char *name)  { fNamePedPhotCam = name; }
    void SetNameSignalCam(const char *name)   { fNameSignalCam = name; }
    void SetNameGeomCam(const char *name)     { fNameGeomCam = name; }

    ClassDef(MImgCleanStd, 4)    // task doing the image cleaning
}; 

#endif
