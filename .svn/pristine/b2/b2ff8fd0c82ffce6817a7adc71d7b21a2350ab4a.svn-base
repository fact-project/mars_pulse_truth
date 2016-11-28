#ifndef MARS_MSimExcessNoise
#define MARS_MSimExcessNoise

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;
class MPhotonEvent;

class MSimExcessNoise : public MTask
{
private:
    MPhotonEvent *fEvt;     //! Event storing the photons

    Double_t fExcessNoise;

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    // MTask
    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:
    MSimExcessNoise(const char *name=NULL, const char *title=NULL);

    ClassDef(MSimExcessNoise, 0) // Task to simulate the excess dependant noise (conversion photon to signal height)
};

#endif
