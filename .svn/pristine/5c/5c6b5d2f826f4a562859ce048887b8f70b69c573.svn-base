#ifndef MARS_MChain
#define MARS_MChain

#ifndef ROOT_TChain
#include <TChain.h>
#endif

class MChain : public TChain
{
public:
    // Taken from TChain::LoadTree --- may have to be updated for different root versions
    enum {
        kCannotAccessTree = -4,  // Tree not found in file
        kCannotAccessFile = -3,  // File access impossible
        kOutOfRange       = -2,  // Event doesn't exist
        kNoError          =  0,  // LoadTree succeeded
        kFatalError       =  1   // LoadTree succeeded, but Notify() returned fatal error
    };

private:
    Bool_t   fNotified;
    Long64_t fLastError; // <0: LoadTree failed, =0: LoadTree was ok, >0: Notify failed

public:
    MChain() : TChain(), fNotified(kFALSE), fLastError(0) {}
    MChain(const char *name, const char *title="") : TChain(name, title), fNotified(kFALSE), fLastError(0) {}

    // Restart from scratch
    void ResetTree() { fTree = 0; fTreeNumber = -1; }

    // Function to be notified and setting the notifier
    virtual Bool_t Notify() { fNotified = kTRUE; return kTRUE; }
    virtual void   SetNotify(TObject *obj) { fNotify = obj; fNotified = kFALSE; }

    // Overwrite LoadTree (for different root versions)
    Int_t    LoadTree(Int_t entry) { return (Int_t)LoadTree((Long64_t)entry); }
    Long64_t LoadTree(Long64_t entry);

    // Handling for last error of LoadTree
    Long64_t GetLastError() const { return fLastError; };
    Bool_t   HasFatalError() const { return fLastError==kFatalError; }
    Bool_t   HasError() const { return fLastError==0 ? kFALSE : kTRUE; }

    ClassDef(MChain, 1) // Class derived from TChain to give access to Notify-return value
};

#endif
