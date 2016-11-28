#ifndef MARS_MMcPedestalNSBAdd
#define MARS_MMcPedestalNSBAdd

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MMcPedestalNSBAdd : public MTask
{
private:
    Float_t fDnsbPixel;

    Bool_t CheckCamVersion(MParList *pList) const;
    Bool_t CheckRunType(MParList *pList) const;

    Float_t GetDnsb(MParList *pList) const;

    Bool_t ReInit(MParList *pList);

public:
    MMcPedestalNSBAdd(const Float_t difnsb = -1.0,
                      const char *name=NULL, const char *title=NULL);

    ClassDef(MMcPedestalNSBAdd, 0)   // Task which adds the NSB fluctuations to the pedestals rms
};

#endif
