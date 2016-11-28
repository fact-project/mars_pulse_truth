#ifndef MARS_MPointingPosCalc
#define MARS_MPointingPosCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MMcEvt;
class MReportDrive;
class MPointingPos;

class MPointingPosCalc : public MTask
{
private:
    MMcEvt       *fMcEvt;    //! MMcEvt to get simulated poiting position from
    MReportDrive *fReport;   //! MReportDrive to get real poiting position from
    MPointingPos *fPosition; //! Output container to store pointing position

    UShort_t fRunType;       //! Run Type to decide where to get pointing position from

    Bool_t ReInit(MParList *plist);
    Int_t  PreProcess(MParList *plist);
    Int_t  Process();

public:
    MPointingPosCalc() : fMcEvt(0), fReport(0), fPosition(0)
    {
        fName  = "MPointingPosCalc";
        fTitle = "Task calculating the pointing position";

        AddToBranchList("MReportDrive.*");
        AddToBranchList("MMcEvt.*");
    }

    ClassDef(MPointingPosCalc, 0) //Task calculating the pointing position
};

#endif
