#ifndef MARS_MHEvent
#define MARS_MHEvent

#ifndef MARS_MH
#include "MH.h"
#endif

class MHCamera;
class MRawEvtData;
class MRawRunHeader;
class MRawEvtHeader;

class MTime;
class MMcEvt;
class MMcTrig;
class MSignalCam;
class MImgCleanStd;
class MTriggerPattern;

class MHEvent : public MH
{
public:
    enum EventType_t {
        kEvtSignalRaw, kEvtSignalDensity, kEvtSignalUncleaned,
        kEvtPedestal, kEvtPedestalRMS,
        kEvtPedPhot, kEvtPedPhotRMS,
        kEvtCleaningLevels, kEvtCleaningData,
        kEvtIdxMax, kEvtArrTime, kEvtArrTimeCleaned,
        kEvtTrigPix, kEvtIslandIndex, kEvtTimeSlope,
        kEvtTimeSlopeCleaned
    };

private:
    MRawEvtData      *fRawEvtData;    //!
    MRawRunHeader    *fRawRunHeader;  //!
    MRawEvtHeader    *fRawEvtHeader;  //!

    MTime            *fTime;          //!
    MMcEvt           *fMcEvt;         //!
    MMcTrig          *fMcTrig;        //!
    MSignalCam       *fCerPhotEvt;    //!
    MImgCleanStd     *fImgCleanStd;   //!
    MTriggerPattern  *fTrigger;       //!

    MRawEvtData      *fClone;         //->
    MTriggerPattern  *fTrigCpy;       //->

    MHCamera *fHist;

    EventType_t fType;

    Double_t fMinimum;
    Double_t fMaximum;

    TH1 *GetHistByName(const TString name) const { return (TH1*)fHist; }

    Bool_t SetupFill(const MParList *plist);
    Int_t  Fill(const MParContainer *par, const Stat_t weight=1);
    Bool_t ReInit(MParList *par);

public:
    MHEvent(EventType_t type=kEvtSignalRaw);
    ~MHEvent();

    void SetMinimum(Double_t min=-1111) { fMinimum=min; }
    void SetMaximum(Double_t max=-1111) { fMaximum=max; }

    MHCamera *GetHist() { return fHist; }

    void Draw(Option_t *o="");
    void Paint(Option_t *o="");

    ClassDef(MHEvent, 1)
};

#endif
