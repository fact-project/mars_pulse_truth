#ifndef MARS_MDrsCalibApply
#define MARS_MDrsCalibApply

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef MARS_DrsCalib
#include "DrsCalib.h"
#endif

#include <list>

class MRawRunHeader;
class MRawEvtData;
class MPedestalSubtractedEvt;
class MDrsCalibration;

class MDrsCalibResult: public MParContainer
{
    friend class MDrsCalibApply;

public:
    std::vector<DrsCalibrate::Step> fData;

    MDrsCalibResult()
    {
        fName = "MDrsCalibResult";
    }

    const std::vector<DrsCalibrate::Step> &GetData() const { return fData; }

    ClassDef(MDrsCalibResult, 0)
};

class MDrsCalibApply : public MTask
{
private:
    MRawRunHeader          *fRunHeader;      //! Run Header
    MRawEvtData            *fRawEvt;         //! Input Raw data
    MDrsCalibration        *fDrsCalib;       //! DRS4 calibration data
    MPedestalSubtractedEvt *fSignal;         //! Output container
    MDrsCalibResult        *fResult;         //! Output container

    std::list<std::vector<Short_t>> fPrevStart; //! History for start cells of previous events

    UShort_t fMaxNumPrevEvents;
    UShort_t fRemoveSpikes;
    UShort_t fSlidingAverage;

    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Process();

public:
    MDrsCalibApply(const char *name=NULL, const char *title=NULL);

    void SetMaxNumPrevEvents(UShort_t n)  { fMaxNumPrevEvents = n; }
    void SetRemoveSpikes(int typ=1) { fRemoveSpikes = typ; }
    void SetSlidingAverage(UShort_t w) { fSlidingAverage = w; }

    ClassDef(MDrsCalibApply, 0)   // Class to subtract pedestal including AB-noise
};

#endif
