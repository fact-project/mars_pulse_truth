#ifndef MARS_MSimAbsorption
#define MARS_MSimAbsorption

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;
class MParSpline;
class MPhotonEvent;
class MCorsikaRunHeader;
class MCorsikaEvtHeader;

class MSimAbsorption : public MTask
{
private:
    MPhotonEvent      *fEvt;       //! Event stroing the photons
    MCorsikaRunHeader *fRunHeader; //! Corsika run header
    MCorsikaEvtHeader *fHeader;    //! Header storing event information

    MParSpline        *fSpline;    //! Spline to interpolate wavelength or incident angle

    TString fParName;            // Container name of the spline containing the curve
    Bool_t  fUseTheta;           // Switches between using wavelength or incident angle
    Bool_t  fForce;              // Force execution even if corsika already simulated the efficiencies

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    // MTask
    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Process();

public:
    MSimAbsorption(const char *name=NULL, const char *title=NULL);

    // MSimAbsorption
    void SetParName(const char *name) { fParName=name; }

    void SetUseTheta(Bool_t b=kTRUE) { fUseTheta = b; }
    void SetForce(Bool_t b=kTRUE) { fForce = b; }

    ClassDef(MSimAbsorption, 0) // Task to calculate wavelength or incident angle dependent absorption
};

#endif
