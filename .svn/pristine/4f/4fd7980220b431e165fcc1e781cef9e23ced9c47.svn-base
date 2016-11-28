#ifndef MARS_MF
#define MARS_MF

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MF                                                                      //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

class MParList;
class TMethodCall;

class MF : public MFilter
{
private:
    static const TString gsDefName;  //!
    static const TString gsDefTitle; //!

    MFilter *fF;                     // Filter
    Bool_t fAllowEmpty;              // Not a Bit to be stored
/*
    Int_t IsRule(TString &txt, TString &rule) const;
    Int_t IsVal(const TString &txt) const;
    Int_t IsAlNum(TString txt) const;

    MFilter *ParseRule(TString &txt, MFilter *filter0, Int_t level) const;
    MFilter *ParseString(TString txt, Int_t level);
  */
    void StreamPrimitive(std::ostream &out) const;

public:
    MF();
    MF(const char *text, const char *name=NULL, const char *title=NULL);
    ~MF();

    // MF
    void SetAllowEmpty(Bool_t b=kTRUE) { fAllowEmpty = b; }
    Bool_t IsAllowEmpty() const { return fAllowEmpty; }

    // MFilter
    TString GetRule() const { return fF ? fF->GetRule() : MFilter::GetRule(); }
    TString GetDataMember() const;

    Bool_t IsExpressionTrue() const;

    // MTask
    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

    // TObject
    void Print(Option_t *opt="") const;

    // MParContainer
    void SetVariables(const TArrayD &arr) { if (fF) fF->SetVariables(arr); }
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    ClassDef(MF, 1) // A Filter for cuts in any data member
};

#endif
