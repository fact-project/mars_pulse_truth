#ifndef MARS_MDataFormula
#define MARS_MDataFormula

#ifndef ROOT_TOrdCollection
#include <TOrdCollection.h>
#endif

#ifndef MARS_MData
#include "MData.h"
#endif

class TFormula;
class MParList;

class MDataFormula : public MData
{
private:
    TFormula      *fFormula;    // TFormula
    TOrdCollection fMembers;	// List of arguments

    TString Parse(const char *rule);

public:
    MDataFormula(const char *rule=0, const char *name=0, const char *title=0);
    MDataFormula(MDataFormula &ts);
    ~MDataFormula();

    Bool_t IsValid() const;// { return fFormula ? kTRUE : kFALSE; }
    Bool_t IsReadyToSave() const;

    Double_t GetValue() const;
    Bool_t PreProcess(const MParList *plist);

    //    void Print(Option_t *opt = "") const;
    TString GetRule() const;
    //    TString GetDataMember() const;

    void SetVariables(const TArrayD &arr);

    ClassDef(MDataFormula, 1) // A concatenation of MData objects by one operator
};

#endif
