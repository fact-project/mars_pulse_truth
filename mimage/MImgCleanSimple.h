#ifndef MARS_MImgCleanSimple
#define MARS_MImgCleanSimple

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MGeomCam;
class MGeom;
class MSignalCam;
class MArrivalTime;

class MImgCleanSimple : public MTask
{
public:
    typedef enum {
        kStandard,
        kSum2,
        kSum3,
    } CleaningMethod_t;

private:
    static const TString gsNameGeomCam;    // default name of the 'MGeomCam' container
    static const TString gsNameSignalCam;  // default name of the 'MSignalCam' container

    const MGeomCam   *fCam;  //!
          MSignalCam *fEvt;  //!

    CleaningMethod_t fCleaningMethod;

    Float_t  fCleanLvl1;
    Float_t  fCleanLvl2;

    Float_t  fTimeLvl1;
    Float_t  fTimeLvl2;

    TString  fNameGeomCam;    // name of the 'MGeomCam' container
    TString  fNameSignalCam;  // name of the 'MSignalCam' container

    // MImgCleanSimple
    void ResetCleaning() const;
    void DoCleanStd() const;
    void DoCleanSum2() const;
    void DoCleanSum3() const;

    // MGTask, MTask, MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);
    //void  StreamPrimitive(ostream &out) const;

    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:
    MImgCleanSimple(const Float_t lvl1=2.0, const Float_t lvl2=1.75,
                 const char *name=NULL, const char *title=NULL);

    void Print(Option_t *o="") const;

    Float_t GetCleanLvl1() const { return fCleanLvl1; }
    Float_t GetCleanLvl2() const { return fCleanLvl2; }
    Float_t GetTimeLvl1() const  { return fTimeLvl1; }
    Float_t GetTimeLvl2() const  { return fTimeLvl2; }

    void SetCleanLvl1(Float_t lvl) { fCleanLvl1=lvl; }
    void SetCleanLvl2(Float_t lvl) { fCleanLvl2=lvl; }
    void SetTimeLvl1(Float_t lvl)  { fTimeLvl1=lvl; }
    void SetTimeLvl2(Float_t lvl)  { fTimeLvl2=lvl; }

    void SetMethod(CleaningMethod_t m) { fCleaningMethod = m; }

    void SetNameSignalCam(const char *name)   { fNameSignalCam = name; }
    void SetNameGeomCam(const char *name)     { fNameGeomCam = name; }

    ClassDef(MImgCleanSimple, 1)    // task doing the image cleaning
}; 

#endif
