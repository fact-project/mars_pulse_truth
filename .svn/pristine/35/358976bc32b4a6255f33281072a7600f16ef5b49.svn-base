#ifndef MARS_MTaskEnv
#define MARS_MTaskEnv

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MFilter;
class MTaskList;

class MTaskEnv : public MTask
{
private:
    MTask *fTask;

    enum { kIsOwner = BIT(14), kIsDummy = BIT(15) };

    MTask *GetTask(const char *name) const;

    Bool_t ReInit(MParList *list);

    Int_t  PreProcess(MParList *list);
    Int_t  Process();
    Int_t  PostProcess();

    Bool_t WriteEnv(TEnv &env, TString prefix, Bool_t print=kFALSE) const;

public:
    MTaskEnv(const char *name=NULL, const char *title=NULL);
    ~MTaskEnv();

    void SetOwner(Bool_t b=kTRUE) { b ? SetBit(kIsOwner) : ResetBit(kIsOwner); }

    void SetDefault(MTask *task=0) { fTask = task; if (fTask) { fTask->SetName(fName); ResetBit(kIsDummy); } else SetBit(kIsDummy); }
    void SetDefault(const char *def);

    MTask *GetTask() { return fTask; }

    Int_t  ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    ClassDef(MTaskEnv, 0) // Task which can be setup from an environment file
};

#endif
