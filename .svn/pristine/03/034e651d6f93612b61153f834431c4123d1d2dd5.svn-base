#ifndef MARS_MData
#define MARS_MData

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  MData                                                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MParList;

class MData : public MParContainer
{
public:
    virtual Double_t GetValue() const = 0;
    virtual Bool_t   IsValid() const = 0;
    virtual Bool_t   PreProcess(const MParList *plist) = 0;
    virtual TString  GetRule() const = 0;
    virtual TString  GetDataMember() const { return ""; }

    Double_t operator()() { return GetValue(); }

    void Print(Option_t *opt = "") const; //*MENU*
    Bool_t AsciiWrite(std::ostream &out) const;

    ClassDef(MData, 0) // A Base class for a generalized value
};

#endif
