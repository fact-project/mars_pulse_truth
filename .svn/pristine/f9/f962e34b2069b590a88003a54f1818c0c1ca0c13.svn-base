#ifndef MARS_MReportCurrents
#define MARS_MReportCurrents

#ifndef MARS_MReport
#include "MReport.h"
#endif

class MCameraDC;

class MReportCurrents : public MReport
{
private:
    Byte_t fStatus1;
    Byte_t fStatus2;

    MCameraDC *fDC; //!

    Bool_t SetupReading(MParList &plist);
    Int_t InterpreteBody(TString &str, Int_t ver);

public:
    MReportCurrents();

    ClassDef(MReportCurrents, 1) // Class for DC-REPORT information
};

#endif
