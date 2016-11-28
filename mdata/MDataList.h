#ifndef MARS_MDataList
#define MARS_MDataList

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  MDataList                                                              //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TOrdCollection
#include <TOrdCollection.h>
#endif

#ifndef MARS_MData
#include "MData.h"
#endif

class MParList;

class MDataList : public MData
{
private:
    TOrdCollection fMembers;	// Container for the filters

    typedef enum { kENone, kEPlus, kEMinus, kEMult, kEDiv, kEModul, kEPow } SignType_t;
    SignType_t fSign;

    enum { kIsOwner = BIT(14) };

public:
    MDataList();
    MDataList(char type);
    MDataList(MDataList &ts);
    ~MDataList()
    {
        if (TestBit(kIsOwner))
            fMembers.SetOwner();
    }

    Bool_t AddToList(MData *member);
    void SetOwner(Bool_t enable=kTRUE) { enable ? SetBit(kIsOwner) : ResetBit(kIsOwner); }

    Bool_t IsValid() const;// { return fMembers.GetSize() ? kTRUE : kFALSE; }
    Bool_t IsReadyToSave() const;

    Double_t GetValue() const;
    Bool_t PreProcess(const MParList *plist);

//    void Print(Option_t *opt = "") const;
    TString GetRule() const;
    TString GetDataMember() const;

    void SetVariables(const TArrayD &arr);

    ClassDef(MDataList, 1) // A concatenation of MData objects by one operator
};

#endif
