#ifndef MARS_MReportFileReadCC
#define MARS_MReportFileReadCC

#ifndef MARS_MReportFileRead
#include "MReportFileRead.h"
#endif

class MReportFileReadCC : public MReportFileRead
{
private:
    Int_t fTelescope;
    Int_t fRunNumber;
    Int_t fFileNumber;

    Int_t GetRunNumber(const TString &str) const;
    Int_t GetVersion(const TString &str) const;
    Int_t GetTelescope(const TString &str) const;
    Int_t GetFileNumber(const TString &str) const;

    Int_t CheckFileHeader();

public:
    MReportFileReadCC(const char *filename, const char *name=NULL, const char *title=NULL);

    void SetTelescope(Int_t num)  { fTelescope  = num; }
    void SetRunNumber(Int_t run)  { fRunNumber  = run; }
    void SetFileNumber(Int_t run) { fFileNumber = run; }

    ClassDef(MReportFileReadCC, 0)// Task to read the central control report file
};

#endif
