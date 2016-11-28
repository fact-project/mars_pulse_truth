#ifndef MARS_MSequence
#define MARS_MSequence

#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif

#ifndef MARS_MTime
#include "MTime.h"
#endif

class MDirIter;

class MSequence : public MParContainer
{
    friend class MSequenceSQL;

public:
    enum LightCondition_t { kNA, kNoMoon, kTwilight, kMoon, kDay };

    enum FileExt_t {
        kRaw    =BIT(0),
        kRoot   =BIT(1),
        kReport =BIT(2),
        kCer    =BIT(3),  // Corsika files
        kFits   =BIT(4),
        kDrs    =BIT(5),
        kFact   =BIT(6),
    };
    enum RunType_t {
        kPed    =BIT(7),  // Don't use overlapping bits!
        kCal    =BIT(8),
        kDat    =BIT(9),
        kExcl   =BIT(10),
        kAll    =kPed|kDat|kCal,
        kImage  =BIT(11),
        kCalib  =BIT(12),
        kDrsRun =BIT(13),
        kDrsFile=BIT(14),
    };

    enum { kRunType   = kPed|kCal|kDat|kImage|kCalib|kDrsRun|kDrsFile };
    enum { kFileType  = kRaw|kRoot|kReport|kCer|kDrs|kFits|kFact };
    enum { kFilePaths = kFileType|kImage|kCalib };

    enum FileType_t {
        kRawDat     = kRaw   |kDat,
        kRawPed     = kRaw   |kPed,
        kRawCal     = kRaw   |kCal,
        kFitsPed    = kFits  |kPed,
        kFitsCal    = kFits  |kCal,
        kFitsDat    = kFits  |kDat,
        kFitsDrs    = kFits  |kDrsFile,
        kFitsDrsRuns= kFits  |kDrsRun,
        kFactCal    = kFits  |kCalib,
        kFactImg    = kFits  |kImage,
        kRawDrs     = kDrs   |kDrsFile,
        kRawAll     = kRaw   |kAll,
        kRootDat    = kRoot  |kDat,
        kRootPed    = kRoot  |kPed,
        kRootCal    = kRoot  |kCal,
        kRootAll    = kRoot  |kAll,
        kReportPed  = kReport|kPed,
        kReportCal  = kReport|kCal,
        kReportDat  = kReport|kDat,
        kReportAll  = kReport|kAll,
        kCalibrated = kRoot  |kCalib,
        kImages     = kRoot  |kImage,
        kCorsika    = kCer   |kDat,
    };

private:
    TString fFileName;         // Expanded file name
    TString fDataPath;         // Path to data files

    UShort_t fTelescope;       // Telescope number
    UInt_t   fSequence;        // Sequence number

    MTime  fStart;             // Start time of sequence

    UInt_t fLastRun;           // Last run in sequence (necessary?)
    UInt_t fNumEvents;         // Number of events in sequence

    UInt_t fPeriod;            // Observation period of the sequence
    MTime  fNight;             // Night (day of sunrise) of the sequence

    UInt_t fDrsSequence;       // Corresponding DRS sequence

    LightCondition_t fLightCondition;

    TString fProject;
    TString fSource;
    TString fTriggerTable;
    TString fHvSettings;
    TString fComment;

    TArrayI fRuns;            // Run numbers
    TArrayI fRunsSub;         // Sub runs (files)

    TArrayI fCalRuns;         // Numbers of calibration runs/files
    TArrayI fCalRunsSub;      // Sub runs (files) of calibration runs

    TArrayI fPedRuns;         // Numbers of pedestal runs/files
    TArrayI fPedRunsSub;      // Sub runs (files) of pedestal runs

    TArrayI fDatRuns;         // Numbers of data runs/files
    TArrayI fDatRunsSub;      // Sub runs (files) of data runs

    TArrayI fDrsRuns;         // Numbers of drs pedestal runs/files
    TArrayI fDrsRunsSub;      // Sub runs (files) of drs pedestal runs

    TArrayI fDrsFiles;        // Runs for which DRS files exist
    TArrayI fDrsFilesSub;     // Sub runs (files) of DRS files runs

    TArrayI fExclRuns;        // Numbers of excluded runs/files
    TArrayI fExclRunsSub;     // Sub runs (files) of excluded runs

    Bool_t fMonteCarlo;       // Flag for Monte Carlo sequences

    // Helper for interpretation
    void    AddEntry(Int_t run, Int_t file, TArrayI &data, TArrayI &sub) const;
    void    EvalEntry(const TEnv *env, const TString &prefix, const TString &num, TArrayI &data, TArrayI &sub) const;

    void    Split(TString &runs, TArrayI &data, TArrayI &sub, const TEnv *env=0, const TString prefix="") const;
    void    Split(const TEnv &env, const TString &prefix, const char *name, TArrayI &data, TArrayI &sub) const;

    LightCondition_t GetLightCondition(const TString &str) const;

    // Helper for file setup
    void    GetArrays(const TArrayI* &r, const TArrayI* &f, Int_t type) const;
    Bool_t  FindPath(TString &path, FileType_t type) const;
    TString FindPath(FileType_t type, TString path="") const { FindPath(path, type); return path; }
    TString FindFile(const TString &d, const TString &f) const;
    TString GetPathName(TString d, FileType_t type) const;
    TString GetFileRegexp(UInt_t num, const TArrayI &arr, const TArrayI &sub, FileType_t type) const;
    TString GetFileName(UInt_t i, const TArrayI &arr, const TArrayI &sub, FileType_t type, const char *path) const;
    UInt_t  GetRuns(MDirIter &iter, const TArrayI &arr, const TArrayI &file, FileType_t type, TString path="") const;

    // Helper for Print()
    TString GetNumSequence(Int_t pos, Int_t n, const TArrayI &f) const;
    TString GetNumSequence(Int_t &pos, const TArrayI &n, const TArrayI &f) const;

    void    PrintRunsClassic(std::ostream &out, const char *pre, const char *name, const TArrayI &r) const;
    TString PrintRuns(std::ostream &out, const char *pre, const char *name, const TArrayI &r, const TArrayI &f) const;

    // General helper
    Bool_t  HasSubRuns() const { return fRunsSub.GetSize()!=0 || fDatRunsSub.GetSize()!=0 || fPedRunsSub.GetSize()!=0 || fCalRunsSub.GetSize()!=0 || fDrsRunsSub.GetSize()!=0 || fDrsFilesSub.GetSize()!=0 || fExclRunsSub.GetSize()!=0; }
    Bool_t  IsContained(const TArrayI &arr, const TArrayI &sub, UInt_t num, UInt_t file) const;

    // Some helpers to handle the arrays
    static Int_t SortArraySub(Int_t p, Int_t n, Int_t *arr1, Int_t *arr2);
    static void  SortArrays(TArrayI &arr1, TArrayI &arr2);
    static Int_t GetSubArray(Int_t p, Int_t n, Int_t *arr1);

public:
/*
    MSequence() : fTelescope(1), fSequence((UInt_t)-1), fLastRun((UInt_t)-1),
        fNumEvents((UInt_t)-1), fPeriod((UInt_t)-1), fLightCondition(kNA), fMonteCarlo(kFALSE)
    {
        fName  = "MSequence";
        fTitle = "Sequence file";
    }
    */
    MSequence(const char *fname=0, const char *path=0, UInt_t id=(UInt_t)-1);
    MSequence(const char *fname, UInt_t seq, UShort_t tel=0);
    MSequence(const MSequence &s) : MParContainer(s),
        fFileName(s.fFileName), fDataPath(s.fDataPath),
        fTelescope(s.fTelescope),
        fSequence(s.fSequence), fStart(s.fStart),
        fLastRun(s.fLastRun), fNumEvents(s.fNumEvents), fPeriod(s.fPeriod),
        fNight(s.fNight), fLightCondition(s.fLightCondition),
        fProject(s.fProject), fSource(s.fSource),
        fTriggerTable(s.fTriggerTable), fHvSettings(s.fHvSettings),
        fRuns(s.fRuns), fRunsSub(s.fRunsSub), fCalRuns(s.fCalRuns),
        fCalRunsSub(s.fCalRunsSub), fPedRuns(s.fPedRuns),
        fPedRunsSub(s.fPedRunsSub), fDatRuns(s.fDatRuns),
        fDatRunsSub(s.fDatRunsSub), fDrsRuns(s.fDrsRuns),
        fDrsRunsSub(s.fDrsRunsSub), fDrsFiles(s.fDrsFiles),
        fDrsFilesSub(s.fDrsFilesSub), fExclRuns(s.fExclRuns),
        fExclRunsSub(s.fExclRunsSub), fMonteCarlo(s.fMonteCarlo) { }

    Bool_t IsCompatible(const MSequence &s) const;
    Bool_t IsIdentical(const MSequence &s) const;
    Bool_t operator==(const MSequence &s) const;

    // I/O
    Int_t  Read(const char *name=0);
    Bool_t ReadDatabase(TString sql, UInt_t seq=0, UShort_t tel=0);
    Bool_t ReadFile(const char *filename, UInt_t id=(UInt_t)-1);
    Bool_t WriteFile(const char *filename, const Option_t *o) const;
    Bool_t WriteFile(const char *filename) const { return WriteFile(filename,""); } //*MENU *ARGS={filename=>fBaseName}
    Bool_t SetDataPath(const char *path="");
    void   SetDrsSequence(UInt_t seq=0) { fDrsSequence = seq; }

    // TObject
    void Print(std::ostream &out, Option_t *o) const;
    void Print(Option_t *o) const;
    void Print() const { Print(""); } //*MENU*

    // Genaral interface
    Bool_t IsValid() const { return fTelescope>0 && fSequence!=(UInt_t)-1; }
    Bool_t IsMonteCarlo() const { return fMonteCarlo; }
    Bool_t IsExcluded(UInt_t run, UInt_t file) const { return IsContained(fExclRuns, fExclRunsSub, run, file); }
    Bool_t IsContained(UInt_t run, UInt_t file) const { return IsContained(fCalRuns, fCalRunsSub, run, file) || IsContained(fPedRuns, fPedRunsSub, run, file) || IsContained(fDatRuns, fDatRunsSub, run, file) || IsContained(fDrsRuns, fDrsRunsSub, run, file); }

    // Setter
    void SetNight(const char*night);

    void AddRun(UInt_t run, RunType_t type=kAll) { AddFile(run, 0, type); }
    void AddRuns(UInt_t run1, UInt_t run2, RunType_t type=kAll) { for (UInt_t i=run1; i<=run2; i++) AddFile(i, 0, type); }
    void AddFile(UInt_t run, UInt_t file, RunType_t type=kAll);
    void AddFiles(UInt_t run, UInt_t f1, UInt_t f2, RunType_t type=kAll) { for (UInt_t i=f1; i<=f2; i++) AddFile(run, i, type); }

    void ExcludeFile(UInt_t run, UInt_t file=0) { /*if (!force || !IsExcluded(run, file))*/ AddFile(run, file, kExcl); }
    void ExcludeRuns(TString runs);

    void SetMonteCarlo(Bool_t ismc=kTRUE) { fMonteCarlo=ismc; }

    // Getter
    UInt_t GetNumExclRuns() const { return fExclRuns.GetSize(); }

    UShort_t GetTelescope() const { return fTelescope; }
    UInt_t GetSequence() const { return fSequence; }
    UInt_t GetLastRun() const  { return fLastRun; }
    UInt_t GetPeriod() const   { return fPeriod; }
    UInt_t GetDrsSequence() const { return fDrsSequence; }
    UInt_t GetDrsFile() const;
    Bool_t HasMoon() const { return fLightCondition==kMoon; }

    Int_t  GetFile(UInt_t i, RunType_t type, UInt_t &run, UInt_t &file) const;

    LightCondition_t GetLightCondition() const { return fLightCondition; }

    const MTime   &GetStart() const { return fStart; }
    const MTime   &GetNight() const { return fNight; }
    const TString &GetSource() const { return fSource; } 

    const TString GetExcludedRuns() const;

    // Filesystem interface
    UInt_t  GetRuns(MDirIter &iter, FileType_t type, const char *path=0) const;
    TString GetFileRegexp(UInt_t num, FileType_t type) const;
    TString GetFileName(UInt_t i, FileType_t type, const char *path=0) const;
    UInt_t  GetNumEntries(RunType_t type=kAll) const;

    // Filesystem getter
    const char    *GetBaseName() const;
    const char    *GetFilePath() const;
    const TString &GetFileName() const { return fFileName; }
    const TString &GetDataPath() const { return fDataPath; }

    const TString  GetStandardPath() const { return GetStandardPath(fMonteCarlo); }

    // Static function for external access
    //static TString GetStandardPath(Bool_t mc) { return mc?"/magic/montecarlo/":"/data00/fact-construction/"; }
    static TString GetStandardPath(Bool_t mc) { return mc?"/magic/montecarlo/":"/fact/"; }
    static Bool_t  InflateSeq(TString &seq, /*FIXME: TELNUM*/ Bool_t ismc=kFALSE);

    static TString InflateFileName(const MTime &night, UShort_t tel, Int_t run, Int_t file, Int_t type=kRawAll);
    static TString InflateFileName(const MTime &night, Int_t run, Int_t type=kRawAll) { return InflateFileName(night, 1, run, 0, type); }

    static TString InflateFilePath(const MTime &night, UShort_t tel, Int_t run, Int_t file=0, Int_t type=kRawAll);
    static TString InflateFilePath(const MTime &night, Int_t run, Int_t type=kRawAll) { return InflateFilePath(night, 1, run, 0, type); }

    static TString InflateFilePath(const MTime &night, Bool_t mc=kFALSE);

    ClassDef(MSequence, 7) // Describes a sequences, reads and writes sequence files
};

#endif
