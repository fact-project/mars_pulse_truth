#ifndef MARS_MBadPixelsMerge
#define MARS_MBadPixelsMerge

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MBadPixelsCam;

class MBadPixelsMerge : public MTask
{
private:
    static const TString fgDefName;  //!
    static const TString fgDefTitle; //!

    MBadPixelsCam *fDest;            //!
    MBadPixelsCam *fSource;          //->

    Bool_t ReInit(MParList *pList);

public:
    MBadPixelsMerge(MBadPixelsCam *bad, const char *name=NULL, const char *title=NULL);
    ~MBadPixelsMerge();

    ClassDef(MBadPixelsMerge, 0) //Merge extra- and intra-loop pixels
}; 

#endif

