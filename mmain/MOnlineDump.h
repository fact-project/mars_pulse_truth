#ifndef MARS_MOnlineDump
#define MARS_MOnlineDump

#ifndef MARS_MGTask
#include "MGTask.h"
#endif

class MFillH;
class MHCamera;
class MRawEvtHeader;
class MRawRunHeader;
class MTime;
class MParList;
class MEventRate;

class MOnlineDump : public MGTask
{
private:
    MFillH *fFill1;
    MFillH *fFill2;
    MFillH *fFill3;

    MHCamera *fCamEvent;

    UInt_t fRunNumber;
    UInt_t fEvtNumber;

    MRawEvtHeader *fRawEvtHeader;
    MRawRunHeader *fRawRunHeader;
    MEventRate    *fRate;

    MTime *fEvtTime;
    Double_t fTime;

    MParList *fPlist;

    void SetNoContextMenu(TObject *c);
    void DisplayTriggerRate();

    Bool_t ProcessMessage(Int_t msg, Int_t submsg, Long_t mp1, Long_t mp2);

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

public:
    MOnlineDump() : fPlist(NULL) { fName = "MOnlineDump"; }
    ~MOnlineDump();

    ClassDef(MOnlineDump, 0) // Task to fill a histogram with data from a parameter container
};

#endif
