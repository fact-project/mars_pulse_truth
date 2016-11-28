#ifndef MARS_MReadTree
#define MARS_MReadTree

#ifndef MARS_MRead
#include "MRead.h"
#endif

class MChain;
class TChain;
class TChainElement;
class TBranch;
class TTree;
class MTaskList;

class MReadTree : public MRead
{
    friend class MReadReports;

private:
    MChain *fChain;            // Pointer to tree==fTree (only if fTree inherits from MChain)
    TTree  *fTree;             // Pointer to tree

    UInt_t  fNumEntry;         // Number of actual entry in chain
    UInt_t  fNumEntries;       // Number of Events in chain

    Bool_t  fBranchChoosing;   // Flag for branch choosing method
    Bool_t  fAutoEnable;       // Flag for auto enabeling scheme

    TList  *fVetoList;         //-> List of Branches which are not allowed to get enabled
    TList  *fNotify;           //-> List of TObjects to notify when switching files

    MTaskList *fTaskList;      // Tasklist to set StreamId
    TList      fParList;

    enum { kChainWasChanged = BIT(14) };
    // MReadReports::kIsRequired  = BIT(22)
    // MReadReports::kIsProcessed = BIT(23)

private:
    void SetBranchStatus(const TList *list, Bool_t status);
    void SetBranchStatus(TObject *branch, Bool_t status);
    void SetBranchStatus(const char *name, Bool_t status);

    Bool_t IsOwned(const TChainElement &e) const;

    void GetListOfBranches(TList &list) const;
    MParContainer *FindCreateObj(MParList &plist, const char *cname, const char *name);
    void *GetParameterPtr(MParList &plist, const TString &bb, const char *parname);
    Bool_t SetBranchAddress(TBranch &b, void *ptr, const char *prefix, const char *type=0);

    void DisableSubBranches(TBranch *b);
    void EnableBranches(MParList *plist);
    void EnableBranchChoosing();

    Bool_t CheckBranchSize();

    virtual void SetReadyToSave(Bool_t flag=kTRUE);
    virtual void StreamPrimitive(std::ostream &out) const;

    enum { kIsOwner = BIT(14) };
    // MReadReports::kIsRequired  = BIT(22)
    // MReadReports::kIsProcessed = BIT(23)

public:
    MReadTree(TTree *tree=0);
    MReadTree(const char *treename, const char *filename=NULL, const char *name=NULL, const char *title=NULL);
    ~MReadTree();

    static Byte_t IsFileValid(const char *name);

    virtual void SortFiles();

    void   DisableAutoScheme() { fAutoEnable = kFALSE; }
    void   EnableBranch(const char *name);
    void   VetoBranch(const char *name);

    Bool_t GetEvent();

    Bool_t DecEventNum(UInt_t dec=1); // decrease number of event (position in tree)
    Bool_t IncEventNum(UInt_t inc=1); // increase number of event (position in tree)
    Bool_t SetEventNum(UInt_t nr);    // set number of event (position in tree)

    UInt_t GetNumEntry() const { return fNumEntry; }
    UInt_t GetEntries();

    //TString GetFileName() const;
    TString GetFullFileName() const;
    Int_t   GetFileIndex() const;
    Int_t   GetNumFiles() const;

    virtual void AddNotify(TObject *obj);
    virtual void SetOwner(Bool_t flag=kTRUE);

    virtual Int_t AddFile(const char *fname, Int_t entries=-1);
    virtual Int_t AddFiles(const MReadTree &read);
    Int_t AddFiles(MDirIter &dir) { return MRead::AddFiles(dir); }
    Bool_t AddFriend(TChain &c);

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

    Bool_t Notify();
    Bool_t Rewind() { SetEventNum(0); return kTRUE; }
    void   Print(Option_t *opt="") const;

    ClassDef(MReadTree, 1)	// Reads a tree from file(s)
};

#endif
