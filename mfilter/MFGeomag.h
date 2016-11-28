#ifndef MARS_MFGeomag
#define MARS_MFGeomag

#ifndef MARS_MFilter
#include "MFilter.h"
#endif

class MMcEvt;
class MParList;

class MFGeomag : public MFilter
{
private:
    MMcEvt *fMcEvt;

    Bool_t fResult;    //!
    Bool_t fGammaElectron;  // switches gammas to electrons

    Float_t fRigMin[2*1152];    //tables to contain cut limits
    Float_t fRigMax[2*1152];
    Float_t fProb  [2*1152];

    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:
    MFGeomag(const char *name=NULL, const char *title=NULL);

    void  SetGammElec();    // allows to use gammas like electrons
    Bool_t IsExpressionTrue() const { return fResult; }

    ClassDef(MFGeomag,0) // Filter for MC particles, by geomagnetic field
};

#endif
