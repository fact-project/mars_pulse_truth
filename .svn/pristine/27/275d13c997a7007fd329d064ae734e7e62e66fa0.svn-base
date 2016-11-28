#ifndef MARS_MPrint
#define MARS_MPrint

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;

class MPrint : public MTask
{
private:
    const TObject *fObject;  // pointer to container which has to be printed
    TString fObjName;        // given name to search for in the parameterlist
    TString fOption;         // Print option

    enum { kSkip = BIT(14), kSeparator = BIT(15) };

    void Init(const char *name, const char *title);

public:
    MPrint();
    MPrint(const char *obj,    const char *option="", const char *name=NULL, const char *title=NULL);
    MPrint(const TObject *obj, const char *option="", const char *name=NULL, const char *title=NULL);

    void SetOption(Option_t *option) { fOption = option; }
    void EnableSkip(Bool_t skip=kTRUE) { skip ? SetBit(kSkip) : ResetBit(kSkip); }

    Int_t PreProcess(MParList *pList);
    Int_t Process();

    ClassDef(MPrint, 0) // Task to call Print() function
};
    
#endif

