#ifndef MARS_MFilter
#define MARS_MFilter

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;

class MFilter : public MTask
{
private:
    Bool_t fInverted;

    virtual Bool_t IsExpressionTrue() const = 0;

public:
    MFilter(const char *name=NULL, const char *title=NULL);

    virtual TString GetRule() const;
    virtual TString GetDataMember() const { return ""; }

    Bool_t IsConditionTrue() const { return fInverted ? !IsExpressionTrue() : IsExpressionTrue(); }

    void SetInverted(Bool_t i=kTRUE) { fInverted=i; }
    Bool_t IsInverted() const  { return fInverted; }

    void PrintSkipped(UInt_t n, const char *str);

    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    ClassDef(MFilter, 1)		// Abstract base class for the filters
};

#endif
