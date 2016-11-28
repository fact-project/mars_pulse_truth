#ifndef MARS_MDataElement
#define MARS_MDataElement

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  MDataElement                                                           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#ifndef MARS_MData
#include "MData.h"
#endif

class MHMatrix;

class MDataElement : public MData
{
private:
    TString   fMatrixName;
    Int_t     fNumCol;

    MHMatrix *fMatrix;

public:
    MDataElement(const char *member=NULL, Int_t col=-1);
    MDataElement(MHMatrix *mat, Int_t col=-1);

    Double_t GetValue() const;
    Bool_t PreProcess(const MParList *plist);

    Bool_t IsValid() const { return kTRUE; }
    Bool_t IsReadyToSave() const;

    //void Print(Option_t *opt = "") const;
    TString GetRule() const;

    Double_t operator()() { return GetValue(); }

    void SetVariables(const TArrayD &arr) { }

    ClassDef(MDataElement, 1) // MData object corresponding to a element of an MHMatrix
};

#endif
