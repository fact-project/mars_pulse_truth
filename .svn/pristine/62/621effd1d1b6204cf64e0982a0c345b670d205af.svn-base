#ifndef MARS_MReportFileRead
#define MARS_MReportFileRead

#ifndef MARS_MTask
#include "MTask.h"
#endif
#ifndef MARS_MTime
#include "MTime.h"
#endif

class THashTable;

class MTime;
class MReport;
class MReportHelp;

class MReportFileRead : public MTask
{
private:
    TString     fFileName;  // Name of the input file

    THashTable *fList;      // List of possible reports to be interpreted

    MTime   fStart;         // Time range which should be read from file
    MTime   fStop;          // Time range which should be read from file

    Long_t  fNumLine;       // line counter

    Int_t   fVersion;       // File format version

    enum { kHasNoHeader = BIT(14) };

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

    virtual Int_t CheckFileHeader() { return 0; }
    MReport *GetReport(const TString &str) const;
    MReport *GetReport(MReportHelp *help) const;
    MReportHelp *GetReportHelp(const TString &str) const;

protected:
    std::ifstream *fIn;         //! buffered input stream (file to read from)
    void SetVersion(Int_t v) { fVersion = v; }

public:
    MReportFileRead(const char *filename, const char *name=NULL, const char *title=NULL);
    ~MReportFileRead();

    void SetHasNoHeader() { SetBit(kHasNoHeader); }
    void SetTimeStart(const MTime &tm) { fStart = tm; }
    void SetTimeStop(const MTime &tm)  { fStop = tm; }

    Bool_t AddToList(const char *name) const;

    ClassDef(MReportFileRead, 0)// Task to read general report file
};

#endif
