#ifndef MARS_MSimAtmosphere
#define MARS_MSimAtmosphere

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;
class MAtmosphere;
class MPhotonEvent;
class MCorsikaRunHeader;

class MSimAtmosphere : public MTask
{
private:
    MCorsikaRunHeader *fRunHeader; //! Corsika run header
    MPhotonEvent *fEvt;            //! Event stroing the photons

    MAtmosphere  *fAtmosphere; //! Instance of class describing atmosphere

    TString fFileAerosols;     // Name of file with aersole absorption
    TString fFileOzone;        // Name of file with ozone absorption

    Bool_t fForce;             // Force execution in case efficiencies are already included (CEFFIC)

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);

    // MTask
    Int_t  PreProcess(MParList *pList);
    Bool_t ReInit(MParList *pList);
    Int_t  Process();

public:
    MSimAtmosphere(const char *name=NULL, const char *title=NULL);
    ~MSimAtmosphere();

    ClassDef(MSimAtmosphere, 0) // Simulate the wavelength and height-dependant atmpsheric absorption
};

#endif
