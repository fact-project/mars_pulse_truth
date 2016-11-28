#ifndef MARS_MFDataPhrase
#define MARS_MFDataPhrase

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

class MData;
class MParList;
class TMethodCall;

class MFDataPhrase : public MFilter
{
private:
    static const TString gsDefName;  //!
    static const TString gsDefTitle; //!

    MData *fData;
    Bool_t fAllowEmpty;              // Not a Bit to be stored
    Bool_t fDefault;                 // Default for empty case

    Bool_t fResult; //!

    Int_t IsRule(TString &txt, TString &rule) const;
    Int_t IsVal(const TString &txt) const;
    Int_t IsAlNum(TString txt) const;

    MFilter *ParseRule(TString &txt, MFilter *filter0, Int_t level) const;
    MFilter *ParseString(TString txt, Int_t level);

    void StreamPrimitive(std::ostream &out) const;

public:
    MFDataPhrase();
    MFDataPhrase(const char *text, const char *name=NULL, const char *title=NULL);
    ~MFDataPhrase();

    // MF
    void SetAllowEmpty(Bool_t b=kTRUE) { fAllowEmpty = b; }
    Bool_t IsAllowEmpty() const { return fAllowEmpty; }

    void SetDefault(Bool_t b) { fDefault = b;    }
    Bool_t GetDefault() const { return fDefault; }

    // MFilter
    TString GetRule() const;
    TString GetDataMember() const;

    Bool_t IsExpressionTrue() const;

    // MTask
    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

    // TObject
    void Print(Option_t *opt="") const;

    // MParContainer
    void SetVariables(const TArrayD &arr);
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    ClassDef(MFDataPhrase, 2) // A Filter for cuts in any data member
};

#endif
