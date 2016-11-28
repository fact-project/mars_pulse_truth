#ifndef MARS_MPointingPositionCalcWA
#define MARS_MPointingPositionCalcWA

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MMcEvt;
class MReportDrive;
class MPointingPos;
class MRawRunHeader;
class MObservatory;

class MPointingPosCalcWA : public MTask
{
private:
    MMcEvt        *fMcEvt;    //! MMcEvt to get simulated poiting position from
    MReportDrive  *fReport;   //! MReportDrive to get real poiting position from
    MPointingPos  *fPosition; //! Output container to store pointing position
    MObservatory  *fObs;
    MRawRunHeader *fRunHeader;
    Double_t fRa;
    Double_t fDec;
    

    UShort_t fRunType;            //! Run Type to decide where to get pointing position from

    Bool_t ReInit(MParList *plist);
    Int_t  PreProcess(MParList *plist);
    Int_t  Process();

public:
    MPointingPosCalcWA()
    {
        fName  = "MPointingPosCalcWA";
        fTitle = "Task calculating the pointing position";
    }

    void SetRaDec(Double_t ra, Double_t dec)  {fRa = ra; fDec = dec;}

    ClassDef(MPointingPosCalcWA, 0) //Task calculating the pointing position
};

#endif
