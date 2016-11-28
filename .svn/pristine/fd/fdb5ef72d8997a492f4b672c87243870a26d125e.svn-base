#ifndef MARS_MDataValue
#define MARS_MDataValue

#ifndef MARS_MData
#include "MData.h"
#endif

class TArrayD;

class MDataValue : public MData
{
private:
    Double_t fValue;
    Int_t    fIndex;

public:
    MDataValue(Double_t v = 0, Int_t idx=-1) : fValue(v), fIndex(idx)
    {
    }

    Double_t GetValue() const { return fValue; }
    Bool_t PreProcess(const MParList *plist) { return kTRUE; }

    Bool_t IsValid() const { return kTRUE; }
    Bool_t IsReadyToSave() const { return kFALSE; }

    TString GetRule() const;

    void SetVariables(const TArrayD &arr);

    ClassDef(MDataValue, 1) // MData object corresponding to a single value
};

#endif
