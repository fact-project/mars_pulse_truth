#ifndef MARS_MJCut
#define MARS_MJCut

#ifndef MARS_MJob
#include "MJob.h"
#endif

class MTask;
class MDataSet;
class MParList;
class MHAlpha;
class MH;
class MWriteRootFile;
class MPointingPos;
class MHSrcPosCam;

class MJCut : public MJob
{
private:
    Bool_t  fStoreSummary;
    Bool_t  fStoreResult;
    Bool_t  fWriteOnly;
    Bool_t  fFullDisplay;
    Bool_t  fRndmSrcPos;
    //Bool_t  fSubstraction;

    UInt_t fNumOffSourcePos;

    TString fNameSummary;
    TString fNameResult;
    TString fNameOutput;

    TString fNameHist;
    TString fNameHistFS;

    MTask  *fCalcHadronness;
    MTask  *fCalcDisp;
    MTask  *fEstimateEnergy;

    TString  GetOutputFile(UInt_t num) const;
    Bool_t   CheckEnvLocal();
    void     SetupWriter(MWriteRootFile *write, const char *name) const;
    Bool_t   WriteTasks(UInt_t num, TObjArray &cont) const;
    Bool_t   WriteResult(const MParList &plist, const MDataSet &set) const;
    MHAlpha *CreateNewHist(MParList &plist, const char *name="") const;
    MH      *CreateNewHistFS(MParList &plist, const char *name="") const;

    Bool_t   CanStoreSummary() const { return !fPathOut.IsNull() && fStoreSummary; }
    Bool_t   CanStoreResult() const  { return !fPathOut.IsNull() && fStoreResult;  }

    Bool_t   FillSrcPosCam(const MDataSet &set, MPointingPos &source, MHSrcPosCam &srcpos);

public:
    MJCut(const char *name=NULL, const char *title=NULL);
    ~MJCut();

    Int_t Process(const MDataSet &set);

    void EnableStorageOfSummary(Bool_t b=kTRUE)  { fStoreSummary = b; } // See SetNameSummary
    void EnableStorageOfResult(Bool_t b=kTRUE)   { fStoreResult  = b; } // See SetNameResult
    void EnableWriteOnly(Bool_t b=kTRUE)         { fWriteOnly    = b; }
    void EnableFullDisplay(Bool_t b=kTRUE)       { fFullDisplay  = b; }
    void EnableRandomSrcPos(Bool_t b=kTRUE)      { fRndmSrcPos   = b; }
    //void EnableSubstraction(Bool_t b=kTRUE)      { fSubstraction = b; }

    void SetNameSummaryFile(const char *name="");
    void SetNameResultFile(const char *name="");
    void SetNameOutFile(const char *name="")      { fNameOutput=name; }

    void SetNameHist(const char *name) { fNameHist=name; }
    void SetNameHistFS(const char *name) { fNameHistFS=name; }

    void SetEnergyEstimator(const MTask *task=0);
    void SetHadronnessCalculator(const MTask *task=0);
    void SetDispCalculator(const MTask *task=0);

    ClassDef(MJCut, 0) // Standard program to perform g/h-separation cuts
};

#endif
