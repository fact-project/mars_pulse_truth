#ifndef MARS_MMcPedestalCopy
#define MARS_MMcPedestalCopy

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MMcPedestalCopy : public MTask
{
private:
    Bool_t CheckRunType(MParList *pList) const;
    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);

public:
    MMcPedestalCopy(const char *name=NULL, const char *title=NULL);

    ClassDef(MMcPedestalCopy, 0)   // Task which copies the pedestals from the MC into the standard container
};

#endif
