#ifndef MARS_MReportHumidity
#define MARS_MReportHumidity

#ifndef MARS_MReport
#include "MReport.h"
#endif

class MReportHumidity : public MReport
{
private:
    Float_t fTimeStamp;   // [s] FSC uptime
    Float_t fHumidity[4]; // H[%] Humidity sensors readout

    Bool_t SetupReadingFits(fits &fits);
    Int_t  InterpreteFits(const fits &fits);

public:
    MReportHumidity();

    Float_t GetTimeStamp() const { return fTimeStamp; }
    Float_t GetMean() const;

    void Print(Option_t *o="") const;

    ClassDef(MReportHumidity, 1) // Class for FTM_CONTROL/TRIGGER_RATES
};

#endif
