#ifndef MARS_MRead
#define MARS_MRead

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MFilter;
class MDirIter;

class MRead : public MTask
{
private:
    MFilter *fSelector;

public:
    MRead() : fSelector(NULL) {}

    virtual UInt_t  GetEntries() = 0;
    virtual TString GetFileName() const;
    virtual TString GetFullFileName() const = 0;
    virtual Bool_t  Rewind();

    static Byte_t IsFileValid(const char *name);

    void SetSelector(MFilter *f) { fSelector = f; }
    MFilter *GetSelector() { return fSelector; }

    virtual Int_t AddFile(const char */*fname*/, Int_t /*entries*/=-1) { return -1; }
    Int_t AddFiles(MDirIter &dir);

    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    ClassDef(MRead, 0)	// Base class for a reading task
};

#endif
