#ifndef MARS_MFTriggerLvl2
#define MARS_MFTriggerLvl2

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MFTriggerLvl2
//
// auth. A.stamerra 
// created 30.01.03                                                       //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

class MMcTrig;
class MParList;
class MMcTriggerLvl2;

class MFTriggerLvl2 : public MFilter
{
private:

    TString fContName;

    MMcTriggerLvl2 *fcell;

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

    MFTriggerLvl2(const char *cname="MMcTriggerLvl2", const char type='>', const Int_t val=0,
                  const char *name=NULL, const char *title=NULL);
    MFTriggerLvl2(MMcTriggerLvl2 *triglvl2, const char type='>', const Int_t val=0,
                  const char *name=NULL, const char *title=NULL);

    Bool_t IsExpressionTrue() const;

    ClassDef(MFTriggerLvl2, 1) // A Filter for the Level 2 Trigger
};

#endif
