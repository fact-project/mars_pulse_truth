#ifndef MARS_MPedestalCalc
#define MARS_MPedestalCalc

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MPedestalCalc                                                           //
//                                                                         //
// Implementation of ped. Eval. defined in Jan 02                          //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef MARS_MHFadCam
#include "MHFadcCam.h"
#endif

class MTime;
class MRawEvtData;
class MPedestalCam;

class MPedestalCalc : public MTask 
{
private:
    Int_t   fNumBranches;
    Float_t fTimeSlot;

    MRawEvtData  *fRawEvt;
    MPedestalCam *fPedestals;  //

    MHFadcCam *fHists;         //[fNumBranches]

    MTime     *fPedTime; // time of the pedestal event
    MTime     *fEvtTime; // time of the current event
    MTime     *fStart;   //[fNumBranches] starting time of the current pedestal


    void Calc(Int_t i) const;
    void Fill(Int_t i);
    void Check(Int_t i);

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

public:
    MPedestalCalc(const char *name=NULL, const char *title=NULL);

    void SetTimeSlot(Float_t newslot) { fTimeSlot = newslot; }
    void SetNumBranches(Int_t num) { fNumBranches = num; }

    ClassDef(MPedestalCalc, 0) // Task to calculate the pestels from pedestal events
};

#endif



