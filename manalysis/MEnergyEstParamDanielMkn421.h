#ifndef MARS_MEnergyEstParamDanielMkn421
#define MARS_MEnergyEstParamDanielMkn421

#ifndef MARS_MTask
#include "MTask.h"
#endif

#ifndef ROOT_TArrayD
#include <TArrayD.h>
#endif

class MHillasSrc;
class MEnergyEst;
class MHillas;
class MNewImagePar;
class MMcEvt;
class MHMatrix;

class MEnergyEstParamDanielMkn421 : public MTask
{
private:
    Int_t     fMap[100]; // FIXME!

    MHMatrix *fMatrix;

    TList    *fPairs;

    TList    *fHillasSrc;
    TList    *fEnergy;

    MHillas       *fHillas;
    MNewImagePar  *fNewImagePar;

    TString   fHillasName;

    TArrayD   fA;
    TArrayD   fB;

    MMcEvt   *fMc;

    void InitCoefficients();

    Double_t GetVal(Int_t i) const;
    Double_t fMm2Deg;

    Int_t PreProcess(MParList *plist);
    Int_t Process();

public:
    MEnergyEstParamDanielMkn421(const char *hil="MHillas", const char *name=NULL, const char *title=NULL);
    ~MEnergyEstParamDanielMkn421();

    void Add(const TString hillas, const TString energy="MEnergyEst");

    void InitMapping(MHMatrix *mat);
    void StopMapping();

    Int_t GetNumCoeff() const { return fA.GetSize()+fB.GetSize(); }

    void SetCoeff(const TArrayD &arr);
    void SetCoeffA(const TArrayD &arr);
    void SetCoeffB(const TArrayD &arr);

    Double_t GetCoeff(Int_t i) { return i<fA.GetSize()? fA[i] : fB[i-fA.GetSize()]; }

    void Print(Option_t *o=NULL) const;

    ClassDef(MEnergyEstParamDanielMkn421, 0) // Task to estimate the energy
};

#endif








