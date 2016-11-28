#ifndef MARS_MJSimReflector
#define MARS_MJSimReflector

#ifndef MARS_MJob
#include "MJob.h"
#endif

class MHn;
class MArgs;
class MParList;
class MSequence;
class MWriteRootFile;
class MWriteFitsFile;
class MRawRunHeader;

class MJSimReflector : public MJob
{
public:
    enum Mode_t { kModeData, kModePed, kModeCal, kModePointRun };

private:
    Bool_t fForceMode;      // Force execution even if RUNE-section was not foundm

    Int_t  fOperationMode;  // Operation mode (data, ped, cal, pointrun)
    Int_t  fRunNumber;      // 8 digits run number to be used for the file names

    TString fFileOut;       // Output file name if all runs should be combines

//    Bool_t WriteResult(const MParList &plist, const MSequence &seq, Int_t run);
    Bool_t CheckEnvLocal();

public:
    MJSimReflector(const char *name=NULL, const char *title=NULL);

    void SetFileOut(const char *file="") { fFileOut = file; }
    void SetForceMode(Bool_t force=kTRUE) { fForceMode = force; }
    void SetMode(Int_t mode) { fOperationMode = mode; }
    void SetRunNumber(Int_t n=-1) { fRunNumber = n%100000000; }
    void SetupHeaderOperationMode(MRawRunHeader &header) const;

    // Process
    Bool_t Process(const MArgs &args, const MSequence &seq);

    ClassDef(MJSimReflector, 0) // Job to run the reflector simulation (reflector)
};

#endif
