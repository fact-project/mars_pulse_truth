#ifndef MARS_MReportPyrometer
#define MARS_MReportPyrometer

#ifndef MARS_MReport
#include "MReport.h"
#endif

class MReportPyrometer : public MReport
{
private:
    Float_t fTempSky;     // [K] Temperature Sky
    Float_t fTempAir;     // [K] Tempareture Air
    Float_t fCloudiness;  // [%] Cloudiness

    Bool_t  fLidOpen;     //     Lid Status

    Int_t InterpreteBody(TString &str, Int_t ver);

public:
    MReportPyrometer();

    Float_t GetTempSky() const { return fTempSky; }
    Float_t GetTempAir() const { return fTempAir; }

    Float_t GetCloudiness() const { return fCloudiness; }

    Bool_t IsLidOpen() const { return fLidOpen; }

    ClassDef(MReportPyrometer, 1) // Class for PYRO-REPORT information
};

#endif
