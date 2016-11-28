#ifndef MARS_MCameraSmooth
#define MARS_MCameraSmooth

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef ROOT_TArrayS
#include <TArrayS.h>
#endif

class MGeomCam;
class MSignalCam;

class MCameraSmooth : public MTask
{
private:
    MSignalCam *fEvt;     //! Pointer to data to smooth
    MGeomCam   *fGeomCam; //! Camera geometry

    Byte_t fCounts;           // number of smoothing loops
    Bool_t fUseCentralPixel;  // use also the central pixel

    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:
    MCameraSmooth(Byte_t cnt=1, const char *name=NULL, const char *title=NULL);

    void SetUseCentralPixel(Bool_t b=kTRUE) { fUseCentralPixel=kTRUE; }

    ClassDef(MCameraSmooth, 0) // task to smooth the camera contants
}; 

#endif

