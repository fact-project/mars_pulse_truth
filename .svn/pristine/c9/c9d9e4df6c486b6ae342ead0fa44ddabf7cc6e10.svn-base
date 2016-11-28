#ifndef MARS_MCT1SupercutsCalc
#define MARS_MCT1SupercutsCalc

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

class MParList;
class MHillas;
class MHillasSrc;
class MHillasExt;
class MNewImagePar;
class MMcEvt;
class MCerPhotEvt;
class MGeomCam;
class MHadronness;
class MHMatrix;
class MCT1Supercuts;

class MCT1SupercutsCalc : public MTask
{
private:
    MHillas       *fHil;
    MHillasSrc    *fHilSrc;
    MHillasExt    *fHilExt;
    MNewImagePar  *fNewPar;
    MMcEvt        *fMcEvt;
    MHadronness   *fHadronness; //! output container for hadronness
    MCT1Supercuts *fSuper;      // container for supercut parameters

    TString  fHadronnessName;   // name of container to store hadronness
    TString  fHilName;
    TString  fHilSrcName;
    TString  fHilExtName;
    TString  fNewParName;
    TString  fSuperName;        // name of container for supercut parameters

    Double_t fMm2Deg;           //!

    Int_t     fMap[11];         //!
    MHMatrix *fMatrix;          //!

    Int_t PreProcess(MParList *pList);
    Int_t Process();

    Double_t GetVal(Int_t i) const;

    Double_t CtsMCut(const Double_t* a, Double_t ls, Double_t ct,
                     Double_t ls2, Double_t dd2) const;

public:
    MCT1SupercutsCalc(const char *hilname="MHillas",
                      const char *hilsrcname="MHillasSrc",
                      const char *name=NULL, const char *title=NULL);

    void SetHadronnessName(const TString name) { fHadronnessName = name; }
    TString GetHadronnessName() const { return fHadronnessName; }

    void InitMapping(MHMatrix *mat);
    void StopMapping() { InitMapping(NULL); }


    ClassDef(MCT1SupercutsCalc, 0) // A class to evaluate the Supercuts
};

#endif












