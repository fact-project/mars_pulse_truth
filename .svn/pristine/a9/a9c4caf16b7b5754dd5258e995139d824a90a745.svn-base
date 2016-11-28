#ifndef MARS_MFCT1SelFinal
#define MARS_MFCT1SelFinal

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MSelFinal                                                               //
//                                                                         //
// Class to evaluate final cuts                                            //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

class MHillas;
class MHillasSrc;
class MHadronness;

class MFCT1SelFinal : public MFilter
{
private:
    MHillasSrc  *fHilSrc;       
    MHadronness *fHadronness;       

    TString      fHilName;
    TString      fHilSrcName;
    TString      fHadronnessName;
 
    Float_t      fHadronnessMax;
    Float_t      fAlphaMax;
    Float_t      fDistMax;

    Double_t     fMm2Deg;   // conversion mm to degrees in camera

    Int_t        fCut[4];

    Bool_t       fResult;

    Int_t Set(Int_t rc);

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

    Bool_t IsExpressionTrue() const  { return fResult; }

public:
    MFCT1SelFinal(const char *HilSrcName="MHillasSrc",
                  const char *name=NULL, const char *title=NULL);

    void SetHadronnessName(const TString name) { fHadronnessName = name; }

    void SetCuts(Float_t hadmax, Float_t alphamax, Float_t distmax); 

    ClassDef(MFCT1SelFinal, 0)   // Class to evaluate final cuts
};

#endif











