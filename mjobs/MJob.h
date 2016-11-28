#ifndef MARS_MJob
#define MARS_MJob

#ifndef MARS_MSequence
#include "MSequence.h"
#endif

class MEnv;
class MEvtLoop;

class MJob : public MParContainer
{
private:
    enum { kIsOwner = BIT(14) };

    // Helper functions for paths
    static TString CombinePath(TString path, TString fname);
    static void FixPath(TString &path);

    // Helper functions to set a new fEnv
    void SetPrefix(const char *prefix);
    void ClearEnv();

    // Data members for resource files
    const MEnv *fEnv;         // Resource file
    TString     fEnvPrefix;   // Prefix for resources
    Int_t       fEnvDebug;    // Debug setup of resources

protected:
    TString   fPathOut;       // Directory to write output file to (eg. calib*.root)
    TString   fPathIn;        // Directory to read output from (eg. calib*.root)

    Bool_t    fOverwrite;     // Allow overwriting output files
    Int_t     fMaxEvents;     // Maximum number of events

    Bool_t    fNullOut;       // Suppress output files

    // FIXME: Split into MJobSequence and MJobDataSet
    MSequence fSequence;      // Sequence

    TString   fCommandLine;   // Command line for possible storage

    const TEnv *GetEnv() const;
    const MEnv *GetMEnv() const { return fEnv; }
    const TString &GetEnvPrefix() const { return fEnvPrefix; }
    Int_t GetEnvDebug() const { return fEnvDebug; }

    Int_t       GetEnv(const char *name, Int_t dflt) const;
    Double_t    GetEnv(const char *name, Double_t dflt) const;
    const char *GetEnv(const char *name, const char *dflt) const;
    Bool_t      HasEnv(const char *name) const;

    Bool_t InitRandomNumberGenerator() const;
    Bool_t SetupEnv(MEvtLoop &loop) const;
    Bool_t CheckEnv();
    Bool_t CheckEnv(MParContainer &c) const;
    void PrintUntouchedEnv() const;
    virtual Bool_t CheckEnvLocal() { return kTRUE; }

    Bool_t WriteContainer(TCollection &list) const;
    Bool_t ReadContainer(TCollection &list) const;
    Bool_t WriteContainer(TCollection &cont, const char *name, const char *option="RECREATE", const int compr=2) const;
    Bool_t WriteDisplay(const char *name, const char *option="RECREATE", const int comp=2) const;

public:
    MJob(const char *name=NULL, const char *title=NULL);
    ~MJob();

    // Setter
    void   SetPathOut(const char *path=".");
    void   SetPathIn(const char *path=".");

    void   SetOverwrite(Bool_t b=kTRUE) { fOverwrite=b; }
    Bool_t SetEnv(const char *env, const char *prefix=0);
    void   SetEnv(MEnv *env, const char *prefix=0);
    void   SetEnvDebug(Int_t d=2) { fEnvDebug=d; }

    void   SetMaxEvents(Int_t max) { fMaxEvents = max; }
    void   SetSequence(const MSequence &seq) { fSequence = seq; }

    void   SetNullOut(Bool_t b=kTRUE) { fNullOut=b; }

    void   SetCommandLine(const TString &s) { fCommandLine = s; }

    // Getter
    TString GetPathOut() const  { return fPathOut; }
    TString GetPathIn() const   { return fPathIn; }

    Bool_t HasWritePermission(TString fname) const;

    Bool_t HasNullOut() const { return fNullOut; }

    // Others
    MStatusDisplay *GetDisplay() { return fDisplay; }

    static TString ExpandPath(TString fname);
    static void SortArray(TArrayI &arr);

    static TString Esc(TString esc) { esc.ReplaceAll("/", "\\/"); return esc; }

    ClassDef(MJob, 0) // Bas class for Jobs
};

#endif
