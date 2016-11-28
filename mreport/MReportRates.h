#ifndef MARS_MReportRates
#define MARS_MReportRates

#ifndef MARS_MReport
#include "MReport.h"
#endif

class MReportRates : public MReport
{
private:
    ULong64_t fTimeStamp;       // [us] Time in microseconds, since trigger enabled or disabled
    ULong64_t fOnTimeCounter;   // [us] Effective on-time, i.e. FTM processes triggers (e.g. No FAD busy)
    UInt_t    fTriggerCounter;  //      Counter of camera trigers (events) since trigger enabled or disabled"
    Float_t   fTriggerRate;     // [Hz] Trigger rate"
    Float_t   fBoardRate[40];   // [Hz] Board rates
    Float_t   fPatchRate[160];  // [Hz] Patch rates
    Float_t   fElapsedTime;     // [s]  Time elapsed since previous report
    Float_t   fElapsedOnTime;   // [s]  On-time elapsed since previous report

    Bool_t SetupReadingFits(fits &fits);
    Int_t  InterpreteFits(const fits &fits);

public:
    MReportRates();

    ULong64_t GetTimeStamp() const      { return fTimeStamp;      }
    ULong64_t GetOnTimeCounter() const  { return fOnTimeCounter;  }
    UInt_t    GetTriggerCounter() const { return fTriggerCounter; }
    Float_t   GetTriggerRate() const    { return fTriggerRate;    }
    Float_t   GetElapsedTime() const    { return fElapsedTime;    }
    Float_t   GetElapsedOnTime() const  { return fElapsedOnTime;  }

    void Print(Option_t *o="") const;

    ClassDef(MReportRates, 1) // Class for FTM_CONTROL/TRIGGER_RATES
};

#endif
