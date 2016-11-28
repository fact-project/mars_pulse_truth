#ifndef MARS_MReportDAQ
#define MARS_MReportDAQ

#ifndef MARS_MReport
#include "MReport.h"
#endif

class MReportDAQ : public MReport
{
private:
    Int_t InterpreteBody(TString &str, Int_t ver);

public:
    MReportDAQ();

    ClassDef(MReportDAQ, 1) // Class for DAQ-REPORT information
};

#endif
