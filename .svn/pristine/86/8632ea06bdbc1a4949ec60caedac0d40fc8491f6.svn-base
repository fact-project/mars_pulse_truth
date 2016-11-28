#ifndef MARS_MReadReports
#define MARS_MReadReports

#ifndef MARS_MRead
#include "MRead.h"
#endif

#ifndef ROOT_TArrayL
#include <TArrayL.h>
#endif

class TChain;
class MTime;
class MTaskList;
class MReadTree;

class MReadReports : public MRead
{
public:
    enum Type_t {
        kStandard,
        kMaster,
        kRequired
    };

private:
    MTaskList *fTrees;    // Hold the trees which are scheduled for reading
    TList     *fChains;   // Hold TChains to read the times in advance

    TArrayL    fPosEntry; // Store the position in each tree/chain
    TArrayL    fPosTree;  // Number of Tree in file.

    Bool_t     fEnableAutoScheme;
    Int_t      fNumRequired;
    //Bool_t     fFirstReInit;

    void    ForceRequired();
    MTime** GetTime(TChain *c) const;
    Int_t   FindNextTime();
    Int_t   FindNextRequired();
    Int_t   FindNext() { return fNumRequired==0 ? FindNextTime() : FindNextRequired(); }

    UInt_t  GetEntries();
    TString GetFullFileName() const;

    //Bool_t  ReInit(MParList *plist);
    Int_t   PreProcess(MParList *plist);
    Int_t   Process();
    Int_t   PostProcess();

    enum {
        kHasMaster   = BIT(14),
        kIsRequired  = BIT(22),
        kIsProcessed = BIT(23)
    };

public:
    MReadReports(); 
    ~MReadReports(); 

    void  AddTree(const char *tree, const char *time=NULL, Type_t master=kStandard);
    void  AddTree(const char *tree, Type_t master)
    {
        AddTree(tree, NULL, master);
    }
    Int_t AddFile(const char *fname, Int_t entries=-1);
    void  AddToBranchList(const char *name);

    void  PrintStatistics(const Int_t lvl=0, Bool_t title=kFALSE, Double_t time=0) const;

    void  EnableAutoScheme(Bool_t e=kTRUE) { fEnableAutoScheme = e; } // Must be called BEFORE AddTree!

    MReadTree *GetReader(const char *tree) const;

    ClassDef(MReadReports, 0) // Reads events and reports from a root file ordered in time
};

#endif
