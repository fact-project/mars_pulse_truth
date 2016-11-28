#ifndef MARS_MReportTemperatures
#define MARS_MReportTemperatures

#ifndef MARS_MReport
#include "MReport.h"
#endif

class MReportTemperatures : public MReport
{
private:
    Float_t fTimeStamp;        // [s] FSC uptime
    Float_t fTempSensors[31];  // [deg C] Sensor compartment temperatures
    Float_t fTempCrate[8];     // [deg C] Temperatures crate 0 (back/front), 1 (back/front), 2 (back/front), 3 (back/front)"
    Float_t fTempPS[8];        // [deg C] Temperatures power supplies crate 0 (back/front), 1 (back/front), 2 (back/front), 3 (back/front)
    Float_t fTempAux[4];       // [deg C] Auxiliary power supply temperatures FTM (top/bottom), FSC (top/bottom)
    Float_t fTempBack[4];      // [deg C] FTM backpanel temperatures FTM (top/bottom), FSC (top/bottom)
    Float_t fTempEthernet[4];  // [deg C] Ethernet switches temperatures top (front/back), bottom (front/back)

    Bool_t SetupReadingFits(fits &fits);
    Int_t  InterpreteFits(const fits &fits);

public:
    MReportTemperatures();

    Float_t GetTimeStamp() const { return fTimeStamp; }
    Float_t GetTempMean() const;
    Float_t GetTempMedian() const;

    void Print(Option_t *o="") const;

    ClassDef(MReportTemperatures, 1) // Class for FTM_CONTROL/TRIGGER_RATES
};

#endif
