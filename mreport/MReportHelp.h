#ifndef MARS_MReportHelp
#define MARS_MReportHelp

#ifndef ROOT_TObject
#include <TObject.h>
#endif

class TString;

class MLog;
class MTime;
class MReport;
class MParList;

class MReportHelp : public TObject
{
private:
    MReport *fReport;
    ULong_t  fNumReports;
    ULong_t  fNumSkipped;

public:
    MReportHelp(const char *name, MLog *fLog);
    ~MReportHelp();

    const char *GetName() const;
    ULong_t GetNumReports() const { return fNumReports; }
    ULong_t GetNumSkipped() const { return fNumSkipped; }
    ULong_t Hash() const;
    MReport *GetReport() { return fReport; }

    Int_t Interprete(TString &str, const MTime &start, const MTime &stop, Int_t ver);

    Bool_t SetupReading(MParList &plist);
    
    void AddToList(MParList &plist);

    ClassDef(MReportHelp, 0) // Wrapper class for MReport to speed up finding the correct MReport-class
};

#endif
