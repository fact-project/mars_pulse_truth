#ifndef MARS_MSimRays
#define MARS_MSimRays

#ifndef MARS_MTask
#include "MTask.h"
#endif

class MParList;
class MGeomCam;
class MPointingPos;
class MPhotonEvent;
class MCorsikaEvtHeader;

class MReflector;

class MSimRays : public MTask
{
private:
    MPhotonEvent  *fEvt;        //! Event storing the photons
    MReflector    *fReflector;  //! Geometry of the reflector
    MPointingPos  *fPointPos;   //! Direction the telescope is pointing to
    MPointingPos  *fSource;     //! Direction the rays are coming from relative to the telescope

    UInt_t   fNumPhotons;       // Number of photons to be produced
    Double_t fHeight;           //[km] Height of point source (<=0 means infinity)

    TString fNameReflector;
    TString fNamePointPos;
    TString fNameSource;

    // MParContainer
    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print);

    // MTask
    Int_t PreProcess(MParList *pList);
    Int_t Process();

public:
    MSimRays(const char *name=NULL, const char *title=NULL);

    void SetNameReflector(const char *name="MReflector")  { fNameReflector = name; }
    void SetNamePointPos(const char *name="MPointingPos") { fNamePointPos  = name; }
    void SetNameSource(const char *name="Source")         { fNameSource    = name; }

    void SetHeight(Double_t h=-1) { fHeight=h; }
    void SetNumPhotons(UInt_t n)  { fNumPhotons=n; }

    // MSimRays
    ClassDef(MSimRays, 0) // Task to simulate a source for rays
};

#endif
