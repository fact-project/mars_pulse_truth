#ifndef MARS_MMcBadPixelsSet
#define MARS_MMcBadPixelsSet

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MGeomCam;
class MBadPixelsCam;

class MMcBadPixelsSet : public MTask
{
private:
    MBadPixelsCam *fBadPixels;  //!
    MGeomCam      *fGeomCam;    //!

    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);

public:
    MMcBadPixelsSet(const char *name=NULL, const char *title=NULL);

    ClassDef(MMcBadPixelsSet, 1) // Task to deal with known MC star-fields
}; 

#endif




