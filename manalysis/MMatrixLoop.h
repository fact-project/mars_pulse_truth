#ifndef MARS_MMatrixLoop
#define MARS_MMatrixLoop

#ifndef MARS_MRead
#include "MRead.h"
#endif

class MHMatrix;

class MMatrixLoop : public MRead
{
public:
    enum OperationMode_t {
        kDefault,
        kEven,
        kOdd
    };
private:
    // MMatrixLoop
    static const TString gsDefName;
    static const TString gsDefTitle;

    MHMatrix *fMatrix;
    Int_t     fNumRow;    //! Number of dimensions of histogram

    Byte_t fOperationMode;

    // MRead
    UInt_t  GetEntries();
    TString GetFullFileName() const;
    Bool_t  Rewind() { fNumRow=0; return kTRUE; }

    virtual Int_t AddFile(const char *fname, Int_t entries=-1) { return 0; }
    Int_t AddFiles(MDirIter &dir) { return 0; }

    // MTask
    Int_t PreProcess(MParList *plist);
    Int_t Process();

public:
    MMatrixLoop(MHMatrix *mat, const char *name=NULL, const char *title=NULL);

    void SetOperationMode(OperationMode_t mode) { fOperationMode = mode; }

    ClassDef(MMatrixLoop, 0) // Task 'reading' events from a MHMatrix
};

#endif
