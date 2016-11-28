#ifndef MARS_MFDataMember
#define MARS_MFDataMember

#ifndef MARS_MFilter
#include "MFilter.h"
#endif
#ifndef MARS_MDataMember
#include "MDataMember.h"
#endif

class MParList;

class MFDataMember : public MFilter
{
private:
    MDataMember fData;

    typedef enum { kELowerThan, kEGreaterThan, kEEqual } FilterType_t;
    FilterType_t fFilterType;

    Bool_t  fResult;           //!
    Double_t fValue;

    void StreamPrimitive(std::ostream &out) const;

    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:
    enum {
        kIsInt = BIT(14)
    };
    MFDataMember(const char *member, const char type, const Double_t val,
                 const char *name=NULL, const char *title=NULL);

    Bool_t IsExpressionTrue() const { return fResult; }

    void Print(Option_t *opt = "") const;

    TString GetRule() const;
    TString GetDataMember() const;

    void SetVariables(const TArrayD &arr) { fData.SetVariables(arr); }

    ClassDef(MFDataMember, 1) // A Filter for cuts in any data member
};

#endif
