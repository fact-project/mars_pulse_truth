#ifndef MARS_MPedestalSubtract
#define MARS_MPedestalSubtract

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MRawRunHeader;
class MRawEvtData;
class MPedestalCam;
class MPedestalSubtractedEvt;

class MPedestalSubtract : public MTask
{
private:
    static const char *fgNamePedestalCam;            //! "MPedestalCam"
    static const char *fgNamePedestalSubtractedEvt;  //! "MPedestalSubtractedEvt"

    MRawRunHeader          *fRunHeader;      //! Run Header
    MRawEvtData            *fRawEvt;         //! Input Raw data
    MPedestalCam           *fPedestals;      //! Pedestals of all pixels in the camera
    MPedestalSubtractedEvt *fSignal;         //! Output container

    TString  fNamePedestalCam;               // Name of the 'MPedestalCam' container
    TString  fNamePedestalSubtractedEvt;     // Name of the 'MPedestalSubtractedEvt' container

    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Process();

    void Memcpy(void *sample, void *ptr, Int_t cnt) const;

public:
    MPedestalSubtract(const char *name=NULL, const char *title=NULL);

    void SetPedestalCam(MPedestalCam *pedcam) { fPedestals = pedcam; }
    void SetNamePedestalCam(const char *name=fgNamePedestalCam) { fNamePedestalCam  = name; }

    void SetNamePedestalSubtractedEvt(const char *name=fgNamePedestalSubtractedEvt) { fNamePedestalSubtractedEvt = name; }

    ClassDef(MPedestalSubtract, 0)   // Class to subtract pedestal including AB-noise
};

#endif
