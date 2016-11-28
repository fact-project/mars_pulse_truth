#ifndef MARS_MFCT1SelStandard
#define MARS_MFCT1SelStandard

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MFCT1SelStandard                                                        //
//                                                                         //
// Class to evaluate standard cuts                                         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

class MHillas;
class MHillasSrc;
class MNewImagePar;

class MFCT1SelStandard : public MFilter
{
private:
    MHillas      *fHil;
    MHillasSrc   *fHilSrc;
    MNewImagePar *fNewImgPar;

    TString      fHilName;
    TString      fHilSrcName;
    TString      fImgParName;

    Float_t      fUsedPixelsMax;
    Float_t      fCorePixelsMin;
    Float_t      fSizeMin;
    Float_t      fDistMin;
    Float_t      fDistMax;
    Float_t      fLengthMin;
    Float_t      fWidthMin;

    Double_t     fMm2Deg;    // conversion mm to degrees in camera

    Int_t        fCut[5];

    Bool_t       fResult;

    Int_t PreProcess(MParList *pList);
    Int_t Process();
    Int_t PostProcess();

    Bool_t IsExpressionTrue() const  { return fResult; }

    Bool_t Set(Int_t rc);

public:
    MFCT1SelStandard(const char *HilSrcName="MHillasSrc",
                     const char *name=NULL, const char *title=NULL);

    void SetHillasName(const char *name) { fHilName = name; }
    void SetImgParName(const char *name) { fImgParName = name; }

    void SetCuts(Float_t usedpixelsmax, Float_t corepixelsmin,
                 Float_t sizemin, Float_t distmin, Float_t distmax,
                 Float_t lengthmin, Float_t widthmin);

    ClassDef(MFCT1SelStandard, 0)   // Class to evaluate standard cuts
};

#endif











