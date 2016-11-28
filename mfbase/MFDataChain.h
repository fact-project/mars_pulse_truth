#ifndef MARS_MFDataChain
#define MARS_MFDataChain

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MFDataChain                                                                 //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MFilter
#include "MFilter.h"
#endif
#ifndef MARS_MDataChain
#include "MDataChain.h"
#endif

class MParList;

class MFDataChain : public MFilter
{
private:
    MDataChain fData;
    MData     *fCond;

    typedef enum { kELowerThan, kEGreaterThan } FilterType_t;
    FilterType_t fFilterType;

    Bool_t  fResult;           //!

    void StreamPrimitive(std::ostream &out) const;

    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:
    MFDataChain(const char *name=NULL, const char *title=NULL);
    MFDataChain(const char *rule, const char type, const Double_t val,
                const char *name=NULL, const char *title=NULL);
    MFDataChain(const char *rule, const char type, const char *cond,
                const char *name=NULL, const char *title=NULL);
    ~MFDataChain();

    Bool_t IsExpressionTrue() const { return fResult; }

    void Print(Option_t *opt = "") const;

    TString GetRule() const;
    TString GetDataMember() const;

    void SetVariables(const TArrayD &arr)
    {
        fData.SetVariables(arr);
        if (fCond)
            fCond->SetVariables(arr);
    }

    ClassDef(MFDataChain, 1) // A Filter for cuts in any data member
};

#endif
