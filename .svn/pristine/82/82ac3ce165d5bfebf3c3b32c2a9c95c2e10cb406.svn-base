#ifndef MARS_MDataPhrase
#define MARS_MDataPhrase

#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif

#ifndef MARS_MData
#include "MData.h"
#endif

#ifndef MARS_MArrayD
#include "MArrayD.h"
#endif

class TFormula;
class MParList;

class MDataPhrase : public MData
{
private:
    TFormula *fFormula;  // TFormula (its a pointer to be able to have no expression)
    TObjArray fMembers;	 // List of arguments

    MArrayD   fStorage;  //! Temporary storage used in GetValue (only!)

    Int_t   CheckForVariable(const TString &phrase, Int_t idx);
    Int_t   Substitute(TString &phrase, const TString &expr, Int_t idx) const;
    TString Parse(TString phrase);

public:
    MDataPhrase(const char *rule=0, const char *name=0, const char *title=0);
    MDataPhrase(MDataPhrase &ts);
    ~MDataPhrase();

    // TObject
    void     Clear(Option_t *o="");

    // MParContainer
    Bool_t   IsReadyToSave() const;
    void     SetVariables(const TArrayD &arr);
    Int_t    ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    // MData
    Bool_t   IsValid() const;
    Bool_t   HasValidRule() const { return fFormula ? kTRUE : kFALSE; }
    Double_t GetValue() const { return Eval((Double_t*)NULL); }
    Double_t Eval(const Double_t *x) const;
    Double_t Eval(Double_t x, Double_t y=0, Double_t z=0, Double_t t=0) const
    {
        const Double_t xx[4] = { x, y, z, t };
        return Eval(xx);
    }
    TString  GetRule() const;
    TString  GetRuleRaw() const;
    Bool_t   PreProcess(const MParList *plist);
    //    void Print(Option_t *opt = "") const;
    //    TString GetDataMember() const;

    // MDataPhrase
    Bool_t   GetBool() const;
    Bool_t   SetRule(const TString &phrase);

    ClassDef(MDataPhrase, 1) // MDataPhrase is basically a mapper for TFormula supporting Mars features
};

#endif
