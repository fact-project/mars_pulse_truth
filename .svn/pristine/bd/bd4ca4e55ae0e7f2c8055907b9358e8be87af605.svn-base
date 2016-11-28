#ifndef MARS_MWriteRootFile
#define MARS_MWriteRootFile

#ifndef MARS_MWriteFile
#include "MWriteFile.h"
#endif
#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif
#ifndef ROOT_TArrayL
#include <TArrayL.h>
#endif

class TFile;
class TTree;
class TBranch;

class MRootFileBranch : public TNamed
{
private:
    TTree         *fTree;       //!
    TBranch       *fBranch;     //!

    MParContainer *fContainer;

    Bool_t         fMust;
    Long64_t       fMaxEntries;

    void Init(const char *name, Bool_t must)
    {
        SetName(name?name:"");
        fMust = must;
    }

public:
    MRootFileBranch() : fTree(NULL), fBranch(NULL), fContainer(NULL), fMust(0), fMaxEntries(0)
    {
        Init(NULL, kFALSE);
        fTitle = "";
    }

    MRootFileBranch(const char *cname, const char *tname=NULL, Bool_t must=kFALSE, Long64_t max=-1)
        : fTree(NULL), fBranch(NULL), fContainer(NULL), fMust(0), fMaxEntries(max)
    {
        Init(tname, must);
        fTitle = cname;
    }

    MRootFileBranch(MParContainer *cont, const char *tname=NULL, Bool_t must=kFALSE, Long64_t max=-1)
        : fTree(NULL), fBranch(NULL), fMust(0), fMaxEntries(max)
    {
        Init(tname, must);
        fTitle = "";
        fContainer = cont;
    }

    TTree         *GetTree() const       { return fTree; }
    MParContainer *GetContainer() const  { return fContainer; }
    void          *GetAddress()          { return &fContainer; }
    TBranch       *GetBranch() const     { return fBranch; }
    const char    *GetContName() const   { return fTitle; }
    Bool_t         MustHave() const      { return fMust; }
    Long64_t         GetMaxEntries() const { return fMaxEntries; }

    void SetContainer(MParContainer *cont) { fContainer = cont; }
    void SetTree(TTree *tree)              { fTree = tree; }
    void SetBranch(TBranch *branch)        { fBranch = branch; }

    ClassDef(MRootFileBranch, 1) // Storage container for MWriteRootFile to store TBranch informations
};

class MWriteRootFile : public MWriteFile
{
private:
    static const TString gsDefName;
    static const TString gsDefTitle;

    TFile *fOut;             // Current file

    TObjArray fBranches;     // List of Branch setup (MRootFileBranch)
    TObjArray fTrees;        //! List of trees
    TObjArray fCopies;       // Branches and tree to copy

    TString fSplitRule;      // file splitting allowed if rule existing (done in ReInit)

    enum {
        kIsNotOwner = BIT(14), // MWriteRootFile is not owner of fOut
        kFillTree   = BIT(14),
        kForced     = BIT(14), // Used for flag in fCopies
        // Be carefull these bits are already in use!
        // TBranch::kAutoDelete = BIT(15)
        // TBranchElement::kDeleteObject = BIT(16)
        // TTree::kFriendLock = BIT(17)
        kIsNewTree  = BIT(23)
    };

    // File handling
    void    Close();
    Bool_t  ChangeFile(const char *fname);
    TFile  *OpenFile(const char *name, Option_t *option, const char *title, Int_t comp);
    void    CopyTree(TTree &t) const;
    Bool_t  MakeCopies(const char *oldname) const;

    // MWrite
    Bool_t      CheckAndWrite();
    Bool_t      IsFileOpen() const;
    Bool_t      GetContainer(MParList *pList);
    const char *GetFileName() const;

    // MTask
    Bool_t ReInit(MParList *pList);
    void   StreamPrimitive(std::ostream &out) const;

    // Constructor
    void Init(const char *name=0, const char *title=0);

public:
    MWriteRootFile();
    MWriteRootFile(const Int_t comp,
                   const char *rule,
                   const Option_t *opt="RECREATE",
                   const char *ftitle="Untitled",
                   const char *name=NULL,
                   const char *title=NULL);
    MWriteRootFile(const char *fname,
                   const Option_t *opt="RECREATE",
                   const char *ftitle="Untitled",
                   const Int_t comp=2,
                   const char *name=NULL,
                   const char *title=NULL);
    ~MWriteRootFile();

    void AddContainer(const char *cname,   const char *tname=NULL, Bool_t must=kTRUE, Long64_t max=-1);
    void AddContainer(MParContainer *cont, const char *tname=NULL, Bool_t must=kTRUE, Long64_t max=-1);
    void AddCopySource(const char *tname, const char *bname=NULL, Bool_t force=kTRUE);
    void AddCopySource(const char *tname, Bool_t force)
    {
        AddCopySource(tname, NULL, force);
    }

    void AddTree(const char *name, Bool_t force=kTRUE)
    {
        AddContainer(Form("MReport%s", name), name, force);
        AddContainer(Form("MTime%s", name),   name, force);
    }

    void Print(Option_t *t=NULL) const;

    Bool_t cd(const char *path=0);

    void RecursiveRemove(TObject *obj);

    static TString SubstituteName(const char *regexp, TString fname);

    ClassDef(MWriteRootFile, 1)	// Task to write data into a root file
};

#endif
