#ifndef MARS_MFSupercuts
#define MARS_MFSupercuts

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

class MParList;
class MHillas;
class MHillasSrc;
class MHillasExt;
class MNewImagePar;
class MCerPhotEvt;
class MGeomCam;
class MHadronness;
class MHMatrix;

class MFSupercuts : public MFilter
{
private:
    MHillas       *fHil;        //!
    MHillasSrc    *fHilSrc;     //!
    MHillasExt    *fHilExt;     //!
    MNewImagePar  *fNewPar;     //!

    Double_t       fMm2Deg;     //!
    Bool_t fResult;             //!

    Int_t     fMap[11];         //!
    MHMatrix *fMatrix;          //!

    TArrayD fVariables;

    Int_t PreProcess(MParList *pList);
    Int_t Process();

    Bool_t IsExpressionTrue() const { return fResult; }

    Double_t GetVal(Int_t i) const;

    Double_t CtsMCut(const Double_t* a, Double_t ls, Double_t ct,
                     Double_t ls2, Double_t dd2) const;

    TString GetDataMember() const;
    //TString GetRule() const { return "<MFSupercuts>"; }

public:
    MFSupercuts(const char *name=NULL, const char *title=NULL);

    void InitMapping(MHMatrix *mat);
    void StopMapping() { InitMapping(NULL); }

    void SetVariables(const TArrayD &arr);

    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    Bool_t CoefficentsRead(const char *fname);
    Bool_t CoefficentsWrite(const char *fname) const;

    ClassDef(MFSupercuts, 0) // A filter to evaluate the Supercuts
};

#endif
