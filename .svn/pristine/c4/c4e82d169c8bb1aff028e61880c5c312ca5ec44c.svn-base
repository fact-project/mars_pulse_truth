#ifndef MARS_MFTriggerLvl1
#define MARS_MFTriggerLvl1

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MFTriggerLvl1                                                           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

class MMcTrig;
class MParList;

class MFTriggerLvl1 : public MFilter
{
private:
    MMcTrig *fMcTrig;
    TString fContName;

    typedef enum { kELowerThan, kEGreaterThan } FilterType_t;
    FilterType_t fFilterType; 

    Bool_t fResult;             //!
    Int_t  fValue;

    void Init(const char type, const Int_t val,
              const char *name, const char *title);

    void StreamPrimitive(std::ostream &out) const;

    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:
    MFTriggerLvl1(const char *cname="MMcTrig", const char type='>', const Int_t val=0,
                  const char *name=NULL, const char *title=NULL);
    MFTriggerLvl1(MMcTrig *mctrig, const char type='>', const Int_t val=0,
                  const char *name=NULL, const char *title=NULL);

    Bool_t IsExpressionTrue() const;

    ClassDef(MFTriggerLvl1, 1) // A Filter for the Level 1 Trigger
};

#endif
