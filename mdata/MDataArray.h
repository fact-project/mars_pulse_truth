#ifndef MARS_MDataArray
#define MARS_MDataArray

/////////////////////////////////////////////////////////////////////////////
//
//  MDataArray
//
/////////////////////////////////////////////////////////////////////////////
#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif

#ifndef ROOT_TVector
#include <TVector.h>
#endif

class MData;
class MParList;

class MDataArray : public MParContainer
{
    TObjArray fList;

    void StreamPrimitive(std::ostream &out) const;

public:
    MDataArray(const char *name=NULL, const char *title=NULL);
    MDataArray(MDataArray &a, const char *name=NULL, const char *title=NULL);

    void AddEntry(const TString rule);
    void AddEntry(MData *data);

    Int_t FindRule(const char *rule) const;

    MData &operator[](Int_t i) const;
    Double_t operator()(Int_t i) const;

    void GetData(TVector &v) const;

    void operator>>(TVector &v) const
    {
        GetData(v);
    }

    TString GetRule(int i) const;

    Bool_t PreProcess(const MParList *plist);

    TString GetDataMember() const;

    void Print(Option_t *opt = "") const; //*MENU*
    Bool_t AsciiWrite(std::ostream &out) const;

    Int_t GetNumEntries() const { return fList.GetEntries(); }

    void Clear(Option_t *option="")  { fList.Clear(option); }
    void Delete(Option_t *option="") { fList.Delete(option); }

    void SetVariables(const TArrayD &arr);

    ClassDef(MDataArray, 1) // An array of MData containers
};

#endif
