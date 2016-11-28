#ifndef MARS_MSignalCalc
#define MARS_MSignalCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MPedestalCam;
class MSignalCam;
class MRawRunHeader;
class MPedestalSubtractedEvt;

class MSignalCalc : public MTask
{
private:
    MRawRunHeader          *fRunHeader;  // RunHeader information
    MPedestalSubtractedEvt *fRawEvt;     // raw event data (time slices)
    MSignalCam             *fCerPhotEvt; // Cerenkov Photon Event used for calculation
    MPedestalCam           *fPedestals;  // Pedestals of all pixels in the camera

    Byte_t fBefore;
    Byte_t fAfter;

    Int_t fSkip;                //!

    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Process();
    Int_t  PostProcess();

public:
    MSignalCalc(Byte_t b=2, Byte_t a=5, const char *name=NULL, const char *title=NULL);

    ClassDef(MSignalCalc, 0)   // Task to calculate cerenkov photons from raw data
};

#endif
