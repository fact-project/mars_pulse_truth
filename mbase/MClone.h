#ifndef MARS_MClone
#define MARS_MClone

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;

class MClone : public MTask
{
private:
    const TObject *fObject; // pointer to container which has to be cloned
    TString fObjName;       // given name to search for in the parameterlist

    TObject* fClone;        // pointer to the cloned object. deletion is handled by MClone

    Bool_t fForceClone;

    // MClone
    void Init(const char *name, const char *title);

    // MTask
    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:
    MClone(const char *obj,    const char *name=NULL, const char *title=NULL);
    MClone(const TObject *obj, const char *name=NULL, const char *title=NULL);
    ~MClone();

    // TObject
    void Clear(Option_t *opt=NULL);

    // MClone
    TObject *GetClone() const  { return fClone; }
    const TObject *GetObject() const { return fObject; }

    void SetForceClone(Bool_t b=kTRUE) { fForceClone = b; }

    ClassDef(MClone, 0) // Task to clone (duplicate) an object in memory
};
    
#endif

