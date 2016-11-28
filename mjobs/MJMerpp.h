#ifndef MARS_MJMerpp
#define MARS_MJMerpp

#ifndef MARS_MJob
#include "MJob.h"
#endif

class MReportFileReadCC;
class MWriteRootFile;
class MFilter;
class MRawFileRead;

class MJMerpp : public MJob
{
    // Merpp general
    Byte_t fCompression;
    Bool_t fUpdate;

    // Setup Raw
    Bool_t fInterleave;
    Bool_t fForceProcessing;

    // Setup Reports
    Short_t  fTelescope;

    UInt_t   fHeaderRun;
    UInt_t   fHeaderFile;

    UInt_t   fReportRun;
     Int_t   fReportFile;

    TString  fOnly;

    // Setup reports, currents
    MTime fTimeStart;
    MTime fTimeStop;

    Bool_t fAutoStartTime;
    Bool_t fAutoStopTime;

private:
    void AddTree(MReportFileReadCC &read, const TString &rep, const TString &only) const;

    void     SetupCurrents(MReportFileReadCC &read, MWriteRootFile *write) const;
    MFilter *SetupReports(MReportFileReadCC &read, MWriteRootFile *write) const;
    void     SetupRaw(MRawFileRead &read, MWriteRootFile *write) const;

    Bool_t GetTimeFromFile();
    Bool_t CheckFilePermissions();

public:
    MJMerpp(const char *name=0, const char *title=0);

    void SetCompression(Byte_t comp)   { fCompression=comp;   }
    void SetUpdate(Bool_t upd)         { fUpdate=upd;         }
    void SetInterleave(Bool_t il)      { fInterleave=il;      }
    void SetForceProcessing(Bool_t fp) { fForceProcessing=fp; }
    void SetOnly(const char *tree)     { fOnly=tree;          }
    void SetConstrainHeader(Short_t tel, Int_t run, Int_t file)
    { fTelescope=tel; fHeaderRun=run; fHeaderFile=file; }
    void SetConstrainRunRep(UInt_t run, Int_t file)
    { fReportRun=run; fReportFile=file; }
    void SetTime(const MTime &start, const MTime &stop)
    { fTimeStart=start; fTimeStop=stop; }
    void SetAutoTime(Bool_t start, Bool_t stop)
    { fAutoStartTime=start; fAutoStopTime=stop; }

    Int_t Process();
    Int_t ProcessSeq(TString fname);

    ClassDef(MJMerpp, 0) // Merging and preprocessing
};

#endif
