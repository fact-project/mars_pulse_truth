#ifndef MARS_MDataMember
#define MARS_MDataMember

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  MDataMember                                                            //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MData
#include "MData.h"
#endif

class MDataMember : public MData
{
private:
    TString fDataMember;

    MParContainer *fObject; //! A pointer to the container from the paramater list
    TMethodCall   *fCall;   //! The corresponding method call to the member function

    enum { kIsInt = BIT(14) };

    Bool_t CheckGet() const;

public:
    MDataMember(const char *member=NULL) : fObject(NULL), fCall(NULL)
    {
        fDataMember = member;
    }

    MDataMember(MParContainer *obj, TMethodCall *call);
    MDataMember(MParContainer *obj, const TString call);

    Double_t GetValue() const;
    const char *GetString() const;
    Bool_t PreProcess(const MParList *plist);

    Bool_t IsValid() const { return fCall ? kTRUE : kFALSE; }
    Bool_t IsReadyToSave() const;

    //void Print(Option_t *opt = "") const;
    TString GetRule() const;
    TString GetDataMember() const;

    Bool_t IsInt() const { return TestBit(kIsInt); }

    void SetVariables(const TArrayD &arr) { }

    ClassDef(MDataMember, 2) // MData object corresponding to a single data member of a Mars container
};

#endif
