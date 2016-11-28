#ifndef MARS_MTreatSaturation
#define MARS_MTreatSaturation

#ifndef MARS_MArrayF
#include "MArrayF.h"
#endif
#ifndef MARS_MTask
#include "MTask.h"
#endif

class MRawEvtData;
class MPedestalSubtractedEvt;

class MTreatSaturation : public MTask
{
private:
    MRawEvtData            *fRaw;
    MPedestalSubtractedEvt *fEvt;

    MArrayF fDev1;
    MArrayF fDev2;

    Int_t  PreProcess(MParList *pList);
    Int_t  Process();

public:
    MTreatSaturation(const char *name=NULL, const char *title=NULL);

    ClassDef(MTreatSaturation, 0)   // Replace saturating samples with pulse
};

#endif
