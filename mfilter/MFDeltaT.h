#ifndef MARS_MFDeltaT
#define MARS_MFDeltaT

#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

#ifndef MARS_MTime
#include "MTime.h"
#endif

class MTime;
class MParList;

class MFDeltaT : public MFilter
{
private:
    MTime  *fTime;     //!
    TString fNameTime;

    Float_t fUpperLimit;

    MTime  fLastTime;  //!
    Bool_t fResult;    //!

    TArrayI fErrors;

    void Init(const char *name, const char *title, Float_t max=0.5, const char *time="MTime");

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

public:
    MFDeltaT(Float_t max, const char *time="MTime", const char *name=0, const char *title=0);
    MFDeltaT(const char *name=0, const char *title=0);

    void SetUpperLimit(Float_t max) { fUpperLimit=max; }
    Float_t GetUpperLimit() const { return fUpperLimit; }

    Bool_t IsExpressionTrue() const;

    ClassDef(MFDeltaT, 1) // A Filter for time differences of consecutive events
};

#endif
