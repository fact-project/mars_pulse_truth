#ifndef MARS_MParEnv
#define MARS_MParEnv

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MParEnv : public MParContainer
{
private:
    MParContainer *fCont;  // Pointer to the paremetr container

    TString fClassName;    // Name of the Class fCont must inherit from
    Bool_t  fIsOwner;      // Flag whether MParEnv owns fCont

    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

public:
    MParEnv(const char *name=NULL, const char *title=NULL);
    ~MParEnv();

    void SetCont(MParContainer *c=0);
    MParContainer *GetCont() const { return fCont; }

    Bool_t SetClassName(const char *cls="MParContainer");

    void Print(Option_t *o="") const;

    ClassDef(MParEnv, 1) // A wrapper container which allows to set a new container from the resource file
};

#endif
