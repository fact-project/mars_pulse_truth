#ifndef MARS_MJSimulation
#define MARS_MJSimulation

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

class MJSimulation : public MJob
{
public:
    enum Mode_t { kModeData, kModePed, kModeCal, kModePointRun };

private:
    Bool_t fForceMode;      // Force execution even if RUNE-section was not foundm

    Bool_t fCamera;         // Switch on/off camera (for fast reflector processing)
    Bool_t fForceTrigger;   // Force the use of the trigger "electronics"
    Bool_t fWriteFitsFile;

    Int_t  fOperationMode;  // Operation mode (data, ped, cal, pointrun)
    Int_t  fRunNumber;      // 8 digits run number to be used for the file names

    TString fFileOut;       // Output file name if all runs should be combines

    Bool_t WriteResult(const MParList &plist, const MSequence &seq, Int_t run);
    Bool_t CheckEnvLocal();

    template<class T>
    void SetupCommonFileStructure(T &write) const;

    void SetupHist(MHn &hist) const;
    void SetupHeaderKeys(MWriteFitsFile& write, MRawRunHeader &header) const;
    void SetupVetoColumns(MWriteFitsFile& write) const;
    void SetupHeaderOperationMode(MRawRunHeader &header) const;
    void CreateBinningObjects(MParList &plist) const;

public:
    MJSimulation(const char *name=NULL, const char *title=NULL);

    void SetForceMode(Bool_t force=kTRUE) { fForceMode = force; }
    void SetMode(Int_t mode) { fOperationMode = mode; }
    void SetWriteFitsFile(Bool_t fits) { fWriteFitsFile = fits; }
    void SetFileOut(const char *file="") { fFileOut = file; }

    void SetRunNumber(Int_t n=-1) { fRunNumber = n%100000000; }

    // Process
    Bool_t Process(const MArgs &args, const MSequence &seq);

    ClassDef(MJSimulation, 0) // Job to run the simulation (ceres)
};

#endif
