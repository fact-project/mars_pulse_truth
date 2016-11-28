#ifndef MARS_MReportRun
#define MARS_MReportRun

#ifndef MARS_MReport
#include "MReport.h"
#endif

class MReportRun : public MReport
{
private:
    Int_t   fRunNumber;
    Int_t   fFileNumber;
    TString fSourceName;

    Int_t InterpreteBody(TString &str, Int_t ver);

public:
    MReportRun();

    const TString &GetSourceName() const { return fSourceName; }
    TString GetSourceName() { return fSourceName; }

    Int_t GetRunNumber() const  { return fRunNumber; }
    Int_t GetFileNumber() const { return fFileNumber; }

    ClassDef(MReportRun, 0) // Class for RUN-REPORT information
};

#endif
