#ifndef MARS_MTaskInteractive
#define MARS_MTaskInteractive

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MFilter;
class MTaskList;

class MTaskInteractive : public MTask
{
private:
    TMethodCall *fCall[4];

    Int_t  (*fPreProcess)(MParList *list);
    Int_t  (*fProcess)();
    Int_t  (*fPostProcess)();
    Bool_t (*fReInit)(MParList *list);

    Int_t  PreProcess(MParList *list) { if (fCall[0]) return Return(0, &list); return fPreProcess  ? (*fPreProcess)(list) : kTRUE; }
    Int_t  Process()                  { if (fCall[1]) return Return(1);        return fProcess     ? (*fProcess)()        : kTRUE; }
    Int_t  PostProcess()              { if (fCall[2]) return Return(2);        return fPostProcess ? (*fPostProcess)()    : kTRUE; }
    Bool_t ReInit(MParList *list)     { if (fCall[3]) return Return(3, &list); return fReInit      ? (*fReInit)(list)     : kTRUE; }

    Int_t Return(Int_t no, void *param=NULL);
    Bool_t Set(void *fcn, Int_t no, const char *params);
    void   Free(Int_t no);

public:
    MTaskInteractive(const char *name=NULL, const char *title=NULL);
    ~MTaskInteractive();

    // This is to be used in compiled code
    void SetPreProcess(Int_t (*func)(MParList *list)) { fPreProcess  = func; Free(0); }
    void SetProcess(Int_t (*func)())                  { fProcess     = func; Free(1); }
    void SetPostProcess(Int_t (*func)())              { fPostProcess = func; Free(2); }
    void SetReInit(Bool_t (*func)(MParList *list))    { fReInit      = func; Free(3); }

    // This is for usage in CINT
    void SetPreProcess(void *fcn)  { Set(fcn, 0, "MParList*"); fPreProcess =0; }
    void SetProcess(void *fcn)     { Set(fcn, 1, "");          fProcess    =0; }
    void SetPostProcess(void *fcn) { Set(fcn, 2, "");          fPostProcess=0; }
    void SetReInit(void *fcn)      { Set(fcn, 3, "MParList*"); fReInit     =0; }

    ClassDef(MTaskInteractive, 0) // Interactive task
};

#endif
