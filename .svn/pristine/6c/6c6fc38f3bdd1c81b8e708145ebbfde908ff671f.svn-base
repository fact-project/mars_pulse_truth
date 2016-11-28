#ifndef MARS_MBadPixelsCalc
#define MARS_MBadPixelsCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif
#ifndef MARS_MBadPixelsPix
#include "MBadPixelsPix.h"
#endif

class MBadPixelsCam;
class MGeomCam;
class MPedPhotCam;
class MPedestalCam;

class MBadPixelsCalc : public MTask
{
private:
    const MGeomCam      *fGeomCam;    //! Input container storing the pixel sizes
    const MPedPhotCam   *fPedPhotCam; //! Input container storing the pedestal and pedestal rms of all pixels
          MBadPixelsCam *fBadPixels;  //! Output container holding the bad pixels

    Float_t fPedestalLevel;
    Float_t fPedestalLevelVarianceLo;
    Float_t fPedestalLevelVarianceHi;

    TString fNamePedPhotCam; // name of the 'MPedPhotCam' container

    Bool_t fCheckInProcess;
    Bool_t fCheckInPostProcess;

    // MTask
    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
    MBadPixelsCalc(const char *name=NULL, const char *title=NULL);

    // Setter
    void SetPedestalLevel(Float_t f=-1)           { fPedestalLevel=f; }
    void SetPedestalLevelVariance(Float_t f=-1)   { fPedestalLevelVarianceLo=fPedestalLevelVarianceHi=f; }
    void SetPedestalLevelVarianceLo(Float_t f=-1) { fPedestalLevelVarianceLo=f; }
    void SetPedestalLevelVarianceHi(Float_t f=-1) { fPedestalLevelVarianceHi=f; }

    void SetNamePedPhotCam(const char *name)      { fNamePedPhotCam = name; }
    void SetGeomCam(const MGeomCam *geom)         { fGeomCam = geom; }

    void EnableCheckInProcess(Bool_t b=kTRUE)     { fCheckInProcess = b; }
    void EnableCheckInPostProcess(Bool_t b=kTRUE) { fCheckInPostProcess = b; }

    // MBadPixelsCalc
    Bool_t CheckPedestalRms(MBadPixelsPix::UnsuitableType_t t) const;
    Bool_t CheckPedestalRms(MBadPixelsCam &cam, const MPedPhotCam  &ped, MBadPixelsPix::UnsuitableType_t t=MBadPixelsPix::kUnsuitableRun);
    Bool_t CheckPedestalRms(MBadPixelsCam &cam, const MPedestalCam &ped, MBadPixelsPix::UnsuitableType_t t=MBadPixelsPix::kUnsuitableRun);

    ClassDef(MBadPixelsCalc, 1) // Task to find bad pixels (star, broken pixels, etc)
}; 

#endif




