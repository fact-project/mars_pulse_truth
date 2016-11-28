#ifndef MARS_MEnergyEstParam
#define MARS_MEnergyEstParam

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

class MHillasSrc;
class MEnergyEst;
class MHillas;
class MMcEvt;
class MHMatrix;

class MEnergyEstParam : public MTask
{
private:
    Int_t     fMap[100]; // FIXME!

    MHMatrix *fMatrix;

    TList    *fPairs;

    TList    *fHillasSrc;
    TList    *fEnergy;

    MHillas  *fHillas;
    TString   fHillasName;

    TArrayD   fA;
    TArrayD   fB;

    MMcEvt   *fMc;

    void InitCoefficients();

    Double_t GetVal(Int_t i) const;

    Int_t PreProcess(MParList *plist);
    Int_t Process();

public:
    MEnergyEstParam(const char *hil="MHillas", const char *name=NULL, const char *title=NULL);
    ~MEnergyEstParam();

    void Add(const TString hillas, const TString energy="MEnergyEst");

    void InitMapping(MHMatrix *mat);
    void StopMapping();

    Int_t GetNumCoeff() const { return fA.GetSize()+fB.GetSize(); }

    void SetCoeff(const TArrayD &arr);
    void SetCoeffA(const TArrayD &arr);
    void SetCoeffB(const TArrayD &arr);

    void SetVariables(const TArrayD &arr) { SetCoeff(arr); }

    Double_t GetCoeff(Int_t i) { return i<fA.GetSize()? fA[i] : fB[i-fA.GetSize()]; }

    void Print(Option_t *o=NULL) const;

    ClassDef(MEnergyEstParam, 0) // Task to estimate the energy
};

#endif
