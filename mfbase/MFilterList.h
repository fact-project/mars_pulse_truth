#ifndef MARS_MFilterList
#define MARS_MFilterList

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  MFilterList                                                            //
//                                                                         //
//  List of several filters                                                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TOrdCollection
#include <TOrdCollection.h>
#endif
#ifndef MARS_MFilter
#include "MFilter.h"
#endif

class MParList;

class MFilterList : public MFilter
{
private:
    TOrdCollection fFilters;	// Container for the filters

    typedef enum { kEAnd, kEOr, kEXor, kELAnd, kELOr } FilterType_t;
    FilterType_t fFilterType;

    enum { kIsOwner = BIT(14) };

    void StreamPrimitive(std::ostream &out) const;

    void Init(const char *name, const char *title);

public:
    MFilterList(const char *type="&&", const char *name=NULL, const char *title=NULL);
    MFilterList(MFilter *f, const char *name=NULL, const char *title=NULL);
    MFilterList(MFilterList &ts);
    ~MFilterList()
    {
        if (TestBit(kIsOwner))
            fFilters.SetOwner();
    }

    Bool_t AddToList(MFilter *filter);
    Bool_t AddToList(const TCollection &col);
    void SetOwner(Bool_t enable=kTRUE) { enable ? SetBit(kIsOwner) : ResetBit(kIsOwner); }

    Bool_t IsExpressionTrue() const;

    void Print(Option_t *opt = "") const;

    TString GetRule() const { return GetRule(""); }
    TString GetRule(Option_t *opt) const;
    TString GetDataMember() const;

    Int_t GetNumEntries() const { return fFilters.GetEntries(); }

    Bool_t ReInit(MParList *plist);
    Int_t  PreProcess(MParList *pList);
    Int_t  Process();
    Int_t  PostProcess();

    void SetAccelerator(Byte_t acc=kAccStandard);

    void SetVariables(const TArrayD &arr);

    ClassDef(MFilterList, 1)		// List to combine several filters logically
};

#endif
