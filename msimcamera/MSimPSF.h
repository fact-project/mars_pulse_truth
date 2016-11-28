#ifndef MARS_MSimPSF
#define MARS_MSimPSF

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;
class MPhotonEvent;

class MSimPSF : public MTask
{
private:
    MPhotonEvent *fEvt;   //! Event stroing the photons

    Double_t      fSigma; //  Gaussian sigma of the smearout

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    // MTask
    Int_t PreProcess(MParList *pList);
    Int_t Process();


public:
    MSimPSF(const char *name=NULL, const char *title=NULL);

    ClassDef(MSimPSF, 0) // Task to do a naiv simulation of the psf by smearout in the camera plane
};
    
#endif

