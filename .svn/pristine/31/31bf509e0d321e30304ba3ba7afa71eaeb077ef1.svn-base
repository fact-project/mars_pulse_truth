#ifndef MARS_MSoftwareTriggerCalc
#define MARS_MSoftwareTriggerCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MRawEvtData;
class MSoftwareTrigger;
class MPedestalSubtractedEvt;

class MSoftwareTriggerCalc : public MTask
{
private:
    MRawEvtData *fRawEvt;
    MPedestalSubtractedEvt *fSignal;
    MSoftwareTrigger *fTrigger;

    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Process();
    Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
    MSoftwareTriggerCalc(const char *name=NULL, const char *title=NULL);

    virtual Bool_t InitArrays(Int_t) { return kTRUE; }

    ClassDef(MSoftwareTriggerCalc, 0)   // Time And Charge Extractor Base Class
};

#endif
